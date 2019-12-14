from utils import *
import numpy as np
import imutils
import cv2


# 名片切割
def segmentation():
    for i in range(1, 12):
        img = cv2.imread(f'../cards/{i}.jpg')

        # 边缘检测
        edged = cv2.Canny(img, 80, 230)
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (15, 1))
        dilation = cv2.dilate(edged, kernel)
        erosion = cv2.erode(dilation, kernel)
        cv2.imwrite(f'../cards/edged_{i}.jpg', edged)
        cv2.imwrite(f'../cards/dilation_{i}.jpg', dilation)
        cv2.imwrite(f'../cards/erosion_{i}.jpg', erosion)

        # 寻找轮廓
        cnts = cv2.findContours(
            erosion, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        cnts = cnts[0] if imutils.is_cv2() else cnts[1]

        # 用外接矩形标记
        result = img.copy()
        for cnt in cnts:
            x, y, w, h = cv2.boundingRect(cnt)  # 外接矩形
            cv2.rectangle(result, (x, y), (x + w, y + h), green, 1)
        cv2.imwrite(f'../cards/contour_{i}.jpg', result)


if __name__ == "__main__":
    # rename('../cards')
    segmentation()
