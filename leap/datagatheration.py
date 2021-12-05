################################################################################
# Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.               #
# Leap Motion proprietary and confidential. Not for distribution.              #
# Use subject to the terms of the Leap Motion SDK Agreement available at       #
# https://developer.leapmotion.com/sdk_agreement, or another agreement         #
# between Leap Motion and you, your company or other organization.             #
################################################################################

# TODO convert into polling architecture from listener architecture (or I can pseudo-poll)

import sys, os, inspect

from pandas.io.sql import DatabaseError
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(inspect.getfile(inspect.currentframe())), 'lib/x64')))

from config import * # ADDRESS, CHARACTERISTIC_UUID(#)

import _thread, time
import Leap

import numpy as np
import pandas as pd
import ahrs
from ahrs.common import orientation
from ahrs import Quaternion


fingers = ['Thumb', 'Index', 'Middle', 'Ring', 'Pinky']
bones = ['Metacarpal', 'Proximal', 'Intermediate', 'Distal']
cols = [f+'_'+b for f in fingers for b in bones]

class Listener(Leap.Listener):

    def __init__(self):
        self.df = None
        self.c = 0
        self.data = np.zeros(shape=(10000, 20))
        self.row = np.zeros(shape=(20))

    def on_init(self, controller):
        print("Initialized")

    def on_connect(self, controller):
        print("Connected")

    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        print("Disconnected")

    def on_exit(self, controller):
        self.df = pd.DataFrame(self.data, columns=cols)
        print(self.df.head())
        print(self.df.tail())
        input('Enter again(?) to exit')

        print("Exited")

    def on_frame(self, controller):
        # fetch frame; report info
        frame = controller.frame()
        # print("Frame id: %d, timestamp: %d, hands: %d, fingers: %d" % (frame.id, frame.timestamp, len(frame.hands), len(frame.fingers)))

        # Get hands
        for hand in frame.hands:

            handType = "Left" if hand.is_left else "Right"
            # print("  %s, id %d, position: %s" % (handType, hand.id, hand.palm_position))

            # hand normal & direction vectors
            normal = hand.palm_normal
            direction = hand.direction

            # hand pitch, roll, and yaw angles
            print("  pitch: %f degrees, roll: %f degrees, yaw: %f degrees" % (
                direction.pitch * Leap.RAD_TO_DEG,
                normal.roll * Leap.RAD_TO_DEG,
                direction.yaw * Leap.RAD_TO_DEG))

            # Get arm bone
            arm = hand.arm
            print("  Arm direction: %s, wrist position: %s, elbow position: %s" % (
                arm.direction,
                arm.wrist_position,
                arm.elbow_position))

            # Get fingers
            for finger in hand.fingers:

                print("    %s finger, id: %d, length: %fmm, width: %fmm" % (
                    self.finger_names[finger.type],
                    finger.id,
                    finger.length,
                    finger.width))

                # Get bones
                for b in range(0, 4):
                    bone = finger.bone(b)
                    print("      Bone: %s, start: %s, end: %s, direction: %s" % (
                        self.bone_names[bone.type],
                        bone.prev_joint,
                        bone.next_joint,
                        bone.direction))
                    
                    if finger.bone(b-1):
                        prev_bone = finger.bone(b-1)
                        rot = orientation.from_vects(bone, prev_bone) # PSEUDO

        self.data[self.c] = self.row

        self.c += 1
        self.row = np.zeros(shape=[20])


        if not frame.hands.is_empty:
            print("")

def main():
    # Create a sample listener and controller
    listener = Listener()
    controller = Leap.Controller()

    # Have the sample listener receive events from the controller
    controller.add_listener(listener)

    # Keep this process running until Enter is pressed
    print("Press Enter to quit...")
    try:
        sys.stdin.readline()
    except KeyboardInterrupt:
        pass
        # implement on_exit code here?
    finally:
        # Remove the sample listener when done
        controller.remove_listener(listener)


if __name__ == "__main__":
    main()
