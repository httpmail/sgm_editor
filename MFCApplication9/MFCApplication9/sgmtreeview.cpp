
#include "stdafx.h"
#include "sgmtreeview.h"


// CSGMTreeView

CSGMTreeView::CSGMTreeView()
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif
}

CSGMTreeView::~CSGMTreeView()
{
}

BEGIN_MESSAGE_MAP(CSGMTreeView, CTreeCtrl)
    ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CSGMTreeView::OnTvnBeginlabeledit)
    ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CSGMTreeView::OnTvnEndlabeledit)
END_MESSAGE_MAP()




// CSGMTreeView 消息处理程序


void CSGMTreeView::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    *pResult = 0;
    HWND eidtorHwnd = TreeView_GetEditControl(GetSafeHwnd());
}


void CSGMTreeView::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    *pResult = 0;
}
