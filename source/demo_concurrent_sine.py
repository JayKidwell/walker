#!/usr/bin/env python3
"""
Demo: Concurrent sine wave sequences on multiple servos
Uses the PreciseServoSequencer library to run three simultaneous sine waves
with different frequencies on separate servo channels.
"""

from typing import List, Tuple, Dict
import signal
import sys
from services.ServoSequencer import ServoSequencer


# Configuration constants
FRAME_RATE = 60
# All 4 legs - leg[0]: 0,1,2  leg[1]: 4,5,6  leg[2]: 8,9,10  leg[3]: 12,13,14
SERVO_CHANNELS = [4, 5, 6]
SEQUENCE_DURATIONS = [3.0, 3.0, 3.0]
# SERVO_CHANNELS = [0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14]
# SEQUENCE_DURATIONS = [1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 2.0, 3.0, 1.0, 2.0, 3.0]
SERVO_CENTER = 90  # Center position in degrees
SERVO_AMPLITUDE = 60  # Sweep amplitude in degrees
# Duration pattern repeated for each leg (1.0s, 2.0s, 3.0s per leg)
USE_REALTIME = True  # Enable real-time priority for better timing


class ConcurrentSineDemo:
    """Manages concurrent sine wave demonstration on multiple servos."""
    
    def __init__(
        self,
        channels: List[int] = SERVO_CHANNELS,
        frame_rate: int = FRAME_RATE,
        center: float = SERVO_CENTER,
        amplitude: float = SERVO_AMPLITUDE,
    ):
        """
        Initialize the demo with configurable parameters.
        
        Args:
            channels: List of servo channel numbers to use
            frame_rate: Sequencer frame rate in Hz
            center: Center angle for sine waves in degrees
            amplitude: Amplitude of sine waves in degrees
        """
        self.channels = channels
        self.center = center
        self.amplitude = amplitude
        self.sequencer = ServoSequencer(channels=16, frame_rate=frame_rate)
        self.threads: List = []
        self.stop_flags: List[Dict[str, bool]] = []
        
        # Setup signal handler for graceful shutdown
        signal.signal(signal.SIGINT, self._signal_handler)
    
    def _signal_handler(self, signum, frame):
        """Handle interrupt signal for graceful shutdown."""
        print("\n\n⏹ Interrupt received, stopping all sequences...")
        self.cleanup()
        sys.exit(0)
    
    def generate_sequences(
        self, durations: List[float]
    ) -> List[List[Tuple[float, int, float]]]:
        """
        Generate sine wave sequences for all configured servos.
        
        Args:
            durations: List of durations for each sequence
            
        Returns:
            List of sequence data for each servo
        """
        if len(durations) != len(self.channels):
            raise ValueError(
                f"Number of durations ({len(durations)}) must match "
                f"number of channels ({len(self.channels)})"
            )
        
        sequences = []
        print("\n📊 Generating sequences...")
        
        for i, (channel, duration) in enumerate(zip(self.channels, durations)):
            sequence = self.sequencer.generate_sine_sequence(
                servo_channel=channel,
                duration=duration,
                center=self.center,
                amplitude=self.amplitude
            )
            sequences.append(sequence)
            print(
                f"  Sequence {i+1}: {len(sequence)} frames over "
                f"{duration:.1f} seconds (servo {channel})"
            )
        
        return sequences
    
    def execute_sequences_concurrent(
        self,
        sequences: List[List[Tuple[float, int, float]]],
        use_realtime: bool = USE_REALTIME,
    ) -> None:
        """
        Execute all sequences concurrently in separate threads.
        
        Args:
            sequences: List of pre-generated sequences
            use_realtime: Whether to use real-time thread priority
        """
        print("\n" + "=" * 60)
        print("Running all sequences CONCURRENTLY...")
        print("=" * 60)
        print(f"(All {len(sequences)} servos will move simultaneously)")
        print()
        
        # Start all sequences in parallel threads
        for i, sequence in enumerate(sequences):
            thread, stop_flag = self.sequencer.execute_sequence_threaded(
                sequence, 
                sequence_id=f"{SEQUENCE_DURATIONS[i]:.1f}s",
                use_realtime=use_realtime
            )
            self.threads.append(thread)
            self.stop_flags.append(stop_flag)
        
        # Wait for all sequences to complete
        print("⏳ Waiting for all sequences to complete...")
        self.sequencer.wait_threads(self.threads)
        
        print("\n" + "=" * 60)
        print("✓ All sequences complete!")
        print("=" * 60)
    
    def cleanup(self) -> None:
        """Stop all running sequences and cleanup resources."""
        if self.stop_flags:
            self.sequencer.stop_all(self.stop_flags)
            self.stop_flags.clear()
        if self.threads:
            self.threads.clear()
    
    def run(self, durations: List[float] = SEQUENCE_DURATIONS) -> None:
        """
        Run the complete demonstration.
        
        Args:
            durations: List of durations for each sequence in seconds
        """
        try:
            sequences = self.generate_sequences(durations)
            self.execute_sequences_concurrent(sequences)
        except KeyboardInterrupt:
            print("\n\n⏹ Interrupted by user")
            self.cleanup()
        except Exception as e:
            print(f"\n❌ Error: {e}")
            self.cleanup()
            raise


def main():
    """Entry point for the concurrent sine wave demo."""
    print("=" * 60)
    print("Precise Servo Sequencer - Concurrent Sine Wave Demo")
    print("=" * 60)
    
    demo = ConcurrentSineDemo()
    demo.run()


if __name__ == "__main__":
    main()
