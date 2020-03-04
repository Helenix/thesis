import matplotlib.pyplot as plt
import numpy as np


with open("../input_files/groundNodes100.txt") as file:
    for line in file:
        positions = line.split()
        x, y = int(positions[0]), int(positions[1])
        # plt.plot(x, y, 'bo', markersize = 3)


hull = []
with open("../input_files/convexHull.txt") as file:
    for line in file:
        positions = line.split()
        point = int(positions[0]), int(positions[1])
        hull.append(point)

size = len(hull)
i = 0
while i < size:
    x1 = hull[i][0]
    y1 = hull[i][1]

    if i+1 != size:
        x2 = hull[i+1][0]
        y2 = hull[i+1][1]
        plt.plot([x1, x2], [y1, y2], 'ro-', linewidth = 0.75, markersize = 3)
    else:
        plt.plot([x1, 0], [y1, 0], 'ro-', linewidth = 0.75, markersize = 3)

    i = i + 1

xMean = 2476
yMean = 2491
# radius = 892
plt.plot(xMean, yMean, 'go', markersize = 3)
# circle1 = plt.Circle((xMean, yMean), radius = 892, fill = False)
# plt.gcf().gca().add_artist(circle1)
# ax = plt.gca()
# ax.add_artist(circle1)

with open("../output_files/pointsLeft.txt") as file:
    for line in file:
        positions = line.split()
        x, y = int(positions[0]), int(positions[1])
        plt.plot(x, y, 'yo', markersize = 3)

with open("../output_files/pointsRight.txt") as file:
    for line in file:
        positions = line.split()
        x, y = int(positions[0]), int(positions[1])
        plt.plot(x, y, 'mo', markersize = 3)


    
plt.grid(True)
plt.axis('square')
plt.show()
