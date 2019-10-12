# Ex3：霍夫变换实现直线检测和圆形检测

> 数据科学与计算机学院 2019级 软件工程 16303050 梁文杰

## Part1 直线检测

Part1先对图像做边缘检测，求出边缘图像的霍夫空间，然后投票得出直线，求出直线并绘制直线和相关点即可。

边缘检测代码使用上次作业改写的canny代码，稍微改下接口即可。

边缘检测代码及霍夫变换代码见附件或[github仓库](https://github.com/liangwj45/Computer-Vision/tree/master/Ex3)。

程序运行结果：

### 原图

图1：

![1](.\img\part1\src\1.bmp)

图2：

![2](.\img\part1\src\2.bmp)

图3：

![3](.\img\part1\src\3.bmp)

图4：

![4](.\img\part1\src\4.bmp)

图5：

![5](.\img\part1\src\5.bmp)

图6：

![6](.\img\part1\src\6.bmp)


### 边缘检测

图1：

![01-edge_detect-1](.\img\part1\01-edge_detect-1.bmp)

图2：

![01-edge_detect-2](.\img\part1\01-edge_detect-2.bmp)

图3：

![01-edge_detect-3](.\img\part1\01-edge_detect-3.bmp)

图4：

![01-edge_detect-4](.\img\part1\01-edge_detect-4.bmp)

图5：

![01-edge_detect-5](.\img\part1\01-edge_detect-5.bmp)

图6：

![01-edge_detect-6](.\img\part1\01-edge_detect-6.bmp)

### 霍夫空间

图1：

![02-hough_space-1](.\img\part1\02-hough_space-1.bmp)

图2：

![02-hough_space-2](.\img\part1\02-hough_space-2.bmp)

图3：

![02-hough_space-3](.\img\part1\02-hough_space-3.bmp)

图4：

![02-hough_space-4](.\img\part1\02-hough_space-4.bmp)

图5：

![02-hough_space-5](.\img\part1\02-hough_space-5.bmp)

图6：

![02-hough_space-6](.\img\part1\02-hough_space-6.bmp)

### 直线方程

图1：

```
line 1: 360 = -0.99863x + 0.052336y
line 2: 101 = -0.999391x + 0.0348995y
line 3: 67 = 0.0871557x + 0.996195y
line 4: 454 = -0.0174524x + 0.999848y
```

图2：

```
line 1: 259 = 0.325568x + 0.945519y
line 2: 48 = 0.358368x + 0.93358y
line 3: 128 = -0.939693x + 0.34202y
line 4: 412 = -0.939693x + 0.34202y
```

图3：

```
line 1: 80 = -0.999848x -0.0174524y
line 2: 372 = -0.999391x + 0.0348995y
line 3: 477 = -0.052336x + 0.99863y
line 4: 63 = 0.0348995x + 0.999391y
```

图4：

```
line 1: 293 = 0.438371x + 0.898794y
line 2: 40 = 0.515038x + 0.857167y
line 3: 176 = -0.866025x + 0.5y
line 4: 473 = -0.891007x + 0.45399y
```

图5：

```
line 1: 316 = -1x + 0.001y
line 2: 36 = -0.99863x + 0.052336y
line 3: 442 = -6.12323e-17x + 1y
line 4: 30 = 0.0348995x + 0.999391y
```

图6：

```
line 1: 84 = -1x + 0.001y
line 2: 337 = -0.999391x + 0.0348995y
line 3: 470 = -0.0697565x + 0.997564y
line 4: 94 = 0.0697565x + 0.997564y
```

### 直线检测

图1：

![04-lines-1](.\img\part1\04-lines-1.bmp)

图2：

![04-lines-2](.\img\part1\04-lines-2.bmp)

图3：

![04-lines-3](.\img\part1\04-lines-3.bmp)

图4：

![04-lines-4](.\img\part1\04-lines-4.bmp)

图5：

![04-lines-5](.\img\part1\04-lines-5.bmp)

图6：

![04-lines-6](.\img\part1\04-lines-6.bmp)

## Part2 圆形检测

圆形检测使用霍夫梯度法，同样是先做边缘检测，然后对边缘图像中的每一个非零点，考虑其局部梯度，即用Sobel（）函数计算x和y方向的Sobel一阶导数得到梯度。利用得到的梯度，由斜率指定的直线上的每一个点都在累加器中被累加，这里的斜率是从一个指定的最小值到指定的最大值的距离。标记边缘图像中每一个非0像素的位置。然后从二维累加器中这些点中选择候选的中心，这些中心都大于给定阈值并且大于其所有近邻。这些候选的中心按照累加值降序排列，以便于最支持像素的中心首先出现。这些像素按照其与中心的距离排序。从到最大半径的最小距离算起，选择非0像素最支持的一条半径。如果一个中心收到边缘图像非0像素最充分的支持，并且到前期被选择的中心有足够的距离，那么它就会被保留下来。

程序运行结果：

### 原图

图1：

![1](.\img\part2\src\1.bmp)

图2：

![2](.\img\part2\src\2.bmp)

图3：

![3](.\img\part2\src\3.bmp)

图4：

![4](.\img\part2\src\4.bmp)

图5：

![5](.\img\part2\src\5.bmp)

图6：

![6](.\img\part2\src\6.bmp)

图7：

![7](.\img\part2\src\7.bmp)

图8：

![8](.\img\part2\src\8.bmp)

图9：

![9](.\img\part2\src\9.bmp)

### 边缘检测

图1：

![01-edge_detect-1](.\img\part2\实心圆\01-edge_detect-1.bmp)

图2：

![01-edge_detect-2](.\img\part2\实心圆\01-edge_detect-2.bmp)

图3：

![01-edge_detect-3](.\img\part2\实心圆\01-edge_detect-3.bmp)

图4：

![01-edge_detect-4](.\img\part2\实心圆\01-edge_detect-4.bmp)

图5：

![01-edge_detect-5](.\img\part2\实心圆\01-edge_detect-5.bmp)

图6：

![01-edge_detect-6](.\img\part2\实心圆\01-edge_detect-6.bmp)

图7：

![01-edge_detect-7](.\img\part2\实心圆\01-edge_detect-7.bmp)

图8：

![01-edge_detect-8](.\img\part2\实心圆\01-edge_detect-8.bmp)

图9：

![01-edge_detect-9](.\img\part2\实心圆\01-edge_detect-9.bmp)

### 圆形检测（实心）

图1：

![02-draw_circles-1](.\img\part2\实心圆\02-draw_circles-1.bmp)

图2：

![02-draw_circles-2](.\img\part2\实心圆\02-draw_circles-2.bmp)

图3：

![02-draw_circles-3](.\img\part2\实心圆\02-draw_circles-3.bmp)

图4：

![02-draw_circles-4](.\img\part2\实心圆\02-draw_circles-4.bmp)

图5：

![02-draw_circles-5](.\img\part2\实心圆\02-draw_circles-5.bmp)

图6：

![02-draw_circles-6](.\img\part2\实心圆\02-draw_circles-6.bmp)

图7：

![02-draw_circles-7](.\img\part2\实心圆\02-draw_circles-7.bmp)

图8：

![02-draw_circles-8](.\img\part2\实心圆\02-draw_circles-8.bmp)

图9：

![02-draw_circles-9](.\img\part2\实心圆\02-draw_circles-9.bmp)

### 圆形检测（空心）

图1：

![02-draw_circles-1](.\img\part2\02-draw_circles-1.bmp)

图2：

![02-draw_circles-2](.\img\part2\02-draw_circles-2.bmp)

图3：

![02-draw_circles-3](.\img\part2\02-draw_circles-3.bmp)

图4：

![02-draw_circles-4](.\img\part2\02-draw_circles-4.bmp)

图5：

![02-draw_circles-5](.\img\part2\02-draw_circles-5.bmp)

图6：

![02-draw_circles-6](.\img\part2\02-draw_circles-6.bmp)

图7：

![02-draw_circles-7](.\img\part2\02-draw_circles-7.bmp)

图8：

![02-draw_circles-8](.\img\part2\02-draw_circles-8.bmp)

图9：

![02-draw_circles-9](.\img\part2\02-draw_circles-9.bmp)

## Part3 名片检测

part3最后还是没能按时完成，但已经有想法了，日后有时间或许会补上。

### 任务分析

由于图片的背景有非常多的噪声，卡片内部也有较多的线条，甚至卡片中的文字都可能被识别成直线，所以直接做直线检测是不可行的。

### 目前的想法

可以先做边缘检测，然后可以先找出每张测试图中两张卡片共8条边的斜率（共4组平行线），因此最多4个不同的斜率，另外有4组相互垂直的直线。因此根据平行与垂直的关系可以得出，在（0，90）的角度空间中，只需找出出现频率最高的两组斜率，即能确定出卡片边的斜率。随后获取新的霍夫空间，仅保留以上两组斜率，可以减少噪声，最终得出卡片的边与各顶点。