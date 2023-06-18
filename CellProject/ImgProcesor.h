#pragma once
#include<vector>
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

struct CenterPoint
{
	int x;
	int y;
	int radius;
};

class ImgProcesor
{
public:
	static const unsigned char MARK_VISITED = 0x81;
	static const unsigned char MARKED = 0x80;
	static const unsigned char VISITED = 0x01;
	static const unsigned char TWOVALUE_H = 0x80;
	static const unsigned char TWOVALUE_L = 0x0;
	static const unsigned char EDGEPOINT = 0x70;
	static const unsigned char pre_shrink_count = 3;
	static const unsigned char NO_MARK = 0x7f;
	static const unsigned char NO_EDGE_POINT = 0x8f;
	static const unsigned char CENTERED = 0x2;
	static const unsigned char NO_CENTER = 0xfd;
	static const unsigned char NO_VISITED = 0xfe;
	static bool m_bFullEdge;
	static long tot_area, tot_x, tot_y, max_radius;		// 用于递归


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
	/// <summary>
	/// 二值化
	/// </summary>
	/// <param name="image"></param>
	static void twovalue(CImage** image);
	/// <summary>
	/// 填洞
	/// </summary>
	/// <param name="image"></param>
	static void fillHole(CImage* image);
	/// <summary>
	/// 收缩
	/// </summary>
	/// <param name="image"></param>
	static void shrink(CImage* image);
	/// <summary>
	/// 获取中心点数据
	/// </summary>
	/// <param name="image"></param>
	/// <returns></returns>
	static std::vector<CenterPoint> calCenter(CImage* image);
	/// <summary>
	/// 取平均值获取中心点数目,当传入pdc时返回平均化相近的中心点，并做一些绘制
	/// </summary>
	/// <param name="image"></param>
	/// <returns></returns>
	static std::vector<CenterPoint>calCenterWithAverage(CImage* image,CDC* pdc=nullptr,CPen* Redpen = nullptr, CPen* Greenpen = nullptr);

	/// <summary>
	/// 去除被包含的圆的潜在错误
	/// </summary>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	static void removeIncludedCircles(std::vector<CenterPoint>& centerPoints, CDC* pdc = nullptr,CPen* Bluepen1=nullptr);
	/// <summary>
	/// 去除小半径的潜在错误
	/// </summary>
	/// <param name="image"></param>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	static void removePoentialErrors(const CImage* image,std::vector<CenterPoint>& centerPoints, CDC* pdc = nullptr,CPen * Redpen1=nullptr);
	/// <summary>
	/// 去除相交的潜在错误
	/// </summary>
	/// <param name="image"></param>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	/// <param name="Bluepen1"></param>
	static void removePotentialErrorsIntersection(const CImage* image, std::vector<CenterPoint>& centerPoints, CDC* pdc,CPen* Bluepen1);

private:
	/// <summary>
	/// 填洞某一个洞
	/// </summary>
	/// <param name="image"></param>
	/// <param name="i"></param>
	/// <param name="j"></param>
	static void processFillHole(CImage* image,int i,int j);
	/// <summary>
	/// 根据周围8个点有没有背景来生成边界
	/// </summary>
	/// <param name="image"></param>
	static void genEdge8(CImage* image);
	/// <summary>
	/// 根据周围四个点有没有背景来生成边界
	/// </summary>
	/// <param name="image"></param>
	static void genEdge4(CImage* image);

	static bool isNeedToSave(CImage* image, int i, int j);

	static void saveIt(CImage* image,std::vector<CenterPoint>&points, int i, int j, int k);

	static void calCenterArea(CImage* image, std::vector<CenterPoint>& points_temp, int i, int j);
};

