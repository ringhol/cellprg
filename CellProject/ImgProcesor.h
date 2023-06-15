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
	/// 标记图片中的细胞为红色，背景不变，可能是细胞的部分为蓝色
	/// </summary>
	/// <param name="image">待处理的图片</param>
	static void markCell(CImage* image,CPoint start,CPoint end);
	/// <summary>
	/// 拷贝一副图片
	/// </summary>
	/// <param name="orig"></param>
	/// <param name="dest"></param>
	/// <returns></returns>
	static bool copyImage(const CImage& orig, CImage& dest);
	/// <summary>
	/// 将可能标注的点进一步进行标注
	/// </summary>
	/// <param name="image"></param>
	static void maybemark2mark(CImage* image);

	/// <summary>
	/// 获取边缘数据
	/// </summary>
	/// <param name="image">经过标注后的图片</param>
	/// <param name="originImage">没有经过处理的原始图片</param>
	static void getEdgeInfomation(CImage* image,const CImage*originImage);

	static void twovalue(CImage** image);
};

