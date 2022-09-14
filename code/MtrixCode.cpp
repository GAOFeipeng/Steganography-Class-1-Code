#include  <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;


const int K = 15, LEN_A = (1 << K) - 1;

int C = 0;

vector<int> StrToBin(string v) {
	// 字符转化到二进制向量
	vector<int> res;

	for (int i = 0; i < v.size(); i++) {
		int x = v[i];

		for (int j = 0; j <= 7; j++) {
			res.push_back((x & (1 << (7 - j))) > 0);
		}
	}

	for (int i = 0; i <= 7; i++) res.push_back(0);
	return res;
}

string BinToStr(vector<int> v) {
	string res;
	for (int i = 0; i < v.size(); i += 8) {
		int x = 0;
		for (int j = 0; j <= 7; j++) {
			if (i + j >= v.size()) break;
			x = x * 2 + v[i + j];
		}
		res.push_back(x);
	}
	return res;
}
/* 编解码 不仅限于字符串 ...*/

int MatrixCode_N(vector<int> a, vector<int> x) {
	int n = a.size(), m = x.size();

	assert(n == (1 << m) - 1); // n == 2 ^ m - 1

	int fa = 0;

	for (int i = 1; i <= n; i++) fa ^= (a[i - 1] * i);

	int xx = 0; // x[0] -> x1 * 2^0 + x2 * 2^1 ...

	for (int i = 0; i < m; i++) xx += x[i] * (1 << i);

	return fa ^ xx;
}

vector<int> Matrix_EX(vector<int> a) {
	int n = a.size();
	int m = log2(n + 1);

	vector<int> x;

	int fa = 0;

	for (int i = 1; i <= n; i++) fa ^= (a[i - 1] * i);
	for (int i = 1; i <= m; i++) {
		x.push_back(fa % 2);
		fa /= 2;
	}
	return x;
}

/*矩阵编码嵌入和读取*/

Mat embedMatrix(Mat img, string s) {
	vector<int> v0, v1;
	v1 = StrToBin(s);
	int row = img.rows;
	int col = img.cols;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			uchar c = img.at<uchar>(i, j);
			v0.push_back(c & 1);
		}
	}
	/*载体选择算法*/
	// 准备载体和秘密消息

	int p = 0;

	for (int i = 0; i < v1.size(); i += K) {

		vector<int> x;
		for (int j = 0; j < K; j++) {
			if (i + j < v1.size()) x.push_back(v1[i + j]);
			else x.push_back(0); // padding
		}


		vector<int> a;
		for (int j = 0; j < LEN_A; j++) a.push_back(v0[p + j]);

		int s = MatrixCode_N(a, x);

		if (s != 0) {
			v0[p + s - 1] ^= 1;
			C++;
		}

		p += LEN_A;
	}


	p = 0;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			uchar &c = img.at<uchar>(i, j);

			c -= (c & 1);
			c += v0[p];

			p++;
		}
	}


	return img;
}


string exMatrix(Mat img) {
	vector<int> v0, v1;
	int row = img.rows;
	int col = img.cols;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			uchar c = img.at<uchar>(i, j);
			v0.push_back(c & 1);
		}
	}

	for (int p = 0; p < v0.size() / LEN_A * LEN_A; p += LEN_A) {


		vector<int> a;
		for (int j = 0; j < LEN_A; j++) a.push_back(v0[p + j]);

		vector<int> x = Matrix_EX(a);

		for (auto &xx : x) v1.push_back(xx);

	}
	return BinToStr(v1);

}
int main() {
	Mat img = imread("std/Lena.bmp", 0);

	Mat mat = embedMatrix(img, "ABCDEFQWE.");

	imwrite("lena_1.png", mat);

	/*string s = exMatrix(img);
	cout << s << endl;
	*/
	cout << "修改幅度为：" << C << endl;
	return 0;
}
