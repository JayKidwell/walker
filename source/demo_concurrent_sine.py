#!/usr/bin/env python3
"""
Demo: Concurrent sine wave sequences on multiple servos
Uses the PreciseServoSequencer library to run three simultaneous sine waves
"""

from precise_servo_sequencer import PreciseServoSequencer


def main():
    """
    Demo: Run three sine wave sequences concurrently on different servos.
    """
    print("=" * 60)
    print("Precise Servo Sequencer - Concurrent Sine Wave Demo")
    print("=" * 60)
    
    # Initialize sequencer at 60 Hz
    sequencer = PreciseServoSequencer(channels=16, frame_rate=60)
    
    # Use three different servo channels for concurrent execution
    SERVO_CHANNELS = [0, 1, 2]  # Change these to match your setup
    
    # Generate the three predefined sequences on different servos
    print("\n📊 Generating sequences...")
    sequence_1s = sequencer.generate_sine_sequence(
        servo_channel=SERVO_CHANNELS[0],
        duration=1.0,
        center=90,
        amplitude=45
    )
    print(f"  Sequence 1: {len(sequence_1s)} frames over 1.0 seconds (servo {SERVO_CHANNELS[0]})")
    
    sequence_2s = sequencer.generate_sine_sequence(
        servo_channel=SERVO_CHANNELS[1],
        duration=2.0,
        center=90,
        amplitude=45
    )
    print(f"  Sequence 2: {len(sequence_2s)} frames over 2.0 seconds (servo {SERVO_CHANNELS[1]})")
    
    sequence_3s = sequencer.generate_sine_sequence(
        servo_channel=SERVO_CHANNELS[2],
        duration=3.0,
        center=90,
        amplitude=45
    )
    print(f"  Sequence 3: {len(sequence_3s)} frames over 3.0 seconds (servo {SERVO_CHANNELS[2]})")
    
    # Execute sequences
    try:
        print("\n" + "=" * 60)
        print("Running all three sequences CONCURRENTLY...")
        print("=" * 60)
        print("(All three servos will move simultaneously)")
        print()
        
        # Start all three sequences in parallel threads
        threads = []
        stop_flags = []
        
        thread1, stop1 = sequencer.execute_sequence_threaded(sequence_1s, sequence_id="1s", use_realtime=True)
        threads.append(thread1)
        stop_flags.append(stop1)
        
        thread2, stop2 = sequencer.execute_sequence_threaded(sequence_2s, sequence_id="2s", use_realtime=True)
        threads.append(thread2)
        stop_flags.append(stop2)
        
        thread3, stop3 = sequencer.execute_sequence_threaded(sequence_3s, sequence_id="3s", use_realtime=True)
        threads.append(thread3)
        stop_flags.append(stop3)
        
        # Wait for all sequences to complete
        print("⏳ Waiting for all sequences to complete...")
        sequencer.wait_threads(threads)
        
        print("\n" + "=" * 60)
        print("✓ All sequences complete!")
        print("=" * 60)
        
    except KeyboardInterrupt:
        print("\n\n⏹ Interrupted by user")
        sequencer.stop_all(stop_flags)
    except Exception as e:
        print(f"\n❌ Error: {e}")
        sequencer.stop_all(stop_flags)
        raise


if __name__ == "__main__":
    main()
