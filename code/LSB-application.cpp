#include  <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

/*
将你的图片隐写到彩色图片中，并设计一个函数，使别人不能轻易还原原图
你需要写一个读取的函数，直接得到嵌入的图像
*/




Mat Arnold(Mat img, int re = 0) {

	// 映射矩阵两个数值k1, k2
	// 变换次数

	int row = img.rows, col = img.cols;
	int N = row;	
	Mat s(row, col, CV_8UC1);
	if (row != col) return s;

	int k1 = 1, k2 = 1;

	int a = 1, b = k1, c = k2, d = k1 * k2 + 1;

	if (re) {
		a = k1 * k2 + 1, b = -k1, c = -k2, d = 1;
	}

	for (int x = 0; x < row; x++) {
		for (int y = 0; y < col; y++) {
			auto color = img.at<uchar>(x, y);

			auto x1 = (a * x + b * y) % N;
			auto y1 = (c * x + d * y) % N;

			x1 = (x1 + N) % N;
			y1 = (y1 + N) % N;

			//  a % N = (a + N) % N 

			s.at<uchar>(x1, y1) = color;

		}
	}
	return s;
}

Mat RandMat(int h, int w, int k) {
	srand(k);
	Mat res(h, w, CV_8UC1);
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			res.at<uchar>(i, j) = rand() % 256;
		}
	}
	return res;
}


Mat Water(Mat img, Mat logo, int x, int y, double a) {
	// logo + x | y < img
	int row = logo.rows;
	int col = logo.cols;

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int tx = i + x;
			int ty = j + y;
			if (tx >= img.rows || ty >= img.cols) break;
			Vec3b c1 = img.at<Vec3b>(tx, ty);
			Vec3b c2 = logo.at<Vec3b>(i, j);

			Vec3b c = c1 * a + c2 * (1 - a);

			img.at<Vec3b>(tx, ty) = c;
		}
	}
	return img;
}

Mat Write_RGB(string img, string logo, int MappingCount = 0) {
	// 载体		logo
	Mat org = imread(img);// 彩色读入
	Mat logo_img = imread(logo, 0);// 灰度读入 之后会转换成二值图 > 125

	int row = org.rows, col = org.cols;
	Mat steg = org.clone();
	resize(logo_img, logo_img, Size(col, row));

	for (int i = 0; i < MappingCount; i++) {
		logo_img = Arnold(logo_img);
	}

	int _a = (row / 3) + 1; // 171
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			// 511 / 171 = 2
			int cl = rand();// 100以下的话就隐藏在0中，100~200 隐藏在1中 200以上2中
			//printf("%d\n", cl);
			// cl / 3 = x ... [y]  0 <= y <= 2  
			int x = org.at<Vec3b>(i, j)[cl];// [0 ~ 2] 表示 B G R的值， 0-B, 1-G 2-R
			// LSB
			int b = x & 1;
			int t = logo_img.at<uchar>(i, j) > 125; // 转2值
			if (b != t) {
				steg.at<Vec3b>(i, j)[cl] ^= 1;
			}
;		}
	}
	// 返回图像
	// 在蓝色通道最低位隐藏二值图像
	return steg;
}

Mat Read_RGB(string steg, int p, int MappingCount = 0) {
	// 读入一张图片 返回他蓝色通道最后一个位面的图
	Mat org = imread(steg); //BGR
	int row = org.rows, col = org.cols;
	Mat solve(row, col, CV_8UC1);

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			Vec3b x = org.at<Vec3b>(i, j);
			// x[0] & 1 -> 1 0 (255 0)
			if (p >= 0) solve.at<uchar>(i, j) = (x[p] & 1) * 255;
			else {
				int cl = (i + j + j) % 3;
				solve.at<uchar>(i, j) = (x[cl] & 1) * 255;
			}
		}
	}

	for (int i = 0; i < MappingCount; i++) {
		solve = Arnold(solve, 1);
	}
	return solve;
}

vector<int> getInt(int num, int c, int k) {
	srand(k);
	vector<int> v;
	for (int i = 0; i < num; i++) {
		v.push_back(rand() % c);
	}
	return v;
}

vector<int> IntToBin(vector<int> v) {
	// 整数向量转化到二进制向量
	vector<int> res;

	for (int i = 0; i < v.size(); i++) {
		int x = v[i];

		for (int j = 0; j <= 7; j++) {
			res.push_back((x & (1 << (7 - j))) > 0);
		}
	}
	return res;
}

vector<int> StrToBin(string v) {
	// 字符转化到二进制向量
	vector<int> res;

	for (int i = 0; i < v.size(); i++) {
		int x = v[i];

		for (int j = 0; j <= 7; j++) {
			res.push_back((x & (1 << (7 - j))) > 0);
		}
	}
	return res;
}

vector<int> BinToInt(vector<int> v) {
	vector<int> res;
	for (int i = 0; i < v.size(); i+=8) {
		int x = 0;
		for (int j = 0; j <= 7; j++) {
			x = x * 2 + v[i + j];
		}
		res.push_back(x);
	}
	return res;
}

string BinToStr(vector<int> v) {
	string res;
	for (int i = 0; i < v.size(); i += 8) {
		int x = 0;
		for (int j = 0; j <= 7; j++) {
			x = x * 2 + v[i + j];
		}
		res.push_back(x);
	}
	return res;
}

Mat Embed_vec(Mat img, vector<int> v) {
	int row = img.rows;
	int col = img.cols;
	int k = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (k >= v.size()) break;
			int b = v[k];
			uchar c = img.at<uchar>(i, j);
			c -= c & 1;
			c |= b;
			img.at<uchar>(i, j) = c;
			k += 1;
		}
	}
	return img;
}

vector<int> Ex_vec(Mat img, int len) {
	int row = img.rows;
	int col = img.cols;
	int k = 0;
	vector<int> v;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			if (k >= len) break;
			int c = img.at<uchar>(i, j) & 1;
			k += 1;
			v.push_back(c);
		}
	}
	return v;
}

Mat edge_up(Mat img) {
	int row = img.rows;
	int col = img.cols;

	Mat res(row, col, CV_8UC1);

	for (int i = 1; i < row; i++) {
		for (int j = 1; j < col ; j++) {

			res.at<uchar>(i, j) = abs(2 * img.at<uchar>(i, j) - img.at<uchar>(i - 1, j) - img.at<uchar>(i, j - 1)) * 10;

		}
	}

	return res;
}

int main() {

	/*
		@1 直接输入调用
		@2 命令行调用
	*/
	string  in_path;
	cout << "please input file path:";
	cin >> in_path;

	/*Mat org = imread(in_path, 0);


	string v0 = "{lena is cute.}";
	cout << "input embed string:";

	getchar();
	getline(cin, v0);
*/

	/*vector<int> v1 = StrToBin(v0);
	Mat e_org = Embed_vec(org, v1);

	imwrite("result.png", e_org);
*/


	Mat org = imread(in_path, 0);


	vector<int> v2 = Ex_vec(org, 10000);
	string v3 = BinToStr(v2);

	cout << v3 << endl;

	cout << "ok" << endl;
	system("pause");


	return 0;
	//Mat logo = imread("logo1.png", 0);
	//imshow("logo", logo);
	//Mat mapping_1 = Arnold(logo);
	////for (int i = 0; i < 45; i++) mapping_1 = Arnold(mapping_1);

	//imshow("logo_arnold", mapping_1);
	////Mat solve = Arnold(mapping_1, 1);
	////for (int i = 0; i < 45; i++) solve = Arnold(solve, 1);
	////imshow("logo_arnold_solve", solve);
	//waitKey(0);
	//return 0 ;

	//Mat steg = Write_RGB("std/LenaRGB.bmp","logo1.png", 10);
	//imwrite("steg/steg_rgb_arnold.png", steg);

	imshow("my_pic_B", Read_RGB("steg/steg_rgb_arnold_cut2.png", 0));//B
	imshow("my_pic_G", Read_RGB("steg/steg_rgb_arnold_cut2.png", 1));//G
	imshow("my_pic_R", Read_RGB("steg/steg_rgb_arnold_cut2.png", 2));//R
	imshow("my_pic", Read_RGB("steg/steg_rgb_arnold_cut2.png", -1, 10));//R
	waitKey(0);
	return 0;
}
