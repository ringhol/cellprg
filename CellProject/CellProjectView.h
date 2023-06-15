
// CellProjectView.h: CCellProjectView 类的接口
//

#pragma once

class CCellProjectView : public CView
{
private:
	CImage *image;
	CImage* backup;//<原图的一份拷贝
	bool is_draging;
	CPoint last_point;
	CPoint start_point;
	CPoint end_point;
protected: // 仅从序列化创建
	CCellProjectView() noexcept;
	DECLARE_DYNCREATE(CCellProjectView)

// 特性
public:
	CCellProjectDoc* GetDocument() const;

// 操作
public:
	void PrepareProcessing();
// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CCellProjectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpen();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCellDetect();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRecoverImage();
	afx_msg void OnMaybeMark2Mark();
	afx_msg void OnGetEdgeInfomation();
	afx_msg void OnTwoValue();
};

#ifndef _DEBUG  // CellProjectView.cpp 中的调试版本
inline CCellProjectDoc* CCellProjectView::GetDocument() const
   { return reinterpret_cast<CCellProjectDoc*>(m_pDocument); }
#endif

