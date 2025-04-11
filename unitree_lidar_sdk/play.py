import numpy as np
import matplotlib.pyplot as plt
import pyvista as pv

with open('data/bruce_fall1.txt', 'r') as f:
    lines = f.readlines()

lines = [line.strip() for line in lines]
frames = []
for line in lines:
    if len(line.split(',')) == 4:
        if len(frames) > 0:
            frames[-1] = np.array(frames[-1])
        frames.append([])
    else:
        point_str = line.split(',')
        point = [float(p) for p in point_str]
        point = np.array(point)
        frames[-1].append(point)
frames[-1] = np.array(frames[-1])
frames = frames[1:-1]

all_pc = np.concatenate(frames, axis=0)
print(all_pc.shape)

pv.plot(all_pc[:, 0:3], show_axes=True, show_grid=True)