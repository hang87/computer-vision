### EX1：图像读取和显示以及像素操作

> 数据科学与计算机学院 软件工程 欧子菁 2017级 16307050

#### 1 读入 1.bmp 文件，并用 CImg.display() 显示。

先读入1.bmp文件，构造Cimg对象

```c
CImg<unsigned char> image("1.bmp");
```

然后调用display方法显示图像

```c
image.display("task1");
```

![task1](https://github.com/J-zin/computer-vision/blob/master/01/image/task1.png?raw=true)

#### 2. 把图像中白色区域变成红色，黑色区域变成绿色

使用CImg库里的cimg_forXY遍历全部像素点，然后根据RGB值进行相应操作

```c
static void task2(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (image(x, y, 0) == 255 && image(x, y, 1) == 255 && image(x, y, 2) == 255) {
				image(x, y, 1) = 0;
				image(x, y, 2) = 0;
			}
			if (image(x, y, 0) == 0 && image(x, y, 1) == 0 && image(x, y, 2) == 0) {
				image(x, y, 1) = 255;
			}
		}
		
		return;
	}
```

![task2](https://github.com/J-zin/computer-vision/blob/master/01/image/task2.png?raw=true)

#### 3. 在图上绘制一个等边三角形区域，其中心坐标(50,50)，边长为 40，填充颜色为蓝色。

首先确定3个定点的坐标，然后即可确定三条边的表达式，然后根据不等式进行颜色填充。

```c
static void task3(CImg<unsigned char>& image) {		
		double Ax = 50;
		double Ay = 50 - 20 / cos(M_PI/6);
		double Bx = 50 - 20;
		double By = 50 + 20 * tan(M_PI/6);
		double Cx = 50 + 20;
		double Cy = 50 + 20 * tan(M_PI/6);
		
		double kab = (Ay - By) * 1.0 / (Ax - Bx);
		double lab = Ay * 1.0 - kab * Ax;
		double kbc = (By - Cy) * 1.0 / (Bx - Cx);
		double lbc = By * 1.0 - kbc * Bx;
		double kac = (Ay - Cy) * 1.0 / (Ax - Cx);
		double lac = Ay * 1.0 - kac * Ax;
		
		cimg_forXY(image, x, y) {
			// y = kx + l   ==> y - kx - l
			double dab = y - kab * x - lab;
			double dbc = y - kbc * x - lbc;
			double dac = y - kac * x - lac;
			
			if (dab >= 0 && dbc <= 0 && dac >= 0) {
				image(x, y, 2) = 255;
			}
		}
		
		return;
	}
```

![task3](https://github.com/J-zin/computer-vision/blob/master/01/image/task3.png?raw=true)

下面是调用CImg库来画三角形

```c
static void task3_2(CImg<unsigned char>& image) {
double Ax = 50;
    double Ay = 50 - 20 / cos(M_PI/6);
    double Bx = 50 - 20;
    double By = 50 + 20 * tan(M_PI/6);
    double Cx = 50 + 20;
    double Cy = 50 + 20 * tan(M_PI/6);

    unsigned char color[]    = { 0, 0, 255 };

    image.draw_triangle(
        Ax, Ay,       // 1st vertex x,y
        Bx, By,      // 2nd vertex x,y
        Cx, Cy,     // 3rd vertex x,y 
        color
    );
}
```

![task3_2](https://github.com/J-zin/computer-vision/blob/master/01/image/task3_2.png?raw=true)

#### 4. 在图上绘制一个圆形区域，圆心坐标(50,50)，半径为 15，填充颜色为黄色。

根据圆的不等式，确定坐标，然后进行颜色填充

```c
	static void task4(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (pow(x-50, 2) + pow(y-50, 2) <= pow(15, 2)) {
				image(x, y, 0) = 255;
				image(x, y, 1) = 255;
			}
		}
			
		return;
	}
```

![task4](https://github.com/J-zin/computer-vision/blob/master/01/image/task4.png?raw=true)

可以看到，画出的圆中间有个类似十字架的东西，为什么会出现这个情况？具体在后面的思考中进行分析

下面是调用CImg库来画圆形

```c
static void task4_2(CImg<unsigned char>& image) {
		unsigned char yellow[] = {255, 255, 0};
		image.draw_circle(50, 50, 15, yellow);
			
		return;
}
```

![task4_2](https://github.com/J-zin/computer-vision/blob/master/01/image/task4_2.png?raw=true)

调用库函数，效果明显比自己画的好一点

#### 5. 在图上绘制一条长为 100 的直线段，起点坐标为(0, 0)，方向角为 135 度，直线的颜色为绿色。

从左上角引一条 45 度角长度为 100 的直线段出来，即令 x 等于 y 即可：

```c
#define dis(x, y, x0, y0) (sqrt(pow((x) - (x0), 2) + pow((y) - (y0), 2)))

static void task5(CImg<unsigned char>& image) {
		cimg_forXY(image, x, y) {
			if (x == y && dis(x, y, 0, 0) <= 100) {
				image(x, y, 0) = 0;
				image(x, y, 1) = 255;
				image(x, y, 2) = 0;
			}
		}
		
		return;
}
```

![task5](https://github.com/J-zin/computer-vision/blob/master/01/image/task5.png?raw=true)

调用库函数画直线：

```c
static void task5_2(CImg<unsigned char>& image) {
		unsigned char blue[] = {0, 255, 0};
		image.draw_line(0, 0, 100 * cos(45*M_PI/180), 100 * sin(45*M_PI/180), blue);
		
		return;
}
```

![task5_2](https://github.com/J-zin/computer-vision/blob/master/01/image/task5_2.png?raw=true)

#### 6. 把上面的操作结果保存为2.bmp。 

让前几步操作均在同一个对象中操作，然后调用 img.save() 方法即可：

```c
static void task6(CImg<unsigned char>& image) {
		image.save("2.bmp");
}

void test6() {
		HW1::task2(image);
		HW1::task3(image);
		HW1::task4(image);
		HW1::task5(image);
		HW1::task6(image);
		image.display("task6");
}
```

![task6](https://github.com/J-zin/computer-vision/blob/master/01/image/task6.png?raw=true)

#### 思考

> 为什么第四步绘制的圆形区域形状效果不好。 

因为我们是通过遍历整个图像的像素点来对图片进行采样的，因为坐标点都是整数值，圆的半径越小意味着我们取的像素点范围越少，自然效果会差。直径很小时，可能看起来就像是个矩形了。

