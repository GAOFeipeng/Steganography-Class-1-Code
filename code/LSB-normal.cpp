#include  <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


Mat Write_RGB(string img, string logo) {
	// 载体		logo
	Mat org = imread(img);// 彩色读入
	Mat logo_img = imread(logo, 0);// 灰度读入 之后会转换成二值图 > 125
	int row = org.rows, col = org.cols;
	Mat steg = org.clone();
	resize(logo_img, logo_img, Size(col, row));
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			// 遍历所有像素点
			Vec3b x = org.at<Vec3b>(i, j);
			// x[0] - 第一个元素 x[1]  第二个
			int b = x[0] & 1;// 拿到最后一位
			int t = logo_img.at<uchar>(i, j) > 125;// > 125 白色
			// 不在原图上改
			if (b != t) {
				// 跳变最后一位
				steg.at<Vec3b>(i, j)[0] ^= 1;
			}
		}
	}

	return steg;
}
Mat Read_RGB(string steg) {
	// 读入一张图片 返回他蓝色通道最后一个位面的图
	Mat org = imread(steg); //BGR
	int row = org.rows, col = org.cols;
	Mat solve(row, col, CV_8UC1);
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Vec3b x = org.at<Vec3b>(i, j);
			// x[0] & 1 -> 1 0 (255 0)
			solve.at<uchar>(i, j) = (x[0] & 1) * 255;
		}
	}
	return solve;
}

int main() {

	//Mat steg = Write_RGB("std/LenaRGB.bmp","logo1.png");
	//imwrite("steg_rgb.png", steg);
	Mat solve = Read_RGB("steg_rgb.png");
	imshow("solve", solve);
	waitKey(0);
	return 0;
}
