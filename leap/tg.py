import numpy as np

print(np.zeros(shape=[2, 3]))

a = np.zeros(shape=[100, 60])
print(a)
a = np.reshape(a, [100, 20, 3])
print(a)
