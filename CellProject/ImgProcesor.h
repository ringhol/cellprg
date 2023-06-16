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
	static const unsigned char MARK_VISITED = 0x81;//& 
	static const unsigned char MARKED = 0x80;//|=set & mark
	static const unsigned char VISITED = 0x01;//|= set 1 ,& visited
	static const unsigned char TWOVALUE_H = 0x80;	// =TWOVALUE_H: set 1
	static const unsigned char TWOVALUE_L = 0x0;
	static const unsigned char EDGEPOINT = 0x70;//|= set 1, & edge
	static const unsigned char pre_shrink_count = 3;
	static const unsigned char NO_MARK = 0x7f;//& set 0
	static const unsigned char NO_EDGE_POINT = 0x8f;//&= noedge
	static const unsigned char CENTERED = 0x2;//|= set 1 ,& center
	static const unsigned char NO_CENTER = 0xfd;//&= nocenter
	static const unsigned char NO_VISITED = 0xfe;//&= novisited 
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

	static void fillHole(CImage* image);

private:
	static void processFillHole(CImage* image,int i,int j);
};

