from utils import *
import numpy as np
import imutils
import cv2


# 展示图片
def imshow(name, img):
    # cv2.namedWindow(name, cv2.WINDOW_NORMAL)
    cv2.imshow(name, img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


# 寻找名片轮廓
def find_contour(edged):
    # 寻找轮廓
    cnts = cv2.findContours(edged, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cnts = cnts[0] if imutils.is_cv2() else cnts[1]

    # 根据轮廓面积排序
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    for c in cnts:
        peri = cv2.arcLength(c, True)                  # 计算轮廓周长
        approx = cv2.approxPolyDP(c, 0.02*peri, True)  # 轮廓多边形拟合
        if len(approx) == 4:  # 如果是四边形就返回
            return approx
    return []


# 将顶点标记并保存图片
def save_peaks(img, peaks, path):
    tmp = img.copy()
    cv2.circle(tmp, tuple(peaks[0]), 10, black, 5)
    cv2.circle(tmp, tuple(peaks[1]), 10, red, 5)
    cv2.circle(tmp, tuple(peaks[2]), 10, blue, 5)
    cv2.circle(tmp, tuple(peaks[3]), 10, white, 5)
    cv2.imwrite(path, tmp)


# 顶点排序
def sort_peaks(peaks):
    peaks = peaks[np.lexsort([peaks[:, 1]])]
    up, down = peaks[:2], peaks[2:]
    up = peaks[np.lexsort([up[:, 0]])]
    down = down[np.lexsort([down[:, 0]])]
    peaks = np.concatenate((up, down))
    return peaks


# 名片纠正
def transform():
    for i in range(1, 23):
        # 导入图片并调整尺寸
        img = cv2.imread(f'../DataSet/{i}.jpg')
        img = imutils.resize(img, height=500)

        # 边缘检测
        gray = cv2.GaussianBlur(img, (5, 5), 0)
        edged = cv2.Canny(gray, 80, 230)
        cv2.imwrite(f'../DataSet/gray_{i}.jpg', gray)
        cv2.imwrite(f'../DataSet/edged_{i}.jpg', edged)

        # 寻找名片轮廓
        src = np.float32(find_contour(edged)).reshape(-1, 2)
        if len(src) > 0:
            print(f'picture{i}')

            # 确定平面在原图中的顶点位置
            src = sort_peaks(src)
            save_peaks(img, src, f'../DataSet/peaks_{i}.jpg')

            # 确定纠正后名片的尺寸
            width, height = src[1][0] - src[0][0], src[2][1] - src[0][1]
            dst = np.float32(
                [[0, 0], [width, 0], [0, height], [width, height]])

            # 名片纠正
            m = cv2.getPerspectiveTransform(src, dst)
            result = cv2.warpPerspective(img, m, (width, height))
            cv2.imwrite(f'../Dataset/result_{i}.jpg', result)


if __name__ == "__main__":
    # rename('../Dataset')
    transform()
