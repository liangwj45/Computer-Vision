from sklearn.cluster import KMeans
from PIL import Image
import numpy as np
import os


def load_img(path, n):
    features = []
    images = []
    for i in range(n):
        img = Image.open('{}/{}.bmp'.format(path, i))
        images.append(img)
        img = np.array(img)
        img = img[:, :, 0]
        features.append(img.reshape([-1]) / (255))
    return images, features


def task(step, n):
    images, features = load_img(
        '../Division/src/output/{}/num'.format(step), n)
    kmeans = KMeans(n_clusters=20, random_state=0).fit(features)
    labels = kmeans.labels_

    cluster = {}
    for i in range(len(labels)):
        if labels[i] not in cluster:
            cluster[labels[i]] = [images[i]]
        else:
            cluster[labels[i]].append(images[i])
    for key, image_list in cluster.items():
        cnt = 0
        for image in image_list:
            path = './{}/i{}'.format(step, key)
            if not os.path.exists(path):
                os.makedirs(path)
            image.save('{}/{}_{}.bmp'.format(path, key, cnt))
            cnt += 1


if __name__ == "__main__":
    task('train', 1088)
    task('test', 2200)
