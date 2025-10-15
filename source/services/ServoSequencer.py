#!/usr/bin/env python3
"""
Precise Servo Sequencer for Raspberry Pi with Adafruit ServoKit
Implements compensated timing, real-time priority, and pre-computed sequences
"""

import os
import time
import math
import threading
from adafruit_servokit import ServoKit


class ServoSequencer:
    """
    Executes pre-computed servo sequences with drift-compensated timing
    and optional real-time thread priority.
    """
    
    def __init__(self, channels=16, frame_rate=60):
        """
        Initialize the servo sequencer.
        
        Args:
            channels: Number of servo channels (8 or 16)
            frame_rate: Updates per second (default 60Hz)
        """
        self.kit = ServoKit(channels=channels)
        self.frame_rate = frame_rate
        self.frame_interval = 1.0 / frame_rate
        self.threads = []
        
        # Servo angle limits for safety
        self.servo_min = 0
        self.servo_max = 180
        
    def set_realtime_priority(self, priority=50):
        """
        Request real-time scheduling priority for timing-critical operations.
        Requires running with sudo or CAP_SYS_NICE capability.
        
        Args:
            priority: SCHED_FIFO priority (1-99, higher = more priority)
        """
        try:
            param = os.sched_param(priority)
            os.sched_setscheduler(0, os.SCHED_FIFO, param)
            print(f"✓ Real-time priority set to {priority}")
            return True
        except PermissionError:
            print("⚠ Warning: Unable to set real-time priority (need sudo)")
            print("  Timing will be less precise but still functional")
            return False
        except AttributeError:
            print("⚠ Warning: Real-time scheduling not supported on this system")
            return False
    
    def generate_sine_sequence(self, servo_channel, duration, center=90, amplitude=45):
        """
        Generate a pre-computed sine wave sequence for a servo.
        
        Args:
            servo_channel: Servo channel number (0-15)
            duration: Duration of the sequence in seconds
            center: Center angle of the sine wave (default 90°)
            amplitude: Amplitude of the sine wave (default ±45°)
            
        Returns:
            List of (timestamp, servo_channel, angle) tuples
        """

        print(f"generate_sine_sequence: channel={servo_channel}, duration={duration}, center={center}, amplitude={amplitude}")

        num_frames = int(duration * self.frame_rate)
        sequence = []
        
        for frame in range(num_frames):
            # Calculate time offset for this frame
            t = frame * self.frame_interval
            
            # Calculate angle using sine wave (0 to 2π over duration)
            angle_radians = (frame / num_frames) * 2 * math.pi
            angle = center + amplitude * math.sin(angle_radians)
            
            # Clamp to safe servo limits
            angle = max(self.servo_min, min(self.servo_max, angle))

            print(f"Sequence {frame}: {angle}")

            sequence.append((t, servo_channel, angle))
        
        return sequence
    
    def precompute_sequence(self, moves):
        """
        Pre-compute servo objects and angles to minimize execution overhead.
        
        Args:
            moves: List of (timestamp, servo_channel, angle) tuples
            
        Returns:
            List of (timestamp, servo_object, angle) tuples
        """
        return [
            (timestamp, self.kit.servo[channel], angle)
            for timestamp, channel, angle in moves
        ]
    
    def execute_sequence(self, moves, sequence_id="", use_realtime=True, stop_flag=None):
        """
        Execute a pre-computed sequence with drift compensation.
        
        Args:
            moves: List of (timestamp, servo_channel, angle) tuples
            sequence_id: Identifier for this sequence (for logging)
            use_realtime: Whether to request real-time priority
            stop_flag: Dictionary with 'running' key to control this sequence
        """
        if use_realtime:
            self.set_realtime_priority()
        
        # Create local stop flag if not provided
        if stop_flag is None:
            stop_flag = {'running': True}
        
        # Pre-compute to minimize overhead (Option 4)
        prepared_moves = self.precompute_sequence(moves)
        
        start_time = time.perf_counter()
        
        seq_label = f"[Seq {sequence_id}]" if sequence_id else ""
        print(f"\n▶ {seq_label} Executing sequence with {len(prepared_moves)} moves...")
        print(f"  Frame rate: {self.frame_rate} Hz")
        print(f"  Duration: {moves[-1][0]:.2f} seconds\n")
        
        for i, (target_time, servo_obj, angle) in enumerate(prepared_moves):
            if not stop_flag['running']:
                print(f"⏹ {seq_label} Sequence stopped")
                break
            
            # Calculate absolute target time
            absolute_target = start_time + target_time
            
            # Drift compensation (Option 2)
            now = time.perf_counter()
            sleep_time = absolute_target - now
            
            # Sleep if we're ahead of schedule
            if sleep_time > 0.001:  # Only sleep if > 1ms
                time.sleep(sleep_time * 0.95)  # Sleep 95% to avoid overshooting
                
                # Busy-wait for final precision
                while time.perf_counter() < absolute_target:
                    pass
            elif sleep_time < -0.01:  # More than 10ms behind
                print(f"⚠ {seq_label} Warning: Frame {i} is {-sleep_time*1000:.1f}ms behind schedule")
            
            # Execute move
            servo_obj.angle = angle
        
        actual_duration = time.perf_counter() - start_time
        print(f"✓ {seq_label} Sequence complete")
        print(f"  Expected: {moves[-1][0]:.3f}s | Actual: {actual_duration:.3f}s")
        print(f"  Drift: {(actual_duration - moves[-1][0])*1000:.2f}ms\n")
        
        stop_flag['running'] = False
    
    def execute_sequence_threaded(self, moves, sequence_id="", use_realtime=True):
        """
        Execute sequence in a background thread (Option 3 - threading).
        
        Args:
            moves: List of (timestamp, servo_channel, angle) tuples
            sequence_id: Identifier for this sequence (for logging)
            use_realtime: Whether to request real-time priority
            
        Returns:
            Tuple of (thread object, stop_flag dict)
        """
        # Create individual stop flag for this thread
        stop_flag = {'running': True}
        
        thread = threading.Thread(
            target=self.execute_sequence,
            args=(moves, sequence_id, use_realtime, stop_flag),
            daemon=False
        )
        thread.start()
        return thread, stop_flag
    
    def stop_all(self, thread_stop_flags):
        """
        Stop all currently running sequences.
        
        Args:
            thread_stop_flags: List of stop_flag dictionaries
        """
        for stop_flag in thread_stop_flags:
            stop_flag['running'] = False
    
    def wait_threads(self, threads):
        """
        Wait for multiple threads to complete.
        
        Args:
            threads: List of thread objects
        """
        for thread in threads:
            thread.join()
