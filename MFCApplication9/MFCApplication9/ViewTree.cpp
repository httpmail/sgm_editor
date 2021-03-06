
#include "stdafx.h"
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CViewTree::OnNMCustomdraw)
    ON_WM_CTLCOLOR_REFLECT()
    ON_WM_MOVE()
    ON_WM_MOVING()
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 消息处理程序

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}
	return bRes;
}

#define MarcoToString(Marco) (#Marco)
#define MarcoToStringEnds(Marco) (#Marco##"\n")
void CViewTree::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    static DWORD nCount = 0;
    HDC hdc;
    CDC cdc;
    HTREEITEM hItem;
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    LPNMTVCUSTOMDRAW pNMTVCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
    *pResult = CDRF_DODEFAULT;
    TRACE("DrawStage --> 0x%04x, 0x%04x\n", pNMCD->dwDrawStage,nCount++);
    switch(pNMCD->dwDrawStage)
    {
    case CDDS_PREPAINT:
        *pResult  =0;
        break;
    case CDDS_POSTPAINT:
        break;
    case CDDS_ITEMPOSTPAINT:
        break;
    case CDDS_ITEMPREPAINT:
        TRACE("CDDS_ITEMPREPAINT Item, Status,text_color,text_bgcolor: ");
        TRACE("0x%04x,",pNMCD->dwItemSpec);
        TRACE("0x%04x,",pNMCD->uItemState);
        TRACE("0x%04x,",pNMTVCD->clrText);
        TRACE("0x%04x,",pNMTVCD->clrTextBk);
        TRACE("\n");
        pNMTVCD->clrTextBk = RGB(255,0,0);
        *pResult |= CDRF_NOTIFYPOSTPAINT;
    break;

    }

#if 0


    if(pNMTVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT /*&& (pNMTVCD->nmcd.uItemState & CDIS_SELECTED)*/)
    {
        TRACE("CViewTree::OnNMCustomdraw --> CDDS_ITEMPREPAINT 0x%04x\n",nCount++);
        CDC cdc;
        HTREEITEM hTreeItem = GetSelectedItem();
        cdc.Attach(pNMTVCD->nmcd.hdc);
        if(hTreeItem)
        {
            CRect itemRect;
            if(GetItemRect(hTreeItem,&itemRect,FALSE))
            {
                cdc.Draw3dRect(itemRect,RGB(255,0,0),RGB(255,0,0));
            }
           
        }
        
        cdc.Detach();
    }
#endif
}


HBRUSH CViewTree::CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/)
{
    // TODO:  在此更改 DC 的任何特性 
    return NULL;
    // TODO:  如果不应调用父级的处理程序，则返回非 null 画笔
    return NULL;
}


void CViewTree::OnMove(int x, int y)
{
    TRACE("CViewTree::OnMove\n");
    CTreeCtrl::OnMove(x, y);

    // TODO: 在此处添加消息处理程序代码
}


void CViewTree::OnMoving(UINT fwSide, LPRECT pRect)
{
    TRACE("CViewTree::OnMoving\n");
    CTreeCtrl::OnMoving(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
}


void CViewTree::OnSize(UINT nType, int cx, int cy)
{
    static DWORD cont = 0;
    TRACE("  ----------> CViewTree::OnSize <---------- 0x%04x\n",cont++);
    CTreeCtrl::OnSize(nType, cx, cy);
    SendMessage(NM_CUSTOMDRAW);
    // TODO: 在此处添加消息处理程序代码
}


void CViewTree::OnSizing(UINT fwSide, LPRECT pRect)
{
    TRACE("CViewTree::OnSizing\n");
    CTreeCtrl::OnSizing(fwSide, pRect);

    // TODO: 在此处添加消息处理程序代码
}


BOOL CViewTree::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    return TRUE;
    return CTreeCtrl::OnEraseBkgnd(pDC);
}


void CViewTree::OnPaint()
{
   // CPaintDC dc(this); // device context for painting
    CTreeCtrl::OnPaint();
        // TODO: 在此处添加消息处理程序代码
        // 不为绘图消息调用 CTreeCtrl::OnPaint()
}
