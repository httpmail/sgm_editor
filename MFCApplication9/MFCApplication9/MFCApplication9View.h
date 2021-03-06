
// MFCApplication9View.h : CMFCApplication9View 类的接口
//

#pragma once


class CMFCApplication9View : public CView
{
protected: // 仅从序列化创建
	CMFCApplication9View();
	DECLARE_DYNCREATE(CMFCApplication9View)

// 特性
public:
	CMFCApplication9Doc* GetDocument() const;

// 操作
public:

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
	virtual ~CMFCApplication9View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CButton m_Button;
// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

#ifndef _DEBUG  // MFCApplication9View.cpp 中的调试版本
inline CMFCApplication9Doc* CMFCApplication9View::GetDocument() const
   { return reinterpret_cast<CMFCApplication9Doc*>(m_pDocument); }
#endif

