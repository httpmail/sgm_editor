#pragma once


// CEditorViewTree 视图

class CEditorViewTree : public CTreeView
{
public:
	CEditorViewTree();           // 动态创建所使用的受保护的构造函数
	virtual ~CEditorViewTree();

public:
	BOOL Create(CWnd *pParent);
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam = NULL);
};


