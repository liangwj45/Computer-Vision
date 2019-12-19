from src.utils import *
from PIL import Image
import pytesseract
import numpy as np
import imutils
import math
import cv2
import re


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
    imgs = []
    for i in range(1, 23):
        # 导入图片并调整尺寸
        img = cv2.imread(f'./DataSet/{i}.jpg')
        img = imutils.resize(img, height=500)

        # 边缘检测
        gray = cv2.GaussianBlur(img, (5, 5), 0)
        edged = cv2.Canny(gray, 80, 230)

        # 膨胀/腐蚀
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (19, 19))
        dilation = cv2.dilate(edged, kernel)
        erosion = cv2.erode(dilation, kernel)

        # 寻找名片轮廓
        src = np.float32(find_contour(erosion)).reshape(-1, 2)
        if len(src) > 0:
            # 确定平面在原图中的顶点位置
            src = sort_peaks(src)

            # 确定纠正后名片的尺寸
            width, height = src[1][0] - src[0][0], src[2][1] - src[0][1]
            dst = np.float32(
                [[0, 0], [width, 0], [0, height], [width, height]])

            # 名片纠正
            m = cv2.getPerspectiveTransform(src, dst)
            result = cv2.warpPerspective(img, m, (width, height))
            result = cv2.resize(result, (530, 340), 0, 0, cv2.INTER_LINEAR)
            imgs.append(result)

    return imgs


def sub_img(img, cnt):
    x, y, w, h = cv2.boundingRect(cnt)
    if x > 0:
        x -= 1
    if w + 2 < img.shape[1]:
        w += 2
    if y > 0:
        y -= 1
    if h + 2 < img.shape[0]:
        h += 2
    return img[y:y+h, x:x+w]


def find_contours(img, w, h):
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (w, h))
    dialtion = cv2.dilate(img, kernel)
    cnts = cv2.findContours(dialtion, cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)
    return cnts[0] if imutils.is_cv2() else cnts[1]


def identify_phone(img):
    tels = []
    cnts = find_contours(img, 9, 3)
    for cnt in cnts:
        sub = sub_img(img, cnt)
        if sub.shape[1] / sub.shape[0] < 6 or sub.shape[0] / sub.shape[1] > 1:
            continue
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6 digits')
        text = re.sub('\r|\n|\t| ', '', text)
        if len(text) > 0 and text[0] == '-':
            text = text[1:]
        if len(text) == 11:
            tels.append(text)
    return tels


def identify_name(img):
    name = None
    cnts = find_contours(img, 21, 3)
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    for cnt in cnts:
        sub = sub_img(img, cnt)
        if sub.shape[1] / sub.shape[0] > 4 or sub.shape[0] / sub.shape[1] > 1:
            continue
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6')
        text = re.sub('\r|\n|\t|-| ', '', text)
        if not name and (len(text) == 2 or len(text) == 3) and is_Chinese(text):
            name = text
    return name


def identify_company(img):
    cnts = find_contours(img, 17, 3)
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    for cnt in cnts:
        sub = sub_img(img, cnt)
        if sub.shape[1] / sub.shape[0] < 6 or sub.shape[0] / sub.shape[1] > 1:
            continue
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6')
        text = re.sub('\r|\n|\t|-| ', '', text)
        if len(text) > 0 and text.find('公司') != -1:
            return text
    return None


def identify(imgs):
    with open('./records.csv', 'w', encoding='utf-8') as rec:
        rec.write('图片编号,姓名,公司,电话\n')
        for i in range(len(imgs)):
            print(">>>>>>>>>", i)
            img = imgs[i]

            # 二值化
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            bin = cv2.adaptiveThreshold(
                gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 9, 5)

            # 删除小连通块
            kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 3))
            dialtion = cv2.dilate(bin, kernel)
            cnts = cv2.findContours(dialtion, cv2.RETR_TREE,
                                    cv2.CHAIN_APPROX_SIMPLE)
            cnts = cnts[0] if imutils.is_cv2() else cnts[1]
            con = bin.copy()
            for cnt in cnts:
                area = cv2.contourArea(cnt)
                if area < 80:
                    cv2.drawContours(con, [cnt], 0, 0, -1)

            name = identify_name(con)
            comp = identify_company(con)
            tels = identify_phone(con)

            blank = ''
            rec.write(f'{i},')
            rec.write(f'{name if name else blank},')
            rec.write(f'{comp if comp else blank},')
            for tel in tels:
                rec.write(f'{tel if tel else blank},')
            rec.write('\n')


if __name__ == "__main__":
    rename('./test')
    imgs = transform()
    identify(imgs)
