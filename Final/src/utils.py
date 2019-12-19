import os
import cv2

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
cyan = (0, 255, 255)
purple = (255, 0, 255)
yellow = (255, 255, 0)


root = '../cards/'


def is_Chinese(word):
    for ch in word:
        if ch < '\u4e00' or ch > '\u9fff':
            return False
    return True


def imshow(img, name='img'):
    # cv2.namedWindow(name, cv2.WINDOW_NORMAL)
    cv2.imshow(name, img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def imsave(img, tag):
    cv2.imwrite(f'{root}{tag}.jpg', img)


def rename(root):
    for name in os.listdir(root):
        os.rename(f'{root}/{name}', f'{root}/prefix_{name}')

    cnt = 1
    for name in os.listdir(root):
        os.rename(f'{root}/{name}', f'{root}/{cnt}.jpg')
        cnt += 1
