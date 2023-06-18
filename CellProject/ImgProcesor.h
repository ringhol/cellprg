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
	static long tot_area, tot_x, tot_y, max_radius;		// ���ڵݹ�


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
	/// <summary>
	/// ��ֵ��
	/// </summary>
	/// <param name="image"></param>
	static void twovalue(CImage** image);
	/// <summary>
	/// �
	/// </summary>
	/// <param name="image"></param>
	static void fillHole(CImage* image);
	/// <summary>
	/// ����
	/// </summary>
	/// <param name="image"></param>
	static void shrink(CImage* image);
	/// <summary>
	/// ��ȡ���ĵ�����
	/// </summary>
	/// <param name="image"></param>
	/// <returns></returns>
	static std::vector<CenterPoint> calCenter(CImage* image);
	/// <summary>
	/// ȡƽ��ֵ��ȡ���ĵ���Ŀ,������pdcʱ����ƽ������������ĵ㣬����һЩ����
	/// </summary>
	/// <param name="image"></param>
	/// <returns></returns>
	static std::vector<CenterPoint>calCenterWithAverage(CImage* image,CDC* pdc=nullptr,CPen* Redpen = nullptr, CPen* Greenpen = nullptr);

	/// <summary>
	/// ȥ����������Բ��Ǳ�ڴ���
	/// </summary>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	static void removeIncludedCircles(std::vector<CenterPoint>& centerPoints, CDC* pdc = nullptr,CPen* Bluepen1=nullptr);
	/// <summary>
	/// ȥ��С�뾶��Ǳ�ڴ���
	/// </summary>
	/// <param name="image"></param>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	static void removePoentialErrors(const CImage* image,std::vector<CenterPoint>& centerPoints, CDC* pdc = nullptr,CPen * Redpen1=nullptr);
	/// <summary>
	/// ȥ���ཻ��Ǳ�ڴ���
	/// </summary>
	/// <param name="image"></param>
	/// <param name="centerPoints"></param>
	/// <param name="pdc"></param>
	/// <param name="Bluepen1"></param>
	static void removePotentialErrorsIntersection(const CImage* image, std::vector<CenterPoint>& centerPoints, CDC* pdc,CPen* Bluepen1);

private:
	/// <summary>
	/// �ĳһ����
	/// </summary>
	/// <param name="image"></param>
	/// <param name="i"></param>
	/// <param name="j"></param>
	static void processFillHole(CImage* image,int i,int j);
	/// <summary>
	/// ������Χ8������û�б��������ɱ߽�
	/// </summary>
	/// <param name="image"></param>
	static void genEdge8(CImage* image);
	/// <summary>
	/// ������Χ�ĸ�����û�б��������ɱ߽�
	/// </summary>
	/// <param name="image"></param>
	static void genEdge4(CImage* image);

	static bool isNeedToSave(CImage* image, int i, int j);

	static void saveIt(CImage* image,std::vector<CenterPoint>&points, int i, int j, int k);

	static void calCenterArea(CImage* image, std::vector<CenterPoint>& points_temp, int i, int j);
};

