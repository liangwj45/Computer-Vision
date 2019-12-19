from utils import *
import numpy as np
import imutils
import re
import cv2
import math
import pytesseract
from PIL import Image


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


def save_contours(img, i):
    cnts = find_contours(img, 21, 3)
    index = 0
    for cnt in cnts:
        sub = sub_img(img, cnt)
        imsave(sub, f'contour/{i}_{index}')
        index += 1


def save_characters(img, i):
    cnts = find_contours(img, 5, 3)
    index = 0
    for cnt in cnts:
        # 根据轮廓切割区域
        sub = sub_img(img, cnt)

        # 统计垂直投影像素个数
        pixs = np.zeros(sub.shape[1], np.uint8)
        for j in range(sub.shape[1]):
            for k in range(sub.shape[0]):
                pixs[j] += 1 if sub[k][j] == 255 else 0

        j = 0
        while j < sub.shape[1]:
            # 根据垂直投影切割字符
            while j < sub.shape[1] and pixs[j] == 0:
                j += 1
            if j >= sub.shape[1]:
                break
            left = j
            while j < sub.shape[1] and pixs[j] != 0:
                j += 1
            right = j

            if right - left > 1:
                if left > 0:
                    left = left - 1
                if right < sub.shape[1]:
                    right = right + 1
                sigle = sub[:, left-1:right+1]
                # 保存字符
                if sigle.shape[0] > 5 and sigle.shape[1] > 5 and sigle.shape[0] > 10 or sigle.shape[1] > 10:
                    imsave(sigle, f'num/{i}_{index}')
                    index += 1


def find_contours(img, w, h):
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (w, h))
    dialtion = cv2.dilate(img, kernel)
    cnts = cv2.findContours(dialtion, cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)
    return cnts[0] if imutils.is_cv2() else cnts[1]


def draw_contours(img, cnts, tag):
    seg = img.copy()
    for cnt in cnts:
        x, y, w, h = cv2.boundingRect(cnt)
        cv2.rectangle(seg, (x, y), (x + w, y + h), blue, 1)
    imsave(seg, tag)


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
    # rename('../cards')
    imgs = []
    for i in range(20):
        img = cv2.imread(f'../cards/{i}.jpg')
        imgs.append(img)
    identify(imgs)
