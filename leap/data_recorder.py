################################################################################
# Copyright (C) 2012-2018 Leap Motion, Inc. All rights reserved.               #
# Leap Motion proprietary and confidential. Not for distribution.              #
# Use subject to the terms of the Leap Motion SDK Agreement available at       #
# https://developer.leapmotion.com/sdk_agreement, or another agreement         #
# between Leap Motion and you, your company or other organization.             #
################################################################################

# TODO convert into polling architecture from listener architecture (or I can pseudo-poll)

import sys, os, inspect
sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(inspect.getfile(inspect.currentframe())), 'lib/x64')))

import _thread, time
import Leap

import numpy as np
import pandas as pd
import ahrs
from ahrs.common import orientation
from ahrs.common import Quaternion
from scipy.spatial.transform import Rotation as R

import time

class Util:
    FPS_ROUGH = 1 # 60

    # bluetooth
    DURATION = 1000
    CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a1"
    CHARACTERISTIC_UUID2 = "beb5483e-36e1-4688-b7f5-ea07361b26a2"
    CHARACTERISTIC_UUID3 = "beb5483e-36e1-4688-b7f5-ea07361b26a3"
    CHARACTERISTIC_UUID4 = "beb5483e-36e1-4688-b7f5-ea07361b26a4"
    CHARACTERISTIC_UUID5 = "63bac001-bee5-4148-b8c7-220305bada25"
    CHARACTERISTIC_UUID6 = "b2465de1-6a70-4ca8-acf6-cd1cd0fa5d46"
    ADDRESS = "98:f4:ab:6c:d9:76"

    def mapper(handle): # need to configure!
        # print(handle)
        if handle == 41:
            return 1
        if handle == 44:
            return 2
        if handle == 47:
            return 3
        if handle == 50:
            return 4
        if handle == 53:
            return 5
        if handle == 56:
            return 6
        # jonna inefficient
    
    # leap
    FINGERS = ['Thumb', 'Index', 'Middle', 'Ring', 'Pinky'] # ordered correctly (nice)
    BONES = ['Metacarpal', 'Proximal', 'Intermediate', 'Distal'] # ordered correctly (nice)
    AXES = ['X', 'Y', 'Z']
    COLS = [f+'_'+b+'_'+axis for f in FINGERS for b in BONES for axis in AXES]

    # https://stackoverflow.com/a/69911892
    def get_rotation_matrix(vec2, vec1=np.array([1, 0, 0])):
        """get rotation matrix between two vectors using scipy"""
        vec1 = np.reshape(vec1, (1, -1))
        vec2 = np.reshape(vec2, (1, -1))
        r = R.align_vectors(vec2, vec1)
        return r[0].as_matrix()

    def rotvec_from_vecs(vec1, vec2):
        vec1 = np.reshape(vec1, (1, -1))
        vec2 = np.reshape(vec2, (1, -1))
        r = R.align_vectors(vec2, vec1)
        return r[0].as_rotvec()


    def test_rot_funcs(self): # 쓰진않음 ㅋ
        vec1 = np.array([2, 3, 2.5])
        vec2 = np.array([-3, 1, -3.4])

        mat = self.get_rotation_matrix(vec1=vec1, vec2=vec2)
        print(mat)
        print(self.rotvec_from_vecs(vec1, vec2))
        vec1_rot = mat.dot(vec1)
        print(vec1_rot)
        assert np.allclose(vec1_rot / np.linalg.norm(vec1_rot), vec2 / np.linalg.norm(vec2))
utils = Util()

dirc = np.zeros(shape=(10000, 60)) # direction vectors
xngl = np.zeros(shape=(10000, 60)) # relative axang
dirc_df = None
xngl_df = None

class Updater(Leap.Listener):

    def on_init(self, controller):
        self.is_empty = None
        self.row = np.zeros(shape=(60))
        self.end = False
        print("Initialized")

    def on_connect(self, controller):
        print("Connected")

    def on_disconnect(self, controller):
        # Note: not dispatched when running in a debugger.
        self.end = True
        print("Disconnected")

    def on_exit(self, controller):
        print("Exited")

    def on_frame(self, controller):
        # fetch frame, report metadata
        frame = controller.frame()
        # print("Frame id: %d, timestamp: %d, hands: %d, fingers: %d" % (frame.id, frame.timestamp, len(frame.hands), len(frame.fingers)))

        # self.frame = frame # debugging purposes

        # hands
        self.is_empty = frame.hands.is_empty
        if self.is_empty: self.row = np.zeros(60)
        
        elif frame.hands[0]:
            hand = frame.hands[0]

            # left / right
            handType = "Left" if hand.is_left else "Right"
            # print("  %s, id %d, position: %s" % (handType, hand.id, hand.palm_position))

            # normal & direction vectors
            normal = hand.palm_normal
            direction = hand.direction

            # pitch, roll, yaw
            # print("  pitch: %f degrees, roll: %f degrees, yaw: %f degrees" % (direction.pitch * Leap.RAD_TO_DEG, normal.roll * Leap.RAD_TO_DEG, direction.yaw * Leap.RAD_TO_DEG))

            # arm
            arm = hand.arm
            # print("  Arm direction: %s, wrist position: %s, elbow position: %s" % (arm.direction, arm.wrist_position, arm.elbow_position))

            # fingers
            for (f, finger) in enumerate(hand.fingers):

                # metadata
                # print("    %s finger, id: %d, length: %fmm, width: %fmm" % (self.finger_names[finger.type], finger.id, finger.length, finger.width))

                # bones
                for b in range(0, 4):
                    bone = finger.bone(b)
                    # print("      Bone: %s, start: %s, end: %s, direction: %s" % (self.bone_names[bone.type], bone.prev_joint, bone.next_joint, bone.direction))
                    self.row[12*f+3*b] = round(bone.direction[0], 3)
                    self.row[12*f+3*b+1] = round(bone.direction[1], 3)
                    self.row[12*f+3*b+2] = round(bone.direction[2], 3)



def process_directions(row): # 이런 씨발 이거 까먹고 있었다
    return row.copy()
    # metacarpal은 arm bone 기반으로. relative한게 중요할듯. 뭐... 무튼


def main(dirc, xngl):
    num = input('Enter data recording #')

    c = 0
    # Create a sample listener and controller
    listener = Updater()
    controller = Leap.Controller()

    # Have the sample listener receive events from the controller
    controller.add_listener(listener)

    # loop
    while not listener.end:
        # metadata
        c += 1

        # loop control
        # i = input('Enter to exit')
        # if i is not None: break
        

        # main
        if not listener.is_empty:
            print(listener.row)
            dirc[c] = listener.row
            xngl[c] = process_directions(listener.row)
        else: 
            print("No hands in frame!")
        
        # flow control (add/remove more)
        time.sleep(1/utils.FPS_ROUGH)

    # save
    dirc = dirc[:c]
    xngl = xngl[:c]
    dirc_df = pd.DataFrame(dirc, columns=utils.COLS)
    dirc_df.to_csv('./data/dirc_'+num+'.csv', index=False)
    xngl_df = pd.DataFrame(xngl, columns=utils.COLS)
    xngl_df.to_csv('./data/xngl_'+num+'.csv', index=False)

    # exit
    controller.remove_listener(listener)


if __name__ == "__main__":
    main(dirc, xngl)
