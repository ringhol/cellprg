// StatisticsDlg.cpp: 实现文件
//

#include "pch.h"
#include "CellProject.h"
#include "afxdialogex.h"
#include "StatisticsDlg.h"
#include "ImgProcesor.h"
// StatisticsDlg 对话框

IMPLEMENT_DYNAMIC(StatisticsDlg, CDialogEx)

StatisticsDlg::StatisticsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

void StatisticsDlg::SetImage(CImage* image)
{
	if (image == nullptr) {
		return;
	}
	this->image = image;
	//绘制HSI直方图
	const UINT length = 256;
	memset(H, 0, sizeof(H));
	memset(S, 0, sizeof(S));
	memset(I, 0, sizeof(I));
	for (int i = 0; i < image->GetWidth(); i++) {
		for (int j = 0; j < image->GetHeight(); j++) {
			BYTE* pixel = (BYTE*)image->GetPixelAddress(i, j);
			Rgb rgb(pixel[2], pixel[1], pixel[0]);
			HSI hsi(rgb);
			// 将HSI值转换到0-255范围内
			int nH = 0, nS = 0, nI = 0;
			if (hsi.H == hsi.H) {
				nH = (int)(hsi.H / 360.0 * length);
				H[nH]++;
			}
			if (hsi.S == hsi.S) {
				nS = (int)(hsi.S * length);
				S[nS]++;
			}
			if (hsi.I == hsi.I) {
				nI = (int)(hsi.I * length);
				I[nI]++;
			}
		}
	}
	// 获取直方图最大值
	int maxH = 0;
	int maxS = 0;
	int maxI = 0;
	for (int a = 0; a < length; a++){
		maxH = max(H[a], maxH);
		maxS = max(S[a], maxS);
		maxI = max(I[a], maxI);
	}

	// 绘制直方图
	//绘制x轴
	CString xlabel[] = {L"0",L"50",L"100",L"150",L"200",L"250"};
	CDC* pDC = GetDC();
	pDC->SetViewportOrg(10, 400);
	for (int i = 0; i < 260; i+=50) {
		pDC->LineTo(i, 0);
		pDC->LineTo(i, -5);
		pDC->MoveTo(i, 0);
		pDC->TextOutW(i - xlabel[i/50].GetLength() * 3, 16, xlabel[i/50]);
	}
	pDC->LineTo(280, 0);
	//绘制y轴
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, -360);
	pDC->MoveTo(0, 0);
	int yGap = 120;
	CPen penH(PS_SOLID, 2, RGB(255, 0, 0));
	CPen penS(PS_SOLID, 2, RGB(0, 255, 0));
	CPen penI(PS_SOLID, 2, RGB(0, 0, 255));
	//绘制I
	CPen* pOldPen = pDC->SelectObject(&penI);
	for (int i = 0; i < length; i++) {
		int nCount = I[i];
		int nBarHeight = (int)(nCount * yGap / maxI);
		pDC->LineTo(i,  - nBarHeight);
		pDC->MoveTo(i, -nBarHeight);
	}
	pDC->SelectObject(pOldPen);
	//绘制S
	pOldPen = pDC->SelectObject(&penS);
	pDC->MoveTo(0,-yGap);
	for (int i = 0; i < length; i++) {
		int nCount = S[i];
		int nBarHeight = (int)(nCount * yGap / maxS);
		pDC->LineTo(i, -yGap  - nBarHeight);
		pDC->MoveTo(i, -yGap - nBarHeight);
	}
	pDC->SelectObject(pOldPen);

	//绘制H
	pOldPen = pDC->SelectObject(&penH);
	pDC->MoveTo(0, -2*yGap);
	for (int i = 0; i < length; i++) {
		int nCount = H[i];
		int nBarHeight = (int)(nCount * yGap / maxH);
		pDC->LineTo(i, -yGap*2-nBarHeight);
		pDC->MoveTo(i, -yGap * 2 - nBarHeight);
	}
	pDC->SelectObject(pOldPen);

}

StatisticsDlg::~StatisticsDlg()
{
}

void StatisticsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(StatisticsDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &StatisticsDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// StatisticsDlg 消息处理程序


void StatisticsDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}
