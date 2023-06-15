#pragma once

#define DISTANCE(x0,y0,x1,y1) sqrt((((float)(x0)-(x1))*((x0)-(x1))+((y0)-(y1))*((y0)-(y1))))

class Rgb {
public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	Rgb(COLORREF pixel) :r(GetRValue(pixel)), g(GetGValue(pixel)), b(GetBValue(pixel)){};
	Rgb(unsigned char r, unsigned char g, unsigned char b) :r(r), g(g), b(b) {}
	Rgb() { r = 0; g = 0; b = 0; }
};
	
class HSI {
public:
	double H;
	double S;
	double I;
	HSI(const Rgb &rgb);
	HSI(COLORREF pixel);
	HSI() { H = 0; S = 0; I = 0; };
	HSI(double H, double S, double I) :H(H),S(S),I(I){};
};

class ImgProcesor
{
public:
	/// <summary>
	/// ���ͼƬ�е�ϸ��Ϊ��ɫ���������䣬������ϸ���Ĳ���Ϊ��ɫ
	/// </summary>
	/// <param name="image">�������ͼƬ</param>
	static void markCell(CImage* image,CPoint start,CPoint end);
	/// <summary>
	/// ����һ��ͼƬ
	/// </summary>
	/// <param name="orig"></param>
	/// <param name="dest"></param>
	/// <returns></returns>
	static bool copyImage(const CImage& orig, CImage& dest);
	/// <summary>
	/// �����ܱ�ע�ĵ��һ�����б�ע
	/// </summary>
	/// <param name="image"></param>
	static void maybemark2mark(CImage* image);

	/// <summary>
	/// ��ȡ��Ե����
	/// </summary>
	/// <param name="image">������ע���ͼƬ</param>
	/// <param name="originImage">û�о��������ԭʼͼƬ</param>
	static void getEdgeInfomation(CImage* image,const CImage*originImage);

	static void twovalue(CImage** image);
};

