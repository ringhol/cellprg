
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
#include "StatisticsDlg.h"
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
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_32772, &CCellProjectView::OnRecoverImage)
	ON_COMMAND(ID_32773, &CCellProjectView::OnMaybeMark2Mark)
	ON_COMMAND(ID_32774, &CCellProjectView::OnGetEdgeInfomation)
	ON_COMMAND(ID_32775, &CCellProjectView::OnTwoValue)
	ON_COMMAND(ID_32776, &CCellProjectView::OnFillHole)
	ON_UPDATE_COMMAND_UI(ID_32771, &CCellProjectView::OnUpdateCellDetectUI)
	ON_UPDATE_COMMAND_UI(ID_32773, &CCellProjectView::OnUpdateMaybeMark2MarkUI)
	ON_UPDATE_COMMAND_UI(ID_32774, &CCellProjectView::OnUpdateGetEdgeInfomationUI)
	ON_UPDATE_COMMAND_UI(ID_32775, &CCellProjectView::OnUpdateTwoValueUI)
	ON_UPDATE_COMMAND_UI(ID_32776, &CCellProjectView::OnUpdateFillHoleUI)
	ON_COMMAND(ID_32777, &CCellProjectView::OnShrink)
	ON_UPDATE_COMMAND_UI(ID_32777, &CCellProjectView::OnUpdateShrinkUI)
	ON_COMMAND(ID_32778, &CCellProjectView::OnCalCenterPoints)
	ON_COMMAND(ID_32779, &CCellProjectView::OnCalCenterWithAverage)
	ON_COMMAND(ID_32780, &CCellProjectView::OnCalCenterPointsWithAverageSimilar)
	ON_UPDATE_COMMAND_UI(ID_32778, &CCellProjectView::OnUpdateCalCenterPointsUI)
	ON_UPDATE_COMMAND_UI(ID_32779, &CCellProjectView::OnUpdateCenterWithAverageUI)
	ON_UPDATE_COMMAND_UI(ID_32780, &CCellProjectView::OnUpdateCalCenterPointsWithAverageSimilarUI)
	ON_COMMAND(ID_32781, &CCellProjectView::OnRemoveIncludedCircles)
	ON_COMMAND(ID_32782, &CCellProjectView::OnRemovePotentialErrors)
	ON_COMMAND(ID_32783, &CCellProjectView::OnRemovePotentialErrorsIntersection)
	ON_UPDATE_COMMAND_UI(ID_32781, &CCellProjectView::OnUpdateRemoveIncludedCirclesUI)
	ON_UPDATE_COMMAND_UI(ID_32782, &CCellProjectView::OnUpdateRemovePotentialErrorsUI)
	ON_UPDATE_COMMAND_UI(ID_32783, &CCellProjectView::OnUpdateRemovePotentialErrorsIntersectionUI)
	ON_COMMAND(ID_32787, &CCellProjectView::OnRemoveAllPotentialErrors)
	ON_UPDATE_COMMAND_UI(ID_32787, &CCellProjectView::OnUpdateRemoveAllPotentialErrorsUI)
	ON_COMMAND(ID_32785, &CCellProjectView::OnCountAll)
	ON_UPDATE_COMMAND_UI(ID_32785, &CCellProjectView::OnUpdateCountAllUI)
END_MESSAGE_MAP()

// CCellProjectView 构造/析构

CCellProjectView::CCellProjectView() noexcept
{
	// TODO: 在此处添加构造代码
	//程序运行时默认自动打开名为data.bmp的图片
	backup = new CImage;
	image = nullptr;
	Redpen = new CPen;
	Greenpen = new CPen;
	Bluepen1 = new CPen;
	Redpen1 = new CPen;
	Emptypen = new CPen;
	Redpen->CreatePen(PS_DOT, 1, RGB(255, 0, 0));
	Redpen1->CreatePen(PS_DOT, 3, RGB(255, 0, 0));
	Greenpen->CreatePen(PS_DOT, 1, RGB(0, 255, 0));
	Bluepen1->CreatePen(PS_DOT, 3, RGB(0, 0, 255));
}

CCellProjectView::~CCellProjectView()
{
	if (backup != nullptr) {
		delete backup;
		backup = nullptr;
	}
	image = nullptr;
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

void CCellProjectView::StepGroup(CCmdUI* pCmdUI, int tstep)
{
	if (step == tstep)pCmdUI->Enable();
	else pCmdUI->Enable(false);
}

void CCellProjectView::OnDraw(CDC* pDC)
{
	CCellProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (!have_read_document) {
		image = GetDocument()->image;
		end_point.x = image->GetWidth();
		end_point.y = image->GetHeight();
		start_point.x = 0; start_point.y = 0;
		ImgProcesor::copyImage(*image, *backup);
		have_read_document = true;



	}
	//显示图片
	if (image!=nullptr&&!image->IsNull()){
		image->Draw(pDC->GetSafeHdc(), 0, 0);

	}

	auto prorect = CRect(start_point, end_point);
	CPen pen;
	pen.CreatePen(PS_DOT,1, RGB(255, 0, 0));
	pDC->SelectObject(pen);
	pDC->SelectObject(GetStockObject(NULL_BRUSH));
	pDC->Rectangle(prorect);
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
	CCellProjectDoc* pDoc = GetDocument();
		CFileDialog dlg(TRUE, _T(".bmp"), _T("*.bmp"), OFN_HIDEREADONLY |
			OFN_OVERWRITEPROMPT, _T("位图文件(*.bmp)|*.bmp|JPEG文件(*.jpg)|*.jpg||"));
		if (dlg.DoModal() == IDOK)
		{
			if (pDoc->image !=nullptr&&!pDoc->image->IsNull())
			{
				pDoc->image->Destroy();
				delete pDoc->image;
				pDoc->image = nullptr;
			}
			pDoc->image = new CImage;
			pDoc->image->Load(dlg.GetPathName());
			if (backup != nullptr) {
				delete backup;
				backup = nullptr;
			}
			image = pDoc->image;
			backup = new CImage;
			ImgProcesor::copyImage(*image,*backup);
			Invalidate(true);
		}
}




void CCellProjectView::OnLButtonDown(UINT nFlags, CPoint point) {
	if (!is_draging) {
		last_point = point;
		is_draging = true;
	}
	CView::OnLButtonDown(nFlags, point);
}

void CCellProjectView::OnLButtonUp(UINT nFlags, CPoint point) {
	is_draging = false;
	start_point.x = min(last_point.x, point.x);
	start_point.y = min(last_point.y, point.y);
	start_point.x = min(start_point.x, image->GetWidth());
	start_point.y = min(start_point.y, image->GetHeight());
	end_point.x = max(last_point.x, point.x);
	end_point.y = max(last_point.y, point.y);
	end_point.x = min(end_point.x, image->GetWidth());
	end_point.y = min(end_point.y, image->GetHeight());
	if (start_point.x == end_point.x && start_point.y == end_point.y) {
		start_point.y = 0;
		start_point.x = 0;
		end_point.x = image->GetWidth();
		end_point.y = image->GetHeight();
	}
	Invalidate(true);
	CView::OnLButtonUp(nFlags, point);
}

void CCellProjectView::OnMouseMove(UINT nFlags, CPoint point)
{
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
	PrepareProcessing();
	CCellProjectDoc* pDoc = GetDocument();
	if (pDoc->image != nullptr) {
		delete pDoc->image;
	}
	pDoc->image = new CImage;
	ImgProcesor::copyImage(*backup, *pDoc->image);
	image = pDoc->image;
	Invalidate(true);
	step = 0;
}

void CCellProjectView::OnCellDetect()
{
	PrepareProcessing();
	ImgProcesor::markCell(image, start_point, end_point);
	Invalidate(true);
	step = 1;
}

void CCellProjectView::OnMaybeMark2Mark(){
	PrepareProcessing();
	ImgProcesor::maybemark2mark(image);
	Invalidate(true);
	step = 2;
}


void CCellProjectView::OnGetEdgeInfomation()
{
	PrepareProcessing();
	ImgProcesor::getEdgeInfomation(image, backup);
	Invalidate(true);
	step = 3;
}


void CCellProjectView::OnTwoValue()
{
	PrepareProcessing();
	ImgProcesor::twovalue(&image);
	GetDocument()->image = image;
	Invalidate(true);
	step = 4;
}

void CCellProjectView::OnFillHole()
{
	PrepareProcessing();
	ImgProcesor::fillHole(image);
	Invalidate();
	step = 5;
}


void CCellProjectView::OnShrink()
{
	PrepareProcessing();
	ImgProcesor::shrink(image);
	Invalidate();
	step = 6;
}

void CCellProjectView::OnCalCenterPoints() {
	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenter(image);
	ImgProcesor::copyImage(bkp, *image);
	CString msg;
	msg.Format(L"获得的中心点数目= %d", points.size());
	MessageBox(msg);
}


void CCellProjectView::OnCalCenterWithAverage()
{
	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image);
	ImgProcesor::copyImage(bkp, *image);
	CString msg;
	msg.Format(L"取平均值，获得的中心点数目= %d", points.size());
	MessageBox(msg);
}



void CCellProjectView::OnCalCenterPointsWithAverageSimilar()
{
	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image,GetDC(),Redpen,Greenpen);
	ImgProcesor::copyImage(bkp, *image);
	CString msg;
	msg.Format(L"平均化相近的中心点后数目= %d", points.size());
	MessageBox(msg);
}

void CCellProjectView::OnRemoveIncludedCircles()
{
	CCellProjectView::OnDraw(GetDC());
	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image, GetDC(),Redpen,Greenpen);
	ImgProcesor::copyImage(bkp, *image);
	int size1 = points.size();
	ImgProcesor::removeIncludedCircles(points, GetDC(),Bluepen1);
	int size2 = points.size();
	CString msg;
	if (size2 != size1) {
		msg.Format(L"去掉被包含的圆(Blue )后数目= %d", size2);
	}
	else {
		msg = L"没有被包含的圆（Blue)";
	}
	MessageBox(msg);
}


void CCellProjectView::OnRemovePotentialErrors()
{
	CCellProjectView::OnDraw(GetDC());

	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image, GetDC(), Redpen, Greenpen);
	int size1 = points.size();
	ImgProcesor::removePoentialErrors(image,points, GetDC(),Redpen1);

	ImgProcesor::copyImage(bkp, *image);
	int size2 = points.size();
	CString msg;
	if (size2 != size1) {
		msg.Format(L"去掉潜在的错误(圆 r<8 Red)后数目=%d", size2);
	}
	else {
		msg = L"没有潜在的错误(圆 r<8 Red)";
	}
	MessageBox(msg);
}

void CCellProjectView::OnRemovePotentialErrorsIntersection()
{
	CCellProjectView::OnDraw(GetDC());

	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image, GetDC(), Redpen, Greenpen);
	//ImgProcesor::removePoentialErrors(image, points);
	int size1 = points.size();
	ImgProcesor::removePotentialErrorsIntersection(image, points, GetDC(),Bluepen1);
	ImgProcesor::copyImage(bkp, *image);
	int size2 = points.size();
	CString msg;
	if (size2 != size1) {
		msg.Format(L"去掉潜在的错误(同两个圆相交,并且不相交的部分是噪声)Bluepen后数目=%d", size2);
	}
	else {
		msg = L"没有相交的潜在错误";
	}
	MessageBox(msg);
}


void CCellProjectView::OnRemoveAllPotentialErrors()
{
	CCellProjectView::OnDraw(GetDC());

	CImage bkp;
	ImgProcesor::copyImage(*image, bkp);
	auto points = ImgProcesor::calCenterWithAverage(image, GetDC(), Redpen, Greenpen);
	int size1 = points.size();
	ImgProcesor::removeIncludedCircles(points, GetDC(), Bluepen1);

	ImgProcesor::removePoentialErrors(image, points, GetDC(), Redpen1);

	ImgProcesor::removePotentialErrorsIntersection(image, points, GetDC(), Bluepen1);

	ImgProcesor::copyImage(bkp, *image);

	int size2 = points.size();
	this->points.swap(points);
	CString msg;
	if (size2 != size1) {
		msg.Format(L"去掉所有潜在的错误后数目=%d", size2);
	}
	else {
		msg = L"没有潜在错误";
	}
	MessageBox(msg);
}


void CCellProjectView::OnCountAll()
{
	if (points.size() == 0) {
		MessageBox(L"请先去除所有潜在的错误");
		return;
	}
	//复位图像
	ImgProcesor::copyImage(*backup, *image);
	CCellProjectView::OnDraw(GetDC());
	//绘制细胞
	CDC* pdc = GetDC();
	pdc->SelectObject(Redpen);
	double averageRadius = 0.0;
	double averageS = 0.0;
	for (auto p : this->points) {
		averageRadius += (double)p.radius;
		averageS += 3.14159 * pow(p.radius, 2);
		Arc(pdc->GetSafeHdc(),
			p.x - p.radius,p.y - p.radius,p.x + p.radius,p.y + p.radius,p.x + p.radius,p.y,p.x + p.radius,p.y
		);
	}
	averageRadius /= points.size();
	averageS /= points.size();
	CString msg;
	msg.Format(L"共有%d个细胞,平均半径%.3f,平均面积%.3f", this->points.size(),averageRadius,averageS);
	MessageBox(msg);
	StatisticsDlg* dlg = new StatisticsDlg();
	dlg->Create(IDD_DIALOG1, this);
	dlg->ShowWindow(SW_SHOW);
	dlg->SetImage(image);

}



void CCellProjectView::OnUpdateCellDetectUI(CCmdUI* pCmdUI){
	StepGroup(pCmdUI, 0);
}


void CCellProjectView::OnUpdateMaybeMark2MarkUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 1);
}


void CCellProjectView::OnUpdateGetEdgeInfomationUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 2);
}


void CCellProjectView::OnUpdateTwoValueUI(CCmdUI* pCmdUI){
	StepGroup(pCmdUI, 3);
}


void CCellProjectView::OnUpdateFillHoleUI(CCmdUI* pCmdUI){
	StepGroup(pCmdUI, 4);
}


void CCellProjectView::OnUpdateShrinkUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 5);
}



void CCellProjectView::OnUpdateCalCenterPointsUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}



void CCellProjectView::OnUpdateCenterWithAverageUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}


void CCellProjectView::OnUpdateCalCenterPointsWithAverageSimilarUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}



void CCellProjectView::OnUpdateRemoveIncludedCirclesUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}



void CCellProjectView::OnUpdateRemovePotentialErrorsUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}



void CCellProjectView::OnUpdateRemovePotentialErrorsIntersectionUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}



void CCellProjectView::OnUpdateRemoveAllPotentialErrorsUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}




void CCellProjectView::OnUpdateCountAllUI(CCmdUI* pCmdUI)
{
	StepGroup(pCmdUI, 6);
}