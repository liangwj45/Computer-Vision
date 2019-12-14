import os

white = (255, 255, 255)
black = (0, 0, 0)
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)
cyan = (0, 255, 255)
purple = (255, 0, 255)
yellow = (255, 255, 0)


def rename(root):
    for name in os.listdir(root):
        os.rename(f'{root}/{name}', f'{root}/prefix_{name}')

    cnt = 1
    for name in os.listdir(root):
        os.rename(f'{root}/{name}', f'{root}/{cnt}.jpg')
        cnt += 1
