#include "pch.h"
#include "ImgProcesor.h"
#include <math.h>
#include <stack>
#include <vector>
HSI::HSI(COLORREF pixel) {
	Rgb rgbval(pixel);
	HSI::HSI(rgbval);
}

HSI::HSI(const Rgb& rgb) {
	double R = ((double)rgb.r) / 255.0;
	double G = ((double)rgb.g) / 255.0;
	double B = ((double)rgb.b) / 255.0;
	double sum = R + G + B;
	I = sum / 3.0;
	double minval = min(R, G);
	minval = min(B, minval);
	double maxval = max(R, G);
	maxval = max(B, maxval);
	if (I < 0.00001)S = 0;
	else S = 1.0 - (3.0 * minval) / sum;
	if (minval == maxval) {
		H = NAN;
		S = 0;
		return;
	}
	double tmp1 = ((R - G) + (R - B)) / 2.0;
	double tmp2 = pow(R - G, 2) + (R - B) * (G - B);
	double quotient = tmp1 / sqrt(tmp2);
	double angle = 180.0 / 3.14159 * acos(quotient);
	H = (B > G) ? 360.0 - angle : angle;
}

//bool ImgProcesor::m_bFullEdge = false;
long ImgProcesor::tot_area = 0; 
long ImgProcesor::tot_x = 0; 
long ImgProcesor::tot_y = 0;
long ImgProcesor::max_radius = 0;		// ���ڵݹ�



bool ImgProcesor::copyImage(const CImage& srcImage, CImage& destImage){
	int i;//ѭ������
	if (srcImage.IsNull())
		return FALSE;
	//Դͼ�����
	BYTE* srcPtr = (BYTE*)srcImage.GetBits();
	int srcBitsCount = srcImage.GetBPP();
	int srcWidth = srcImage.GetWidth();
	int srcHeight = srcImage.GetHeight();
	int srcPitch = srcImage.GetPitch();
	//����ԭ��ͼ��
	if (!destImage.IsNull()){
		destImage.Destroy();
	}
	//������ͼ��
	//֧��alphaͨ��
	if (srcBitsCount == 32){
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//���ص�ɫ��
	if (srcBitsCount <= 8 && srcImage.IsIndexed()){
		RGBQUAD pal[256];
		int nColors = srcImage.GetMaxColorTableEntries();
		if (nColors > 0){
			srcImage.GetColorTable(0, nColors, pal);
			destImage.SetColorTable(0, nColors, pal);//���Ƶ�ɫ�����
		}
	}
	//Ŀ��ͼ�����
	BYTE* destPtr = (BYTE*)destImage.GetBits();
	int destPitch = destImage.GetPitch();
	//����ͼ������
	for (i = 0; i < srcHeight; i++){
		memcpy(destPtr + i * destPitch, srcPtr + i * srcPitch, abs(srcPitch));
	}
	return TRUE;
}


void ImgProcesor::markCell(CImage* image, CPoint start, CPoint end) {

	constexpr double meanH = 210.0 * 360 / 255;
	constexpr double meanS = 55.0 / 255;
	const double markDoor = 0.09;
	const double maybeMarkDoor = 0.15;
	for (int i = start.x; i < end.x; i++) {
		for (int j = start.y; j < end.y; j++) {
			BYTE* pByte = (BYTE*)image->GetPixelAddress(i, j);
			Rgb rgb(pByte[2], pByte[1], pByte[0]);
			//����HSI
			HSI hsi(rgb);

			double x1 = hsi.H;
			double x2 = meanH;
			if (x1 < 90)x1 += 360;
			double y1 = hsi.S;
			double y2 = meanS;
			x1 /= 180; x2 /= 180;
			double dis = DISTANCE(x1, y1, x2, y2);
			if (dis < markDoor) {
				//��Ǻ�ɫ
				pByte[2] = 255; pByte[1] = 0; pByte[0] = 0;
			}
			else if (dis < maybeMarkDoor) {
				//�����ɫ
				pByte[2] = 0; pByte[1] = 0; pByte[0] = 255;
			}
			else {
				if (rgb.b == 0) {
					pByte[2] = rgb.r; pByte[1] = rgb.g; pByte[0] = 1;
				}
				else if (rgb.b == 255) {
					pByte[2] = rgb.r; pByte[1] = rgb.g; pByte[0] = 254;
				}
				else if (rgb.r == 128) {
					pByte[2] = 127; pByte[1] = rgb.g; pByte[0] = rgb.b;
				}
				if (rgb.g == 255) {
					pByte[2] = rgb.r; pByte[1] = 254; pByte[0] = rgb.b;
				}
			}
		}
	}
}


void ImgProcesor::maybemark2mark(CImage* image) {
	bool MarkChg = true;//�Ƿ�����Ҫ�ı�ĵ�
	while (MarkChg) {
		MarkChg = false;
		for (int i = 0; i < image->GetWidth(); i++) {
			for (int j = 0; j < image->GetHeight(); j++) {
				BYTE* pByte = (BYTE*)image->GetPixelAddress(i, j);
				if (*pByte == 255) {//maybe Mark 
					bool bProc = false;
					if ((j > 0 && *(BYTE*)image->GetPixelAddress(i, j - 1) == 0)
						|| (j < image->GetHeight() - 1 && *(BYTE*)image->GetPixelAddress(i, j + 1) == 0)
						|| (i > 0 && *(BYTE*)image->GetPixelAddress(i - 1, j) == 0)
						|| (i < image->GetWidth() - 1 && *(BYTE*)image->GetPixelAddress(i + 1, j) == 0)) {
						bProc = true;
					}
					// maybe Mark have Mark Point to Mark 
					if (bProc) {
						pByte[0] = 0;
						MarkChg = true;
						*(pByte + 2) = MARKED;
					}
				}
			}
		}
	}
}

void ImgProcesor::getEdgeInfomation(CImage* image, const CImage* originImage) {
	const int edgeDoor = 45;
	CImage oriImg;
	copyImage(*originImage, oriImg);
	//get edge information
	for (int i = 0 + 1; i < image->GetHeight() - 1; i++) {//boarder no edge
		for (int j = 0 + 1; j < image->GetWidth() - 1; j++) {
			BYTE* lpDst = (BYTE*)image->GetPixelAddress(j, i);
			if (*(lpDst) == 0 || *(lpDst) == 255) {//Mark /Maybe Mark
				double pixel[9];
				for (int m = -1; m < 2; m++)                    //3*3����
					for (int n = -1; n < 2; n++) {
						BYTE* lpSrc1 = (BYTE*)oriImg.GetPixelAddress(j, i+m);
						pixel[(m + 1) * 3 + n + 1] = ((int)*lpSrc1 + *(lpSrc1 + 1) + *(lpSrc1 + 2)) / 3;      //���޸�
					}
				//Sobel
				double tmp1 = pixel[0] + 2 * pixel[1] + pixel[2] - pixel[6] - 2 * pixel[7] - pixel[8];
				double tmp2 = pixel[0] + 2 * pixel[3] + pixel[6] - pixel[2] - 2 * pixel[5] - pixel[8];
				double edge = sqrt(tmp1 * tmp1 + tmp2 * tmp2);
				if (edge > edgeDoor) {
					*(lpDst + 1) = 255;//edge
				}
			}
		}
	}

	//�˲�
	const int M = 5;  //5*5�����˲�
	bool bdelete;
	//filter 
	for (int i = 0 + M; i < image->GetHeight() - M; i++) {
		// ���ÿ��ͼ��ÿ�н��в���
		for (int j = 0 + M; j < image->GetWidth() - M; j++) {
			// ָ��ԴDIB��i�У���j�����ص�ָ��
			BYTE* lpDst = (BYTE*)image->GetPixelAddress(j, i);
			//edge 
			if (*(lpDst + 1) == 255) {
				bdelete = true;
				for (int m = -M; m <= M; m++)
					for (int n = -M; n <= M; n++) {
						if (m == -M || m == M || n == -M || n == M) {
							BYTE* lpDst1 = (BYTE*)image->GetPixelAddress(j, i-m)+1;
							//noMark && no Edge
							if (*(lpDst1)|| (*(lpDst1 + 1) == 255)) {
								bdelete = false;
								m = M + 1; n = M + 1;//out
							}
						}
					}
				if (bdelete)
					*(lpDst + 1) = 0;//delete edge
			}
		}
	}
}


void ImgProcesor::twovalue(CImage** image) {
		// ��ʱ�����ڴ棬�Ա�����ͼ��
	CImage* newImage = new CImage;
	newImage->Create((*image)->GetWidth(), (*image)->GetHeight(), 8);
	// �ڵ�ɫ���г�ʼ��256����ɫֵ
	RGBQUAD colors[256];
	for (int i = 0; i < 256; i++) {
		colors[i].rgbBlue = i;
		colors[i].rgbGreen = i;
		colors[i].rgbRed = i;
		colors[i].rgbReserved = 0;
	}
	// ����ɫ�帳ֵ��ͼ��
	if (newImage->GetDC() != NULL) {
		::SetDIBColorTable(newImage->GetDC(), 0, 256, colors);
		newImage->ReleaseDC();
	}
	for (int i = 0; i < (*image)->GetHeight(); i++) {
		for (int j = 0; j < (*image)->GetWidth(); j++) {
			BYTE* lpSrc = (BYTE*)(*image)->GetPixelAddress(j, i);
			BYTE* lpDst = (BYTE*)newImage->GetPixelAddress(j, i);
			BYTE v = 0;
			//Mark
			if (*(lpSrc) == 0) {
				v = TWOVALUE_H;
				if (*(lpSrc + 1))v |= EDGEPOINT;//set edge
				else if (j == 0 || j == newImage->GetWidth() - 1 || i == 0 || i == newImage->GetHeight() - 1) {
					v |= EDGEPOINT;
				}
			}
			*lpDst = v;
		}
	}
	delete* image;
	*image = newImage;
	newImage->ReleaseDC();//���ﲻ�����һ�εĻ�delete��ʱ��ͻᱨ��
}


std::vector<Hole> ImgProcesor::fillHole(CImage* image){
	//0x7X---edge
	//0x8X---Mark--not edge
	//0xfX--Mark --edge
	//0xX1---visited
	std::vector<Hole> holes;
	for (int i = 0 + 1; i < image->GetHeight() ; i++){
		for (int j = 0 + 1; j < image->GetWidth() ; j++){
			BYTE* lpSrc = (BYTE*)image->GetPixelAddress(j, i);
			//if no-marked & no-visited
			if (!(*lpSrc & MARK_VISITED)) {//δ���ʹ��ĺڵ�
				auto hole = processFillHole(image,j, i);
				if (hole.size != 0) {
					holes.push_back(hole);
				}
			}
		}
	}
	//edge area back
	//0xfX--Mark --edge
	for (int i = 0; i < image->GetHeight(); i++){
		for (int j = 0; j < image->GetWidth(); j++){
			BYTE* lpSrc = (BYTE*)image->GetPixelAddress(j, i);
			if (!(*lpSrc & MARKED))//��mark point
				*lpSrc = 0;//ɾ�����ʱ�־
			else if (*lpSrc & EDGEPOINT)//if marked & edge
					*lpSrc = 0;
		}
	}
	return holes;
}



Hole ImgProcesor::processFillHole(CImage* image, int x, int y){
	using namespace std;
	stack<CPoint> s;
	vector<CPoint> v;//v save for fill holes
	const int MAX_HOLE = 100;
	s.push(CPoint(x, y));
	v.push_back(CPoint(x, y));
	BYTE* lpSrc =  (BYTE*)image->GetPixelAddress(x, y);
	*lpSrc |= VISITED;//vistied	
	bool bBorder = false;
	while (!s.empty()){
		//Add new members to stack
		//Above current pixel
		lpSrc = (BYTE*)image->GetPixelAddress(x, y);
		if (y > 0&&y<image->GetHeight()-1&& x > 0 && x < image->GetWidth()-1){
			//if no-marked & no-visited
			if (/*y > 0 &&*/ !(*(BYTE*)image->GetPixelAddress(x, y - 1) & MARK_VISITED)) {
				s.push({ x, y - 1 });
				v.push_back({ x, y - 1 });
				*(BYTE*)image->GetPixelAddress(x, y - 1) |= VISITED;
			}

			if (/*y<image->GetHeight()-1&&*/!(*(BYTE*)image->GetPixelAddress(x, y + 1) & MARK_VISITED)) {
				s.push({ x, y + 1 });
				v.push_back({ x, y + 1 });
				*(BYTE*)image->GetPixelAddress(x, y + 1) |= VISITED;
			}

			if (/*x>0&&*/!(*(BYTE*)image->GetPixelAddress(x - 1, y) & MARK_VISITED)) {
				s.push({ x - 1, y });
				v.push_back({ x - 1, y });
				*(BYTE*)image->GetPixelAddress(x - 1, y) |= VISITED;
			}

			if (/*x<image->GetWidth()-1&&*/!(*(BYTE*)image->GetPixelAddress(x + 1, y) & MARK_VISITED)) {
				s.push({ x + 1, y });
				v.push_back({ x + 1, y });
				*(BYTE*)image->GetPixelAddress(x + 1, y) |= VISITED;
			}
		}
		else bBorder = true;
		//Retrieve current stack member
		x = s.top().x;
		y = s.top().y;
		s.pop();
	}
	if (v.size() < MAX_HOLE && !bBorder){
		for (UINT k = 0; k < v.size(); k++) {
			lpSrc = (BYTE*)image->GetPixelAddress(v[k].x, v[k].y);
			*lpSrc |= MARKED;
		}
		return { x,y,v.size() };
	}
	return { -1,-1,0 };
}




void ImgProcesor::shrink(CImage* image){
	const int pre_shrink_count = 3;
	// ��ȥ��pre_shrink_count��Ƥ
	genEdge8(image);
	for (int k = 0; k < pre_shrink_count; k++) {
		for (int i =0; i < image->GetHeight(); i++) {
			for (int j = 0; j < image->GetWidth(); j++) {
				BYTE* lpSrc = (BYTE*)image->GetPixelAddress(j, i);
				// ȥ���߽�
				if (*lpSrc & EDGEPOINT)(*lpSrc) &= NO_MARK;//marked=0;
			}
		}
		if (k % 2 == 0)genEdge8(image);
		else genEdge4(image);
	}
}

void ImgProcesor::genEdge8(CImage* image){
	for (int j = 0; j < image->GetHeight(); j++){
		for (int i = 0; i < image->GetWidth(); i++){
			BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i,j);
			*lpSrc &= NO_EDGE_POINT;//no edge
			if (*lpSrc & MARKED){//marked
				if (j == 0 || i == 0 || j == image->GetHeight()-1 || i == image->GetWidth()-1){
					*lpSrc |= EDGEPOINT;
				}else if(// normal
						!((*(BYTE*)image->GetPixelAddress(i+1, j) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i-1, j) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i, j+1) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i, j-1) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i+1, j+1) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i+1, j-1) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i-1, j+1) & MARKED)
							&& (*(BYTE*)image->GetPixelAddress(i-1, j-1) & MARKED)
							)
						)* lpSrc |= EDGEPOINT;
			}
		}
	}
}

void ImgProcesor::genEdge4(CImage* image){
	for (int j = 0; j < image->GetHeight(); j++) {
		for (int i = 0; i < image->GetWidth(); i++) {
			BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i, j);
			*lpSrc &= NO_EDGE_POINT;//no edge
			if (*lpSrc & MARKED) {//marked
				if (j == 0 || i == 0 || j == image->GetHeight() - 1 || i == image->GetWidth() - 1) {
					*lpSrc |= EDGEPOINT;
				}
				else if (// normal
					!((*(BYTE*)image->GetPixelAddress(i + 1, j) & MARKED)
						&& (*(BYTE*)image->GetPixelAddress(i - 1, j) & MARKED)
						&& (*(BYTE*)image->GetPixelAddress(i, j + 1) & MARKED)
						&& (*(BYTE*)image->GetPixelAddress(i, j - 1) & MARKED))
					)*lpSrc |= EDGEPOINT;
			}
		}
	}
}



std::vector<CenterPoint> ImgProcesor::calCenter(CImage* image){
	using namespace std;
	vector<CenterPoint> points_temp;
	bool changed = true;
	BYTE* lpSrc;
	for (int k = 0; changed; k++){// ��־���ĵ�ĸ�ʴ	
		changed = false;
		// ���visited��־
		for (int j = 0; j < image->GetHeight(); j++){
			for (int i = 0; i < image->GetWidth(); i++){
				lpSrc = (BYTE*)image->GetPixelAddress(i,j);
				*lpSrc &= NO_VISITED;//
			}
		}
		for (int j = 0; j < image->GetHeight(); j++){
			for (int i =0; i < image->GetWidth(); i++){
					if (j > 0 && j<image->GetHeight()-1 && i>0 && i < image->GetWidth()-1) // ����ϵĲ��ô���
					{
						lpSrc = (BYTE*)image->GetPixelAddress(i,j);
						if (*lpSrc & EDGEPOINT && !(*lpSrc & VISITED))	// û�з��ʹ��ı߽�
						{
							if (!(*(BYTE*)image->GetPixelAddress(i, j - 1) & MARKED) &&
								!(*(BYTE*)image->GetPixelAddress(i, j + 1) & MARKED) &&
								!(*(BYTE*)image->GetPixelAddress(i - 1, j) & MARKED) &&
								!(*(BYTE*)image->GetPixelAddress(i + 1, j) & MARKED)/*&&
								
								!(*(BYTE*)image->GetPixelAddress(i + 1, j+1) & MARKED)&&
								!(*(BYTE*)image->GetPixelAddress(i + 1, j-1) & MARKED)&&
								!(*(BYTE*)image->GetPixelAddress(i - 1, j+1) & MARKED)&&
								!(*(BYTE*)image->GetPixelAddress(i - 1, j-1) & MARKED)*/){
								if (k <= 2){ // ����������������
									continue;
								}
								// �����ĵ�
								*lpSrc |= CENTERED;
								// ����һ��CENTER_POINT��Ϣ
								points_temp.push_back({i,j,k+pre_shrink_count+4/*circle adjust */});
								continue;
							}
							else if (isNeedToSave(image, i, j)) {
								// �ж��Ƿ���Ҫ����
								// û�з��ʹ���־�˲����ǷǱ�Ե����
								saveIt(image, points_temp, i, j, k + pre_shrink_count + 4);//����
							}
						}
					}
			}
		}

		for (int j = 0; j < image->GetHeight(); j++){
			for (int i = 0; i < image->GetWidth(); i++){
				lpSrc = (BYTE*)image->GetPixelAddress(i, j);
				// ȥ���߽�!
				if (*lpSrc & EDGEPOINT){
					changed = true;
					*lpSrc &= NO_MARK;
				}
			}
		}
		if (k % 2 == 0)genEdge4(image);
		else genEdge8(image);
	}
	return points_temp;
}


std::vector<CenterPoint> ImgProcesor::calCenterWithAverage(CImage* image, CDC* pdc, CPen* Redpen,CPen* Greenpen) {
	// ȡƽ��ֵ,������ĵ�
	using namespace std;
	auto points_temp = calCenter(image);
	vector<CenterPoint> points;
	CenterPoint pt;
	for (int j = 0; j < image->GetHeight(); j++) {
		for (int i = 0; i < image->GetWidth(); i++) {
			if (j > 0 && j < image->GetHeight() - 1 && i>0 && i < image->GetWidth() - 1) { // ����ϵĲ��ô���
				BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i, j);
				if (*lpSrc & CENTERED)
				{
					if (!(*(BYTE*)image->GetPixelAddress(i - 1, j) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i + 1, j) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i, j - 1) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i, j + 1) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i - 1, j - 1) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i - 1, j + 1) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i + 1, j - 1) & CENTERED) &&
						!(*(BYTE*)image->GetPixelAddress(i + 1, j + 1) & CENTERED)) {
						// �����ĵ�
						pt.x = i;
						pt.y = j;
						for (unsigned int n = 0; n < points_temp.size(); n++) {
							if (points_temp.at(n).x == i && points_temp.at(n).y == j) {
								pt.radius = points_temp.at(n).radius;
								break;
							}
						}
						points.push_back(pt);
						continue;
					}
					else {							//����������ʵ�
						tot_area = 0;
						max_radius = 0;
						tot_x = 0;
						tot_y = 0;
						calCenterArea(image, points_temp, i, j);         //�������������������ĵ���ʵ������뾶��ȥ�������ĵ��־
						pt.x = tot_x / tot_area;
						pt.y = tot_y / tot_area;
						pt.radius = max_radius;
						*(lpSrc - (pt.y - j) * image->GetWidth() * image->GetBPP() / 8 + pt.x - i) |= CENTERED;
						points.push_back(pt);
					}
				}
			}
		}
	}
	if (pdc != nullptr) {
		vector<CenterPoint> center_points;
		for (unsigned int i = 0; i < points.size(); i++){
			int x0 = points.at(i).x;
			int y0 = points.at(i).y;
			pt = points.at(i);
			bool adj = false;
			//Red ���- delete

			for (unsigned int j = i + 1; j < points.size() - 1; j++){
				int x = points.at(j).x;
				int y = points.at(j).y;
				if (abs(x0 - x) + abs(y0 - y) < 10){ // ��� //Բ�ľ���
					points.at(i).x = (x + x0) / 2;
					points.at(i).y = (y + y0) / 2;
					points.at(i).radius = (points.at(i).radius + points.at(j).radius) / 2;
					pt = points.at(j);
					//display err position--delete 
					if (Redpen != nullptr){
						pdc->SelectObject(Redpen);
						Arc(pdc->m_hDC,//-3 for display 
							pt.x - pt.radius + 3,
							pt.y - pt.radius + 3,
							pt.x + pt.radius - 3,
							pt.y + pt.radius - 3,
							pt.x + pt.radius - 3,
							pt.y - 3,
							pt.x + pt.radius - 3,
							pt.y - 3
						);
					}
					
					points.erase(points.begin() + j);//&points.at(j));
					i--;
					adj = true;
					break;
				}
			}
			if (!adj){ // �����
				if (points.at(i).radius > 4){
					center_points.push_back(points.at(i));
					if (Greenpen != nullptr) {
						pdc->SelectObject(Greenpen);
						Arc(pdc->m_hDC,
							pt.x - pt.radius,
							pt.y - pt.radius,
							pt.x + pt.radius,
							pt.y + pt.radius,
							pt.x + pt.radius,
							pt.y,
							pt.x + pt.radius,
							pt.y
						);
					}
				}
			}
		}
		return center_points;
	}
	return points;
}


bool ImgProcesor::isNeedToSave(CImage* image, int i, int j)
{
	bool res = true;
	BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i, j);
	*(lpSrc) |= VISITED;

	if (j == 0 || j == image->GetHeight()-1 || i == 0 || i == image->GetWidth()-1)return false; // ����ϵĲ��ô���
		
	if (!(*(BYTE*)image->GetPixelAddress(i - 1, j) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i - 1, j) & MARKED){// ��־��
		if (*(BYTE*)image->GetPixelAddress(i - 1, j) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i - 1, j); // ���
		else
			res = false;
	}

	if (!(*(BYTE*)image->GetPixelAddress(i+1, j) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i + 1, j) & MARKED){		// ��־��
		if (*(BYTE*)image->GetPixelAddress(i + 1, j) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i + 1, j); // �ұ�
		else
			res = false;
	}



	if (!(*(BYTE*)image->GetPixelAddress(i, j - 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i, j - 1) & MARKED){	// ��־��
		if (*(BYTE*)image->GetPixelAddress(i, j - 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i, j - 1); // ����
		else
			res = false;
	}

	if (!(*(BYTE*)image->GetPixelAddress(i, j + 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i, j + 1) & MARKED) {	// ��־��
		if (*(BYTE*)image->GetPixelAddress(i, j + 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image, i, j + 1); // ����
		else
			res = false;
	}

	if (!(*(BYTE*)image->GetPixelAddress(i-1, j + 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i - 1, j + 1) & MARKED)		// ��־��
	{
		if (*(BYTE*)image->GetPixelAddress(i - 1, j + 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i - 1, j + 1); // ����
		else
			res = false;
	}

	if (!(*(BYTE*)image->GetPixelAddress(i - 1, j - 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i - 1, j - 1) & MARKED){		// ��־��
		if (*(BYTE*)image->GetPixelAddress(i - 1, j - 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i - 1, j - 1); // ����
		else
			res = false;
	}
	if (!(*(BYTE*)image->GetPixelAddress(i + 1, j - 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i + 1, j - 1) & MARKED) {		// ��־��
		if (*(BYTE*)image->GetPixelAddress(i + 1, j - 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image, i + 1, j - 1); // ����
		else
			res = false;
	}
	if (!(*(BYTE*)image->GetPixelAddress(i + 1, j + 1) & VISITED) &&	// û�з��ʹ�
		*(BYTE*)image->GetPixelAddress(i + 1, j + 1) & MARKED){		// ��־��
		if (*(BYTE*)image->GetPixelAddress(i + 1, j + 1) & EDGEPOINT)		// �����Ǳ�Ե
			res = isNeedToSave(image,i + 1, j + 1); // ����
		else
			res = false;
	}
	return res;
}

void ImgProcesor::saveIt(CImage* image, std::vector<CenterPoint>& points_temp, int i, int j, int radius)
{
	using namespace std;
	BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i,j);
	if (j == 0 || j == image->GetHeight()-1 || i ==0 || i == image->GetWidth()-1) // ����ϵĲ��ô���
		return;
	if (!(*lpSrc & CENTERED)) {
		points_temp.push_back({i,j,radius});
		// marke current point
		*lpSrc |= CENTERED;
	}
	*lpSrc &= NO_VISITED;//���ʹ��Ž�ջ��ȥ�����ν�ջ
	if (*(BYTE*)image->GetPixelAddress(i-1, j) & VISITED){
		saveIt(image, points_temp,i - 1, j, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i+1, j) & VISITED){
		saveIt(image, points_temp,i + 1, j, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i, j-1) & VISITED){
		saveIt(image, points_temp,i, j - 1, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i, j+1) & VISITED){
		saveIt(image, points_temp,i, j + 1, radius);
	}

	if (*(BYTE*)image->GetPixelAddress(i+1, j+1) & VISITED){
		saveIt(image, points_temp,i + 1, j + 1, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i-1, j+1) & VISITED){
		saveIt(image, points_temp,i - 1, j + 1, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i+1, j-1) & VISITED){
		saveIt(image, points_temp,i + 1, j - 1, radius);
	}
	if (*(BYTE*)image->GetPixelAddress(i-1, j-1) & VISITED){
		saveIt(image, points_temp,i - 1, j - 1, radius);
	}
}

void ImgProcesor::calCenterArea(CImage* image,std::vector<CenterPoint>& points_temp, int i, int j){
	BYTE* lpSrc = (BYTE*)image->GetPixelAddress(i,j);
	if (j == 0 || j == image->GetHeight()-1 || i == 0 || i == image->GetWidth()-1){ // ����ϵĲ��ô���
		return;
	}
	tot_area++;
	tot_x += i;
	tot_y += j;
	*lpSrc &= NO_CENTER;
	for (unsigned int n = 0; n < points_temp.size(); n++){
		if (points_temp.at(n).x == i && points_temp.at(n).y == j){
			if (points_temp.at(n).radius > max_radius)
				max_radius = points_temp.at(n).radius;
			break;
		}
	}
	if (*(BYTE*)image->GetPixelAddress(i-1, j) & CENTERED){
		calCenterArea(image, points_temp,i - 1, j);
	}
	if (*(BYTE*)image->GetPixelAddress(i+1, j) & CENTERED){
		calCenterArea(image,points_temp,i + 1, j);
	}
	if (*(BYTE*)image->GetPixelAddress(i, j-1) & CENTERED){
		calCenterArea(image, points_temp,i, j - 1);
	}
	if (*(BYTE*)image->GetPixelAddress(i, j+1) & CENTERED){
		calCenterArea(image, points_temp,i, j + 1);
	}
	if (*(BYTE*)image->GetPixelAddress(i+1, j+1) & CENTERED){
		calCenterArea(image,points_temp,i + 1, j + 1);
	}
	if (*(BYTE*)image->GetPixelAddress(i-1, j+1) & CENTERED){
		calCenterArea(image,points_temp,i - 1, j + 1);
	}
	if (*(BYTE*)image->GetPixelAddress(i+1, j-1) & CENTERED){
		calCenterArea(image,points_temp,i + 1, j - 1);
	}
	if (*(BYTE*)image->GetPixelAddress(i-1, j-1) & CENTERED){
		calCenterArea(image,points_temp,i - 1, j - 1);
	}
}



void ImgProcesor::removeIncludedCircles(std::vector<CenterPoint>& centerPoints, CDC* pdc, CPen* Bluepen1){
	int r;
	// ȥ����������Բ
	//Blue ���- delete
	bool bdelete = false;
	pdc->SelectObject(Bluepen1);
	for (unsigned int i = 0; i < centerPoints.size(); i++){
		CenterPoint pt;
		int x0 = centerPoints.at(i).x;
		int y0 = centerPoints.at(i).y;
		int r0 = centerPoints.at(i).radius;
		for (unsigned int j = i + 1; j < centerPoints.size() - 1; j++)
		{
			int x = centerPoints.at(j).x;
			int y = centerPoints.at(j).y;
			r = centerPoints.at(j).radius;
			if (DISTANCE(x0, y0, x, y) < abs(r0 - r) + 1) // ����
			{
				if (r0 > r) // ȥ��r0
					pt = centerPoints.at(i);
				else
					pt = centerPoints.at(j);
				Arc(pdc->m_hDC,
					pt.x - pt.radius,
					pt.y - pt.radius,
					pt.x + pt.radius,
					pt.y + pt.radius,
					pt.x + pt.radius,
					pt.y,
					pt.x + pt.radius,
					pt.y
				);
				if (r0 > r){ // ȥ��r0
					centerPoints.erase(centerPoints.begin() + i);//(&m_vCenterPoints.at(i));
					i--;
				}
				else
					centerPoints.erase(centerPoints.begin() + j);//(&m_vCenterPoints.at(j));
			}
		}
	}
}


void ImgProcesor::removePoentialErrors(const CImage*image,std::vector<CenterPoint>& centerPoints, CDC* pdc,CPen*	Redpen1) {
	std::vector<CenterPoint> tocheck;
	if(pdc!=nullptr)
		pdc->SelectObject(Redpen1);
	for (unsigned int i = 0; i < centerPoints.size(); i++)
	{	//baord area process
		CenterPoint centerp;
		centerp = centerPoints.at(i);
		if (centerp.x - centerp.radius < 0)
			centerp.radius -= (centerp.x - centerp.radius);
		if (centerp.y - centerp.radius < 0)
			centerp.radius -= (centerp.y - centerp.radius);
		if (centerp.x + centerp.radius > image->GetWidth() - 1)
			centerp.radius += (centerp.x + centerp.radius - image->GetWidth());
		if (centerp.y + centerp.radius > image->GetHeight() - 1)
			centerp.radius += (centerp.y + centerp.radius - image->GetHeight());
		if (centerPoints.at(i).radius < 8){ // need adjust <
			if (pdc != nullptr) {
				Arc(pdc->m_hDC,
					centerp.x - centerp.radius,
					centerp.y - centerp.radius,
					centerp.x + centerp.radius,
					centerp.y + centerp.radius,
					centerp.x + centerp.radius,
					centerp.y,
					centerp.x + centerp.radius,
					centerp.y
				);
			}
			centerPoints.erase(centerPoints.begin() + i);//(&m_vCenterPoints.at(i));
			i--;
		}
	}
}

void ImgProcesor::removePotentialErrorsIntersection(const CImage*image,std::vector<CenterPoint>& centerPoints, CDC* pdc, CPen* Bluepen1){

	pdc->SelectObject(Bluepen1);
	std::vector<CenterPoint> tocheck;
	// ȥ��Ǳ�ڵĴ���(ͬ����Բ�ཻ,���Ҳ��ཻ�Ĳ���������)
	for (int i = 0; i < centerPoints.size(); i++){
		tocheck.clear();
		int x0 = centerPoints.at(i).x;
		int y0 = centerPoints.at(i).y;
		int r0 = centerPoints.at(i).radius;
		for (int j = 0; j < centerPoints.size(); j++){
			if (i == j)
				continue;
			int x = centerPoints.at(j).x;
			int y = centerPoints.at(j).y;
			int r = centerPoints.at(j).radius;
			if (DISTANCE(x0, y0, x, y) < abs(r0 + r)){ // �ཻ
				tocheck.push_back({x,y,r});
			}
		}
		size_t size = tocheck.size();
		if (size > 0){ // ͬ�������ϵ�Բ�ཻ
			int total = 0;
			for (int tx = x0 - r0; tx < x0 + r0; tx++)
				for (int ty = y0 - r0; ty < y0 + r0; ty++)
				{
					if (DISTANCE(x0, y0, tx, ty) < r0){ // ����Բ�ڲ��ĵ�
						if (tx<0 || tx>=image->GetWidth()|| ty<0 || ty>=image->GetHeight())
							continue;
						bool isok = true;
						for (auto ptIter = tocheck.begin(); ptIter != tocheck.end(); ptIter++) {
							if (DISTANCE(tx, ty, ptIter->x, ptIter->y) < ptIter->radius) {
								isok = false;
								break;
							}
						}
						total += isok ? 1 : 0;// ͬ���е�Բ�����ཻ�Ĳ���
					}
				}
			if (total < 3.14159 * r0 * r0 * 0.5){ // need adjust 50%
				auto p = centerPoints.at(i);
				Arc(pdc->m_hDC,
					p.x - p.radius,p.y - p.radius,p.x + p.radius,p.y + p.radius,p.x + p.radius,p.y,p.x + p.radius,p.y
				);
				centerPoints.erase(centerPoints.begin() + i);//(&m_vCenterPoints.at(i));
				i--;
			}
		}
	}
}


