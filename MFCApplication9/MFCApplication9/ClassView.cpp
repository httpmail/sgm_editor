
#include "stdafx.h"
#include "MainFrm.h"
#include "ClassView.h"
#include "Resource.h"
#include "MFCApplication9.h"


class CClassViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CClassView;

	DECLARE_SERIAL(CClassViewMenuButton)

public:
	CClassViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CClassViewMenuButton, CMFCToolBarMenuButton, 1)

//////////////////////////////////////////////////////////////////////
// 构造/析构
//////////////////////////////////////////////////////////////////////

CClassView::CClassView()
{
	pctrl = NULL;
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
}

CClassView::~CClassView()
{
}

BEGIN_MESSAGE_MAP(CClassView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClassView 消息处理程序

int CClassView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

    m_pToolTip = NULL;
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	if(pctrl)
	{
	}
	// 创建视图:
	const DWORD dwViewStyle =  TVS_FULLROWSELECT| TVS_SHOWSELALWAYS| TVS_EDITLABELS|WS_CHILD | WS_VISIBLE | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("未能创建类视图\n");
		return -1;      // 未能创建
	}

	// 加载图像:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CClassViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	FillClassView();

	return 0;
}

void CClassView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CClassView::FillClassView()
{
	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("FakeApp 类"), 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hClass = m_wndClassView.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);

	m_wndClassView.Expand(hRoot, TVE_EXPAND);

	hClass = m_wndClassView.InsertItem(_T("CFakeApp"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeApp()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("InitInstance()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnAppAbout()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppDoc"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppDoc()"), 4, 4, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppDoc()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnNewDocument()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppView"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppView()"), 4, 4, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetDocument()"), 3, 3, hClass);
	m_wndClassView.Expand(hClass, TVE_EXPAND);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppFrame"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_wndMenuBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndToolBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndStatusBar"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("Globals"), 2, 2, hRoot);
	m_wndClassView.InsertItem(_T("theFakeApp"), 5, 5, hClass);
	m_wndClassView.Expand(hClass, TVE_EXPAND);
}

void CClassView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);
    TRACE("CClassView::OnContextMenu(CWnd* pWnd, CPoint point) --> %08x, %08x\n",m_wndClassView.GetEditControl(),pWnd);
	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		//m_wndClassView.OnContextMenu(pWnd,point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}
	pWndTree->SetFocus();
#if 0
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}
#endif
}

void CClassView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CClassView::PreTranslateMessage(MSG* pMsg)
{
	//if(m_pToolTipCtrl)
	//	m_pToolTipCtrl->RelayEvent(pMsg);
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CClassView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CClassViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CClassViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CClassView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CClassView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("添加成员函数..."));
}

void CClassView::OnClassAddMemberVariable()
{
	// TODO: 在此处添加命令处理程序代码
}

void CClassView::OnClassDefinition()
{
	// TODO: 在此处添加命令处理程序代码
}

void CClassView::OnClassProperties()
{
	// TODO: 在此处添加命令处理程序代码
}

void CClassView::OnNewFolder()
{
	AfxMessageBox(_T("新建文件夹..."));
}

void CClassView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CClassView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CClassView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* 锁定*/);
}
BEGIN_MESSAGE_MAP(CMyViewTree, CViewTree)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, &CMyViewTree::OnTvnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CMyViewTree::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CMyViewTree::OnTvnSelchanged)
	ON_NOTIFY_REFLECT(TVN_SELCHANGING, &CMyViewTree::OnTvnSelchanging)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMCHANGING, &CMyViewTree::OnTvnItemChanging)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CMyViewTree::OnTvnKeydown)
	ON_WM_NOTIFYFORMAT()
    ON_COMMAND(AFX_IDD_EDITLINKS, &CMyViewTree::OnAfxIddEditlinks)
    ON_WM_CREATE()
    ON_WM_SETCURSOR()
    ON_WM_KEYUP()
    ON_COMMAND(ID_EDIT_COPY, &CMyViewTree::OnEditCopy)
    ON_COMMAND(ID_EDIT_CUT, &CMyViewTree::OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, &CMyViewTree::OnEditPaste)
    ON_COMMAND(ID_EDIT_UNDO, &CMyViewTree::OnEditUndo)
    ON_COMMAND(ID_EDIT_CLEAR, &CMyViewTree::OnEditClear)
    ON_WM_ACTIVATE()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

HWND editorHwnd;
void CMyViewTree::OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	if(pTVDispInfo->item.lParam)
	{
		*pResult = TRUE;
	}
	else
	{
		*pResult = FALSE;
	}
    int end;
    CEdit *pEdit = GetEditControl();
    pEdit->SetLimitText(20);
    pEdit->GetSel(m_SelBeforUpdate,end);
    m_MyEditor.SubclassWindow(TreeView_GetEditControl(GetSafeHwnd()));
   // m_MyEditor.SetValidChars(_T("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"));
}


void CMyViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
    TRACE("CMyViewTree::OnTvnEndlabeledit\n");
	if(pTVDispInfo->item.pszText)
	{
		*pResult = 0;
		CString temp = this->GetItemText(GetSelectedItem());
		this->SetItemText(GetSelectedItem(),pTVDispInfo->item.pszText);
		HWND hWnd = TreeView_GetEditControl(this->GetSafeHwnd());

	}
	else
	{
		HWND hWnd = TreeView_GetEditControl(this->GetSafeHwnd());
		*pResult = 1;
	}
    if(m_MyEditor.GetSafeHwnd() != NULL)
    {
        m_MyEditor.UnsubclassWindow();
    }
}


void CMyViewTree::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 1;
}


void CMyViewTree::OnTvnSelchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
     TRACE("CMyViewTree::OnTvnSelchanging\n");
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
    CString str;
    TRACE("CMyViewTree::OnTvnSelchanging ---> %0x \n",GetEditControl());
    if(GetEditControl())
    {
        CEdit *pEdit = GetEditControl();
        if( 3 > pEdit->GetWindowTextLength())
            *pResult = 1;
    }
	*pResult = 0;
}
void CMyViewTree::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 在此处添加消息处理程序代码
    TRACE("CMyViewTree::OnContextMenu\n");
}


void CMyViewTree::OnTvnItemChanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE *>(pNMHDR);
	*pResult = 0;
}


void CMyViewTree::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
		*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

CString temp;

BOOL CMyViewTree::PreTranslateMessage(MSG* pMsg)
{
	return CViewTree::PreTranslateMessage(pMsg);
}



void CMyViewTree::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
    TRACE("CMyViewTree::PreSubclassWindow\n");
	CViewTree::PreSubclassWindow();
}


LRESULT CMyViewTree::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    return CViewTree::DefWindowProc(message, wParam, lParam);
}


UINT CMyViewTree::OnNotifyFormat(CWnd *pWnd, UINT nCommand)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	return CViewTree::OnNotifyFormat(pWnd, nCommand);
}

 HACCEL gHaccel;
BOOL CMyViewTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	CViewTree::Create(dwStyle, rect, pParentWnd, nID);
	m_pToolTipCtrl = NULL;
    gHaccel = ::LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
    TRACE("gHacclel %0x",gHaccel);
	return true;
}


void CMyViewTree::OnAfxIddEditlinks()
{
    // TODO: 在此添加命令处理程序代码
}


int CMyViewTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CViewTree::OnCreate(lpCreateStruct) == -1)
        return -1;
        return 0;
}


BOOL CMyViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(wParam)
    {
        LPNMHDR pNMHDR = (LPNMHDR)lParam;
        HWND hwnd = pNMHDR->hwndFrom;
        //pNMHDR->idFrom
    }
    return CViewTree::OnNotify(wParam, lParam, pResult);
}


BOOL CMyViewTree::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
{
    // TODO: 在此添加专用代码和/或调用基类
    return CViewTree::OnChildNotify(message, wParam, lParam, pLResult);
}


BOOL CMyViewTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return CViewTree::OnSetCursor(pWnd, nHitTest, message);
}


LRESULT CMyViewTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类 
    return CViewTree::WindowProc(message, wParam, lParam);
}

void CMyViewTree::OnItemEditChangeCursor()
{
    CEdit *pEdit = GetEditControl();
    int start,end;
    pEdit->GetSel(m_SelBeforUpdate,end);
    TRACE("CMyViewTree::OnItemEditChangeCursor start  ---> %2d\n", m_SelBeforUpdate);
}

void CMyViewTree::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CViewTree::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CMyViewTree::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(message == WM_COMMAND)
    if(GetEditControl() && (GetEditControl() == CWnd::FromHandle((HWND)lParam)))
    {
    }
    return CViewTree::OnWndMsg(message, wParam, lParam, pResult);
}


BOOL CMyViewTree::OnCommand(WPARAM wParam, LPARAM lParam)
{
    return CViewTree::OnCommand(wParam, lParam);
}


void CMyViewTree::OnEditCopy()
{

    TRACE("OnEditCopy!!\n");
    
    CEdit *pEdit = GetEditControl();
    if(pEdit)
    {
        COleDataSource *pCopyData = new COleDataSource();
        CString text;
        pEdit->GetWindowText(text);
        int start,end;
        pEdit->GetSel(start,end);
        if(start >= end)
        {
            TRACE("OnEditCopy -> un-select!!!");
            return;
        }
        HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE,text.Mid(start,end).GetLength());
        TRACE(_T("HGLOBAL size %3d\n"),GlobalSize(hGlobal));
        char *pGlobal = (char*)(GlobalLock(hGlobal));

        for(int i = start; i < end; i++)
        {
           // pGlobal[i] = text[i];
            //TRACE("%c"),text[i]);
        }
        GlobalUnlock(hGlobal);
        TRACE(_T("\n"));
        pCopyData->CacheGlobalData(CF_TEXT,hGlobal);
        pCopyData->SetClipboard();
    }
    // TODO: 在此添加命令处理程序代码
}


void CMyViewTree::OnEditCut()
{
    TRACE("OnEditCut!!\n");
    // TODO: 在此添加命令处理程序代码
}


void CMyViewTree::OnEditPaste()
{
    TRACE("OnEditPaste!!\n");
    CEdit *pEdit = GetEditControl();
    if(pEdit)
    {
        pEdit->SetWindowText(_T("xxx"));
    }
    // TODO: 在此添加命令处理程序代码
}


void CMyViewTree::OnEditUndo()
{
    TRACE("OnEditUndo!!\n");
    // TODO: 在此添加命令处理程序代码
}


void CMyViewTree::OnEditClear()
{
    TRACE("OnEditClear!!\n");
    // TODO: 在此添加命令处理程序代码
}

void CMyViewTree::IllicitCharacterCheck()
{

}


void CMyViewTree::DoDataExchange(CDataExchange* pDX)
{
    // TODO: 在此添加专用代码和/或调用基类
    CViewTree::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(MyEditor, CEdit)
    ON_CONTROL_REFLECT(EN_MAXTEXT, &MyEditor::OnEnMaxtext)
//    ON_WM_CONTEXTMENU()
ON_WM_CHAR()
ON_WM_KILLFOCUS()
ON_WM_CLOSE()
ON_WM_DESTROY()
ON_WM_ACTIVATE()
//ON_CONTROL_REFLECT(EN_UPDATE, &MyEditor::OnEnUpdate)
//ON_CONTROL_REFLECT(EN_UPDATE, &MyEditor::OnEnUpdate)
END_MESSAGE_MAP()


void MyEditor::OnEnMaxtext()
{
    TRACE("MyEditor::OnEnMaxtext\n");
    // TODO: 在此添加控件通知处理程序代码
}


//void MyEditor::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
//{
//    // TODO: 在此处添加消息处理程序代码
//}


void MyEditor::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    TRACE("MyEditor::OnChar --> %04d, %04d,%04d\n",nChar, nRepCnt, nFlags);
    CString str("1234567");
    if(_istprint(nChar))
    {
        if(str.Find(nChar) == -1)
        {
            TRACE("MyEditor::OnChar : not found!!!! \n");
            return;
        }
        else
        {
            TRACE("MyEditor::OnChar : found!!!! \n");
        }
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}


void MyEditor::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);
    // TODO: 在此处添加消息处理程序代码
}


void MyEditor::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    TRACE("MyEditor::OnClose()\n");
    CEdit::OnClose();
}


void MyEditor::OnDestroy()
{
    TRACE("MyEditor::OnDestroy\n");
    CEdit::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
}


void MyEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    TRACE("MyEditor::OnActivate %4d, %08x, %02d\n",nState,pWndOther,bMinimized);
    CEdit::OnActivate(nState, pWndOther, bMinimized);

    // TODO: 在此处添加消息处理程序代码
}


HRESULT CMyViewTree::accSelect(long flagsSelect, VARIANT varChild)
{
    // TODO: 在此添加专用代码和/或调用基类
    TRACE("CMyViewTree::accSelect\n");
    return CViewTree::accSelect(flagsSelect, varChild);
}


void CMyViewTree::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
    TRACE("CMyViewTree::OnActivate\n");
    CViewTree::OnActivate(nState, pWndOther, bMinimized);

    // TODO: 在此处添加消息处理程序代码
}


void CMyViewTree::OnSetFocus(CWnd* pOldWnd)
{

    TRACE("CMyViewTree::OnKillFocus %0x\n",pOldWnd);
    CViewTree::OnSetFocus(pOldWnd);

    // TODO: 在此处添加消息处理程序代码
}


void CMyViewTree::OnKillFocus(CWnd* pNewWnd)
{
    TRACE("CMyViewTree::OnKillFocus %0x\n",pNewWnd);
    CViewTree::OnKillFocus(pNewWnd);

    // TODO: 在此处添加消息处理程序代码
}


//void MyEditor::OnEnUpdate()
//{
//    // TODO:  如果该控件是 RICHEDIT 控件，它将不
//    // 发送此通知，除非重写 CMFCMaskedEdit::OnInitDialog()
//    // 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
//    // 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。
//
//
//
//    // TODO:  在此添加控件通知处理程序代码
//}


//void MyEditor::OnEnUpdate()
//{
//    // TODO:  如果该控件是 RICHEDIT 控件，它将不
//    // 发送此通知，除非重写 CEdit::OnInitDialog()
//    // 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
//    // 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。
//    // TODO:  在此添加控件通知处理程序代码
//    CString str;
//    TRACE(_T("MyEditor::OnEnUpdate() --> %s\n"),str);
//}
