fingers = ['Thumb', 'Index', 'Middle', 'Ring', 'Pinky']
bones = ['Metacarpal', 'Proximal', 'Intermediate', 'Distal']
cols = [a+'_'+b for a in fingers for b in bones]
print(cols)