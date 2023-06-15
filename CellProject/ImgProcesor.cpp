#include "pch.h"
#include "ImgProcesor.h"
#include <math.h>

HSI::HSI(COLORREF pixel) {
	Rgb rgbval(pixel);
	HSI::HSI(rgbval);
}

HSI::HSI(const Rgb& rgb) {
	double R = ((double)rgb.r) / 255.0;
	double G = ((double)rgb.g) / 255.0;
	double B = ((double)rgb.b) / 255.0;
	double sum = R + G + B;
	int minV = min(rgb.r, rgb.g);
	minV = min(minV, rgb.b);
	this->I = ((double)(rgb.r + rgb.g + rgb.b)) / 255.0 / 3;
	this->S = 1 - 3.0 / (rgb.r + rgb.g + rgb.b) * minV;
	this->H = 180.0 / 3.14159 * acos(((double)(rgb.r - rgb.g) + (rgb.r - rgb.b)) / 2 / sqrt((double)(rgb.r - rgb.g) * (rgb.r - rgb.g) + (rgb.r - rgb.b) * (rgb.g - rgb.b)));
	if (rgb.g < rgb.b)this->H = 360 - this->H;
}


bool ImgProcesor::copyImage(const CImage& srcImage, CImage& destImage)
{
	int i, j;//ѭ������
	if (srcImage.IsNull())
		return FALSE;
	//Դͼ�����
	BYTE* srcPtr = (BYTE*)srcImage.GetBits();
	int srcBitsCount = srcImage.GetBPP();
	int srcWidth = srcImage.GetWidth();
	int srcHeight = srcImage.GetHeight();
	int srcPitch = srcImage.GetPitch();
	//����ԭ��ͼ��
	if (!destImage.IsNull())
	{
		destImage.Destroy();
	}
	//������ͼ��
	if (srcBitsCount == 32)   //֧��alphaͨ��
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else
	{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//���ص�ɫ��
	if (srcBitsCount <= 8 && srcImage.IsIndexed())//��Ҫ��ɫ��
	{
		RGBQUAD pal[256];
		int nColors = srcImage.GetMaxColorTableEntries();
		if (nColors > 0)
		{
			srcImage.GetColorTable(0, nColors, pal);
			destImage.SetColorTable(0, nColors, pal);//���Ƶ�ɫ�����
		}
	}
	//Ŀ��ͼ�����
	BYTE* destPtr = (BYTE*)destImage.GetBits();
	int destPitch = destImage.GetPitch();
	//����ͼ������
	for (i = 0; i < srcHeight; i++)
	{
		memcpy(destPtr + i * destPitch, srcPtr + i * srcPitch, abs(srcPitch));
	}
	return TRUE;
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
						*(pByte + 2) = 128;
					}
				}
			}
		}
	}
}

void ImgProcesor::getEdgeInfomation(CImage* image,const CImage*originImage) {
	const int edgeDoor = 45;
	CImage oriImg;
	copyImage(*originImage, oriImg);
	//get edge information
	for (int i = 0 + 1; i < image->GetHeight() - 1; i++) {//boarder no edge
		for (int j = 0 + 1; j < image->GetWidth() - 1; j++) {
			BYTE* lpDst = (BYTE*)image->GetPixelAddress(j, i);
			if (*(lpDst) == 0 || *(lpDst) == 255) {//Mark /Maybe Mark
				double pixel[9];
				BYTE* lpSrc = (BYTE*)oriImg.GetPixelAddress(j, i);
				for (int m = -1; m < 2; m++)                    //3*3����
					for (int n = -1; n < 2; n++) {
						unsigned char* lpSrc1 = lpSrc - image->GetBPP()/8*image->GetWidth() * m + 3 * n;
						pixel[(m + 1) * 3 + n + 1] = ((int)*lpSrc1 + *(lpSrc1 + 1) + *(lpSrc1 + 2)) / 3;      //���޸�
					}
				//Sobel
				double tmp1 =
					pixel[0] + 2 * pixel[1] + pixel[2] - pixel[6] - 2 * pixel[7] - pixel[8];
				double tmp2 =
					pixel[0] + 2 * pixel[3] + pixel[6] - pixel[2] - 2 * pixel[5] - pixel[8];
				double edge = sqrt(tmp1 * tmp1 + tmp2 * tmp2);
				if (edge > edgeDoor) {
					*(lpDst + 1) = 255;//edge
				}
			}
		}
	}

}



void ImgProcesor::markCell(CImage* image) {
	int height = image->GetHeight();
	int width = image->GetWidth();
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			BYTE* pByte = (BYTE*)image->GetPixelAddress(i, j);
			Rgb rgb(pByte[2], pByte[1], pByte[0]);
			//����HSI
			HSI hsi(rgb);
			double meanH = 210.0 * 360 / 255;
			double meanS = 55.0 / 255;
			double markDoor = 0.09;
			double maybeMarkDoor = 0.15;
			double x1 = hsi.H;
			double x2 = meanH;
			if (x1 < 90)x1 += 360;
			double y1 = hsi.S;
			double y2 = meanS;
			x1 /= 360; x2 /= 360;
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