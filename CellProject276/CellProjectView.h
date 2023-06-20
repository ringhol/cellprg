
// CellProjectView.h: CCellProjectView 类的接口
//

#pragma once
#include <vector>
#include "ImgProcesor.h"
class CCellProjectView : public CView
{
private:
	CImage *image;
	CImage* backup;//<原图的一份拷贝
	std::vector<CenterPoint> points;//<去除所有潜在错误后的中心点

	bool is_draging{ false };//<当前鼠标是否处于滑动状态
	CPoint last_point;
	CPoint start_point;
	CPoint end_point;
	bool have_read_document{ false };
	int step{ 0 };


	CPen*	Redpen;
	CPen* Greenpen;
	CPen* Bluepen1;
	CPen* Redpen1;
	CPen* Emptypen;

protected: // 仅从序列化创建
	CCellProjectView() noexcept;
	DECLARE_DYNCREATE(CCellProjectView)

// 特性
public:
	CCellProjectDoc* GetDocument() const;

// 操作
public:
	void PrepareProcessing();
	void StepGroup(CCmdUI* pCmdUI, int step);
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
	afx_msg void OnFillHole();
	afx_msg void OnUpdateCellDetectUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMaybeMark2MarkUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGetEdgeInfomationUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTwoValueUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFillHoleUI(CCmdUI* pCmdUI);
	afx_msg void OnShrink();
	afx_msg void OnUpdateShrinkUI(CCmdUI* pCmdUI);
	afx_msg void OnCalCenterPoints();
	afx_msg void OnCalCenterWithAverage();
	afx_msg void OnCalCenterPointsWithAverageSimilar();
	afx_msg void OnUpdateCalCenterPointsUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCenterWithAverageUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCalCenterPointsWithAverageSimilarUI(CCmdUI* pCmdUI);
	afx_msg void OnRemoveIncludedCircles();
	afx_msg void OnRemovePotentialErrors();
	afx_msg void OnRemovePotentialErrorsIntersection();
	afx_msg void OnUpdateRemoveIncludedCirclesUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemovePotentialErrorsUI(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemovePotentialErrorsIntersectionUI(CCmdUI* pCmdUI);
	afx_msg void OnRemoveAllPotentialErrors();
	afx_msg void OnUpdateRemoveAllPotentialErrorsUI(CCmdUI* pCmdUI);
	afx_msg void OnCountAll();
	afx_msg void OnUpdateCountAllUI(CCmdUI* pCmdUI);
	afx_msg void OnAllSteps();
};

#ifndef _DEBUG  // CellProjectView.cpp 中的调试版本
inline CCellProjectDoc* CCellProjectView::GetDocument() const
   { return reinterpret_cast<CCellProjectDoc*>(m_pDocument); }
#endif

