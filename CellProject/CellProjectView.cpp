
// CellProjectView.cpp: CCellProjectView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CellProject.h"
#endif

#include "CellProjectDoc.h"
#include "CellProjectView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "ImgProcesor.h"
#include "MainFrm.h"
// CCellProjectView

IMPLEMENT_DYNCREATE(CCellProjectView, CView)

BEGIN_MESSAGE_MAP(CCellProjectView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CCellProjectView::OnFileOpen)
	ON_COMMAND(ID_32771, &CCellProjectView::OnCellDetect)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_32772, &CCellProjectView::OnRecoverImage)
	ON_COMMAND(ID_32773, &CCellProjectView::OnMaybeMark2Mark)
	ON_COMMAND(ID_32774, &CCellProjectView::OnGetEdgeInfomation)
END_MESSAGE_MAP()

// CCellProjectView 构造/析构

CCellProjectView::CCellProjectView() noexcept
{
	// TODO: 在此处添加构造代码
	//程序运行时默认自动打开名为data.bmp的图片
	image = new CImage;
	backup = new CImage;
	CString filename;
	WCHAR strDirName[80];
	GetCurrentDirectory(80, (LPWSTR)strDirName);
	CString pathname;
	pathname.Format(L"%s", strDirName);
	filename = pathname + L"\\data.bmp";
	image->Load(filename);
	ImgProcesor::copyImage(*image, *backup);
}

CCellProjectView::~CCellProjectView()
{
	if (this->image != nullptr) {
		delete image;
		image = nullptr;
	}
	if (backup != nullptr) {
		delete backup;
		backup = nullptr;
	}
}

BOOL CCellProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCellProjectView 绘图

void CCellProjectView::PrepareProcessing()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->m_wndStatusBar.SetPaneText(0, L"处理中", TRUE);
}

void CCellProjectView::OnDraw(CDC* pDC)
{
	CCellProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	//显示图片
	if (image!=nullptr&&!image->IsNull())
	{
		image->Draw(pDC->GetSafeHdc(), 0, 0);
	}
	//获取点击点的坐标以及rgb信息
	if (nowPoint.x > 0 && nowPoint.y > 0) {
		CString str;
		COLORREF pixel = GetPixel(::GetDC(this->m_hWnd),nowPoint.x,nowPoint.y);
		Rgb rgb(pixel);
		HSI hsi(rgb);
		str.Format(L"%d,%d;rgb(%d,%d,%d);HSI(%.3f,%.3f,%.3f)", nowPoint.x, nowPoint.y,rgb.r,rgb.g,rgb.b,hsi.H,hsi.S,hsi.I);
		pDC->TextOutW(nowPoint.x,nowPoint.y, str);
	}
}


// CCellProjectView 打印

BOOL CCellProjectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCellProjectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCellProjectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CCellProjectView 诊断

#ifdef _DEBUG
void CCellProjectView::AssertValid() const
{
	CView::AssertValid();
}

void CCellProjectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCellProjectDoc* CCellProjectView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCellProjectDoc)));
	return (CCellProjectDoc*)m_pDocument;
}
#endif //_DEBUG


// CCellProjectView 消息处理程序


void CCellProjectView::OnFileOpen()
{
		// TODO: 在此添加命令处理程序代码
		CFileDialog dlg(TRUE, _T(".bmp"), _T("*.bmp"), OFN_HIDEREADONLY |
			OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|JPEG文件(*.jpg)|*.jpg||"));
		if (dlg.DoModal() == IDOK)
		{
			if (image!=nullptr&&!image->IsNull())
			{
				image->Destroy();
				delete image;
				image = nullptr;
			}
			image = new CImage;
			image->Load(dlg.GetPathName());
			if (backup != nullptr) {
				delete backup;
				backup = nullptr;
			}
			backup = new CImage;
			ImgProcesor::copyImage(*image,*backup);
			Invalidate();
		}
}


void CCellProjectView::OnCellDetect()
{
	// TODO: 在此添加命令处理程序代码
	PrepareProcessing();
	ImgProcesor::markCell(image);
	Invalidate();
}

void CCellProjectView::OnLButtonDown(UINT nFlags, CPoint point) {
	nowPoint = point;
	Invalidate();
	CView::OnLButtonDown(nFlags, point);
}


void CCellProjectView::OnMouseMove(UINT nFlags, CPoint point)
{
	 //TODO: 在此添加消息处理程序代码和/或调用默认值
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	HDC hdc = ::GetDC(this->m_hWnd);
	COLORREF Windows_Hdc_RGB = GetPixel(hdc, point.x, point.y);	//获取指定DC上的像素点RGB值
	Rgb rgb(Windows_Hdc_RGB);
	HSI hsi(rgb);

	CString str;
	str.Format(L"就绪 %d,%d RGB:(%d,%d,%d) HSI:( %.3f,%.3f,%.3f)",point.x,point.y,rgb.r,rgb.g,rgb.b ,hsi.H, hsi.S,hsi.I);
	pMainFrame->m_wndStatusBar.SetPaneText(0, str, TRUE);
}



void CCellProjectView::OnRecoverImage()
{
	// TODO: 在此添加命令处理程序代码
	PrepareProcessing();
	delete image;
	this->image = new CImage;
	ImgProcesor::copyImage(*backup, *image);
	Invalidate();
}


void CCellProjectView::OnMaybeMark2Mark()
{
	// TODO: 在此添加命令处理程序代码
	PrepareProcessing();
	ImgProcesor::maybemark2mark(image);
	Invalidate();
}


void CCellProjectView::OnGetEdgeInfomation()
{
	// TODO: 在此添加命令处理程序代码
	PrepareProcessing();
	ImgProcesor::getEdgeInfomation(image, backup);
	Invalidate();
}
