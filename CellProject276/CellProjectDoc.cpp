﻿
// CellProjectDoc.cpp: CCellProjectDoc 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CellProject.h"
#endif

#include "CellProjectDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCellProjectDoc

IMPLEMENT_DYNCREATE(CCellProjectDoc, CDocument)

BEGIN_MESSAGE_MAP(CCellProjectDoc, CDocument)
END_MESSAGE_MAP()


// CCellProjectDoc 构造/析构

CCellProjectDoc::CCellProjectDoc() noexcept
{
	// TODO: 在此添加一次性构造代码
	image = new CImage;
	CString filename;
	WCHAR strDirName[80];
	GetCurrentDirectory(80, (LPWSTR)strDirName);
	CString pathname;
	pathname.Format(L"%s", strDirName);
	filename = pathname + L"\\data.bmp";
	image->Load(filename);
}

CCellProjectDoc::~CCellProjectDoc()
{
	if (image != nullptr) {
		delete image;
		image = nullptr;
	}
}

BOOL CCellProjectDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)
	SetTitle(L"Cellprg276.bmp");
	return TRUE;
}




// CCellProjectDoc 序列化

void CCellProjectDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		IStream* pStream = NULL;
		if (SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &pStream)))
		{
			// 将图像保存到流中
			image->Save(pStream, Gdiplus::ImageFormatBMP);
			// 将流数据写入存档
			HGLOBAL hGlobal = NULL;
			GetHGlobalFromStream(pStream, &hGlobal);
			if (hGlobal != NULL)
			{
				void* pData = GlobalLock(hGlobal);
				if (pData != NULL)
				{
					DWORD dwSize = GlobalSize(hGlobal);
					ar.Write(pData, dwSize);
					GlobalUnlock(hGlobal);
				}
			}
			// 释放资源
			pStream->Release();
		}
	}
	else
	{
		
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CCellProjectDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 搜索处理程序的支持
void CCellProjectDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
	SetSearchContent(strSearchContent);
}

void CCellProjectDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CCellProjectDoc 诊断

#ifdef _DEBUG
void CCellProjectDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCellProjectDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCellProjectDoc 命令
