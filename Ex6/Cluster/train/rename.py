import os

if __name__ == "__main__":
    for i in range(20):
        for root, dirs, files in os.walk(str(i)):
            cnt = 0
            for file in files:
                os.rename(os.path.join(root, file),
                          os.path.join(root, str(cnt) + 'a.bmp'))
                cnt += 1
        for root, dirs, files in os.walk(str(i)):
            cnt = 0
            for file in files:
                os.rename(os.path.join(root, file),
                          os.path.join(root, str(cnt) + '.bmp'))
                cnt += 1
            print(cnt)
