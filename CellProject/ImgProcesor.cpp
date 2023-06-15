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


bool ImgProcesor::copyImage(const CImage& srcImage, CImage& destImage){
	int i;//循环变量
	if (srcImage.IsNull())
		return FALSE;
	//源图像参数
	BYTE* srcPtr = (BYTE*)srcImage.GetBits();
	int srcBitsCount = srcImage.GetBPP();
	int srcWidth = srcImage.GetWidth();
	int srcHeight = srcImage.GetHeight();
	int srcPitch = srcImage.GetPitch();
	//销毁原有图像
	if (!destImage.IsNull()){
		destImage.Destroy();
	}
	//创建新图像
	//支持alpha通道
	if (srcBitsCount == 32){
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 1);
	}
	else{
		destImage.Create(srcWidth, srcHeight, srcBitsCount, 0);
	}
	//加载调色板
	if (srcBitsCount <= 8 && srcImage.IsIndexed()){
		RGBQUAD pal[256];
		int nColors = srcImage.GetMaxColorTableEntries();
		if (nColors > 0){
			srcImage.GetColorTable(0, nColors, pal);
			destImage.SetColorTable(0, nColors, pal);//复制调色板程序
		}
	}
	//目标图像参数
	BYTE* destPtr = (BYTE*)destImage.GetBits();
	int destPitch = destImage.GetPitch();
	//复制图像数据
	for (i = 0; i < srcHeight; i++){
		memcpy(destPtr + i * destPitch, srcPtr + i * srcPitch, abs(srcPitch));
	}
	return TRUE;
}


void ImgProcesor::maybemark2mark(CImage* image) {
	bool MarkChg = true;//是否有需要改变的点
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

void ImgProcesor::getEdgeInfomation(CImage* image, const CImage* originImage) {
	const int edgeDoor = 45;
	CImage oriImg;
	copyImage(*originImage, oriImg);
	int lineBytes = image->GetBPP() / 8 * image->GetWidth();
	//get edge information
	for (int i = 0 + 1; i < image->GetHeight() - 1; i++) {//boarder no edge
		for (int j = 0 + 1; j < image->GetWidth() - 1; j++) {
			BYTE* lpDst = (BYTE*)image->GetPixelAddress(j, i);
			if (*(lpDst) == 0 || *(lpDst) == 255) {//Mark /Maybe Mark
				double pixel[9];
				BYTE* lpSrc = (BYTE*)oriImg.GetPixelAddress(j, i);
				for (int m = -1; m < 2; m++)                    //3*3矩阵
					for (int n = -1; n < 2; n++) {
						unsigned char* lpSrc1 = lpSrc - lineBytes * m + 3 * n;
						pixel[(m + 1) * 3 + n + 1] = ((int)*lpSrc1 + *(lpSrc1 + 1) + *(lpSrc1 + 2)) / 3;      //可修改
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

	//滤波
	const int M = 5;  //5*5窗口滤波
	bool bdelete;
	//filter 
	for (int i = 0 + M; i < image->GetHeight() - M; i++){
		// 针对每行图像每列进行操作
		for (int j = 0 + M; j < image->GetWidth() - M; j++){
			// 指向源DIB第i行，第j个象素的指针
			BYTE* lpDst = (BYTE*)image->GetPixelAddress(j, i);
			//edge 
			if (*(lpDst + 1) == 255){
				bdelete = true;
				for (int m = -M; m <= M; m++)
					for (int n = -M; n <= M; n++){
						if (m == -M || m == M || n == -M || n == M) {
							//noMark && no Edge
							if (*(lpDst + lineBytes * m + n * 3) || (*(lpDst + lineBytes * m + n * 3 + 1) == 255)){
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

void ImgProcesor::twovalue(CImage** image){
	// TODO: 在此添加命令处理程序代码
		// 暂时分配内存，以保存新图像
		CImage *newImage = new CImage;
		newImage->Create((*image)->GetWidth(), (*image)->GetHeight(), 8);
		// 在调色板中初始化256种颜色值
		RGBQUAD colors[256];
		for (int i = 0; i < 256; i++){
			colors[i].rgbBlue = i;
			colors[i].rgbGreen = i;
			colors[i].rgbRed = i;
			colors[i].rgbReserved = 0;
		}
		// 将调色板赋值给图像
		if (newImage->GetDC() != NULL){
			::SetDIBColorTable(newImage->GetDC(), 0, 256, colors);
			newImage->ReleaseDC();
		}
		for (int i = 0; i < (*image)->GetHeight(); i++) {
			for (int j = 0; j < (*image)->GetWidth(); j++) {
				BYTE* lpSrc = (BYTE*)(*image)->GetPixelAddress(j, i);
				BYTE* lpDst = (BYTE*)newImage->GetPixelAddress(j, i);
				BYTE v=0;
				//Mark
				if (*(lpSrc) == 0){
					v = 0x80;
					if (*(lpSrc + 1))v |= 0x70;//set edge
				}
				*lpDst = v;
			}
		}
		CImage* tmp = *image;
		*image = newImage;
		delete tmp;
		tmp = nullptr;
}

void ImgProcesor::markCell(CImage* image, CPoint start, CPoint end) {
	int height = image->GetHeight();
	int width = image->GetWidth();
	for (int i = start.x; i < end.x; i++) {
		for (int j = start.y; j < end.y; j++) {
			BYTE* pByte = (BYTE*)image->GetPixelAddress(i, j);
			Rgb rgb(pByte[2], pByte[1], pByte[0]);
			//计算HSI
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
				//标记红色
				pByte[2] = 255; pByte[1] = 0; pByte[0] = 0;
			}
			else if (dis < maybeMarkDoor) {
				//标记蓝色
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