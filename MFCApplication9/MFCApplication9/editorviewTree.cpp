// editorviewTree.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication9.h"
#include "editorviewTree.h"


// CEditorViewTree
CEditorViewTree::CEditorViewTree()
{
}

CEditorViewTree::~CEditorViewTree()
{
}

BEGIN_MESSAGE_MAP(CEditorViewTree, CTreeView)
END_MESSAGE_MAP()


BOOL CEditorViewTree::CreateEx(DWORD dwExStyle, LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, LPVOID lpParam)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CTreeView::CreateEx(dwExStyle, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, lpParam);
}
