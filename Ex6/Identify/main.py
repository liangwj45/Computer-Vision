from PIL import Image
import numpy as np
from sklearn.svm import SVC
from sklearn.ensemble import AdaBoostClassifier
from sklearn.neighbors import KNeighborsClassifier
import xgboost as xgb
import time
import os


def load_data(path):
    data, label = [], []
    for i in range(20):
        for root, dirs, files in os.walk(path + str(i)):
            cnt = 0
            for file in files:
                label.append(i)
                img = Image.open('{}{}/{}.bmp'.format(path, i, str(cnt)))
                img = np.array(img)[:, :, 0]
                data.append(img.reshape([-1]) / (255))
                cnt += 1
    return data, label


def identify(algorithm, model, train, train_label, test, test_label):
    start = time.time()
    model.fit(train, train_label)
    pre_label = model.predict(test)
    end = time.time()
    rate = sum(pre_label == test_label) / len(test_label)*100
    print('-------------')
    print(algorithm)
    print('time: {}s'.format(round(end-start, 3)))
    print('rate: {}%'.format(round(rate, 2)))


if __name__ == "__main__":
    train, train_label = load_data("../Cluster/train/")
    test, test_label = load_data("../Cluster/test/")
    print('training:', len(train_label))
    print('testing:', len(test_label))

    # SVM
    algorithm = 'SVM'
    model = SVC(gamma='auto')
    identify(algorithm, model, train, train_label, test, test_label)

    # AdaBoost
    algorithm = 'AdaBoost'
    model = AdaBoostClassifier(n_estimators=80, learning_rate=0.1)
    identify(algorithm, model, train, train_label, test, test_label)

    # KNN
    algorithm = 'KNN'
    model = KNeighborsClassifier(n_neighbors=10)
    identify(algorithm, model, train, train_label, test, test_label)

    # XGB
    algorithm = 'XGB'
    model = xgb.XGBClassifier(objective="multi:softprob")
    identify(algorithm, model, np.array(train), np.array(
        train_label), np.array(test), test_label)
