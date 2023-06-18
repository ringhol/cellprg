#pragma once
#include "afxdialogex.h"


// StatisticsDlg 对话框

class StatisticsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(StatisticsDlg)

private:
	CImage* image;
	int H[256];
	int S[256];
	int I[256];
public:
	StatisticsDlg(CWnd* pParent = nullptr);   // 标准构造函数
	void SetImage(CImage* image);
	virtual ~StatisticsDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
