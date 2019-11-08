import os
import numpy as np
import cv2

for i in range(20):
    for root, dirs, files, in os.walk("{}".format(i)):
        j = 0
        for f in files:
            os.rename(os.path.join(root, f), os.path.join(root, "{}.png".format(j)))
            j+=1

# for root, dirs, files in os.walk("."):
#     for d in dirs:
#         for _, _, dfs in os.walk(d):
#             i = 0
#             for f in dfs:
#                 path = os.path.join(d, f)
#                 img = cv2.imread(path, 1)
#                 img90 = np.rot90(img)
#                 cv2.imwrite("1"+path, img90)