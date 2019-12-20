from utils import *
import numpy as np
import imutils
import re
import cv2
import math
import pytesseract
from PIL import Image


# 功能函数：截取子图像
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


# 功能函数：保存通过连通块分割后的图片
def save_contours(img, i):
    cnts = find_contours(img, 21, 3)
    index = 0
    for cnt in cnts:
        sub = sub_img(img, cnt)
        imsave(sub, f'contour/{i}_{index}')
        index += 1


# 功能函数：保存单个字符切割后的图像
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


# 功能函数：保存用轮廓标记后的图像
def draw_contours(img, cnts, tag):
    seg = img.copy()
    for cnt in cnts:
        x, y, w, h = cv2.boundingRect(cnt)  # 最小邻接矩
        cv2.rectangle(seg, (x, y), (x + w, y + h), blue, 1)
    imsave(seg, tag)


# 功能函数：通过先膨胀操作后寻找图像的轮廓集合
# 通过传入 w 和 h 参数决定横向膨胀（w）和纵向（h）膨胀的程度
def find_contours(img, w, h):
    kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (w, h))
    dialtion = cv2.dilate(img, kernel)
    cnts = cv2.findContours(dialtion, cv2.RETR_EXTERNAL,
                            cv2.CHAIN_APPROX_SIMPLE)
    return cnts[0] if imutils.is_cv2() else cnts[1]


# 识别电话号码主要逻辑
def identify_phone(img):
    tels = []
    # 获取横向膨胀后的图像轮廓
    cnts = find_contours(img, 9, 3)
    # 遍历所有连通块，逐一识别电话号码
    for cnt in cnts:
        sub = sub_img(img, cnt)
        # 筛选出横纵比大于 1 小于 6 的连通块
        if sub.shape[1] / sub.shape[0] < 6 or sub.shape[0] / sub.shape[1] > 1:
            continue
        # 识别数字
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6 digits')
        text = re.sub('\r|\n|\t| ', '', text)
        if len(text) > 0 and text[0] == '-':
            text = text[1:]
        # 只判断是否是 11 位的数字
        if len(text) == 11:
            tels.append(text)
    return tels


# 识别姓名主要逻辑
def identify_name(img):
    name = None
    # 获取横向膨胀后的图像轮廓
    cnts = find_contours(img, 21, 3)
    # 由于姓名的字号通常较大，所以将连通块以面积从大到小排序
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    for cnt in cnts:
        sub = sub_img(img, cnt)
        # 筛选出横纵比大于 1 小于 4 的连通块
        if sub.shape[1] / sub.shape[0] > 4 or sub.shape[0] / sub.shape[1] > 1:
            continue
        # 识别汉字
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6')
        text = re.sub('\r|\n|\t|-| ', '', text)
        # 只判断两个字或三个字的姓名
        if not name and (len(text) == 2 or len(text) == 3) and is_Chinese(text):
            name = text
    return name


# 识别公司名称主要逻辑
def identify_company(img):
    # 获取横向膨胀后的图像轮廓
    cnts = find_contours(img, 17, 3)
    # 由于公司名称的字号通常较大，所以将连通块以面积从大到小排序
    cnts = sorted(cnts, key=cv2.contourArea, reverse=True)
    for cnt in cnts:
        sub = sub_img(img, cnt)
        # 筛选出横纵比大于 1 小于 6 的连通块
        if sub.shape[1] / sub.shape[0] < 6 or sub.shape[0] / sub.shape[1] > 1:
            continue
        # 识别汉字
        text = pytesseract.image_to_string(
            sub, 'chi_sim', config='-psm 6')
        text = re.sub('\r|\n|\t|-| ', '', text)
        # 只判断有“公司”字段的公司名称
        if len(text) > 0 and text.find('公司') != -1:
            return text
    return None


# 识别程序主要逻辑
def identify(imgs):
    with open('./records.csv', 'w', encoding='utf-8') as rec:
        rec.write('图片编号,姓名,公司,电话\n')
        for i in range(len(imgs)):
            print(">>>>>>>>>", i)
            img = imgs[i]

            # 图像二值化
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
            bin = cv2.adaptiveThreshold(
                gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 9, 5)

            # 获取横向膨胀后的图像轮廓
            cnts = find_contours(bin, 5, 3)
            con = bin.copy()
            for cnt in cnts:
                area = cv2.contourArea(cnt)
                # 删除面积小于80的连通块
                if area < 80:
                    cv2.drawContours(con, [cnt], 0, 0, -1)

            name = identify_name(con)
            comp = identify_company(con)
            tels = identify_phone(con)

            # 记录识别结果
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
