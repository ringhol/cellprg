
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


// CCellProjectView

IMPLEMENT_DYNCREATE(CCellProjectView, CView)

BEGIN_MESSAGE_MAP(CCellProjectView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_FILE_OPEN, &CCellProjectView::OnFileOpen)
END_MESSAGE_MAP()

// CCellProjectView 构造/析构

CCellProjectView::CCellProjectView() noexcept
{
	// TODO: 在此处添加构造代码

}

CCellProjectView::~CCellProjectView()
{
}

BOOL CCellProjectView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCellProjectView 绘图

void CCellProjectView::OnDraw(CDC* pDC)
{
	CCellProjectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	if (!image.IsNull())
	{
		image.Draw(pDC->GetSafeHdc(), 0, 0);
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
			if (!image.IsNull())
			{
				image.Destroy();
			}
			image.Load(dlg.GetPathName());
			Invalidate();
		}
}
