// SGMMaskEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication9.h"
#include "SGMMaskEdit.h"


// CSGMMaskEdit
static UINT WM_EDIT_SELECT_ALL = WM_USER + 1;

IMPLEMENT_DYNAMIC(CSGMMaskEdit, CEdit)

CSGMMaskEdit::CSGMMaskEdit( LPCTSTR lpszValid /*= NULL*/, BOOL bNoCase /*= TRUE*/ ):
m_MaskStr(lpszValid),m_bNoCase(bNoCase)
{
    //m_MaskStr = _T("0123456789abcdefghijklmnopqrstuvwxyz_");
}

CSGMMaskEdit::~CSGMMaskEdit()
{
}

BOOL CSGMMaskEdit::IsLegalCharacters( WCHAR wChar ) const
{
    if(NeedCheckLegalCharacters())
    {
        CString srcStr(wChar);
        return IsInRange(m_MaskStr,srcStr,m_bNoCase);
    }

    return TRUE;
}

BOOL CSGMMaskEdit::IsLegalCharacters( const CString &srcString ) const
{
    if( NeedCheckLegalCharacters() && (!srcString.IsEmpty()) )
    {
        return IsInRange(m_MaskStr,srcString,m_bNoCase);
    }
    return TRUE;
}

BOOL CSGMMaskEdit::IsInRange( const CString &refString, const CString &srcString ,BOOL bNoCase /*= TRUE*/) const
{
    TRACE(_T("IsInRange : %d\n"),bNoCase);
    TRACE(_T("refString -> %s\n"),refString);
    TRACE(_T("srcString -> %s\n"),srcString);
    if( (!refString.IsEmpty()) && (!srcString.IsEmpty()) )
    {
        CString tempRefStr(refString);
        CString tempSrcStr(srcString);
        if(bNoCase)
        {
            tempRefStr.MakeLower();
            tempSrcStr.MakeLower();
        }
        int length = srcString.GetLength();
        for( int i = 0; i < length; i++)
        {
            if(-1 == tempRefStr.Find(tempSrcStr[i]))
            {
                TRACE(_T("CSGMMaskEdit::IsInRange -> FALSE\n"));
                return FALSE;
            }
        }
    }
    TRACE(_T("CSGMMaskEdit::IsInRange -> TRUE\n"));
    return TRUE;
}

BOOL CSGMMaskEdit::NeedCheckLegalCharacters() const
{
    return (!m_MaskStr.IsEmpty());
}

void CSGMMaskEdit::SetValidString( LPCTSTR lpszValid, BOOL bNoCase /*= TRUE*/ )
{
    m_MaskStr.Empty();
    m_MaskStr = lpszValid;
    m_bNoCase = bNoCase;
}

BEGIN_MESSAGE_MAP(CSGMMaskEdit, CEdit)
    ON_COMMAND(WM_CUT,  &CSGMMaskEdit::OnCut)
    ON_COMMAND(WM_COPY, &CSGMMaskEdit::OnCopy)
    ON_COMMAND(WM_PASTE,&CSGMMaskEdit::OnPaste)
    ON_COMMAND(WM_UNDO, &CSGMMaskEdit::OnUndo)
    ON_COMMAND(WM_EDIT_SELECT_ALL, &CSGMMaskEdit::OnSelectAll)
    ON_WM_CHAR()
    ON_CONTROL_REFLECT(EN_MAXTEXT, &CSGMMaskEdit::OnEnMaxtext)
END_MESSAGE_MAP()

void CSGMMaskEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(iswprint(nChar) && (!IsLegalCharacters(nChar)))
    {
        return;
    }
    CEdit::OnChar(nChar, nRepCnt, nFlags);
}




void CSGMMaskEdit::OnEnMaxtext()
{
    ShowBalloonTip(_T("MaxText"),_T("Limited 20"),TTI_ERROR);
}

void CSGMMaskEdit::OnCut(void)
{
    TRACE(_T("CSGMMaskEdit::OnCut\n"));
    CopyEditDataToClipboard();
    Clear();
}

void CSGMMaskEdit::OnCopy( void )
{
    TRACE(_T("CSGMMaskEdit::OnCopy\n"));
    CopyEditDataToClipboard();
}

void CSGMMaskEdit::OnPaste( void )
{
    TRACE(_T("CSGMMaskEdit::OnPaste\n"));
    CString strFromClipboard;
    if(ExtractDataFromClipboard(strFromClipboard))
    {
        if(!strFromClipboard.IsEmpty())
        {
            CString srcString(strFromClipboard);
            if( NeedCheckLegalCharacters() && (!IsInRange(m_MaskStr,srcString,m_bNoCase)))
            {
                return;
                TRACE(_T("CSGMMaskEdit::has illegal characters\n"));
            }
            Clear();
            Paste();
            TRACE(_T("CSGMMaskEdit::OnPaste\n"));
        }
    }
}

void CSGMMaskEdit::OnSelectAll( void )
{
    TRACE(_T("CSGMMaskEdit::OnSelectAll\n"));
    SetSel(0,-1);
}

void CSGMMaskEdit::OnUndo( void )
{
    if(CanUndo())
    {
        DWORD BeforeSel,AfterSel;
        BeforeSel = GetSel();
        Undo();
        AfterSel = GetSel();

        SetSel(0); // scroll to the left
        SetSel(AfterSel,TRUE); // select the last selection without scrolling window.
        
    }
}

BOOL CSGMMaskEdit::CopyEditDataToClipboard()
{   
    int start,end;
    CString srcStr;

    COleDataSource *pCopyData = new COleDataSource();
    GetWindowText(srcStr);
    GetSel(start,end);
    if(start >= end)
    {
        TRACE(_T("No Selected Characters\n"));
        return FALSE;
    }
    TRACE(_T("[%2d,%2d]\n"),start,end);
    TRACE(_T("CSGMMaskEdit::CopyEditDataToClipboard -> %s\n"),srcStr.Mid(start,end - start));
    INT length = (end - start) + 1;
    if(IsWindowUnicode(GetSafeHwnd()))
    {
        length *= sizeof(WCHAR);
    }
    HGLOBAL hGlobal = GlobalAlloc(GPTR,length);
    if(!hGlobal)
    {
        return FALSE;
    }

    WCHAR *pBuffer = (WCHAR*)(GlobalLock(hGlobal));
    memset(pBuffer,'\0',length);
    TRACE(_T("hGlobal size 0x%04x\n"),GlobalSize(hGlobal));
    for(int i = start; i < end; i++)
    {
        pBuffer[i-start] = srcStr[i];
    }
    GlobalUnlock(hGlobal);
    pCopyData->CacheGlobalData(CF_UNICODETEXT,hGlobal);
    pCopyData->SetClipboard();

    return TRUE;
}

BOOL CSGMMaskEdit::ExtractDataFromClipboard(CString& srcString)
{
    COleDataObject oleObject;
    HGLOBAL hGlobal = NULL;
    VERIFY(oleObject.AttachClipboard());

    if(oleObject.IsDataAvailable(CF_UNICODETEXT))
    {
        hGlobal = oleObject.GetGlobalData(CF_UNICODETEXT);
        srcString = (WCHAR*)GlobalLock(hGlobal);
        TRACE(_T("ExtractDataFromClipboard:CF_UNICODETEXT: 0x%04x\n"),GlobalSize(hGlobal));
        TRACE(_T("ExtractDataFromClipboard:CF_UNICODETEXT: %s\n"),srcString);
        GlobalUnlock(hGlobal);
    }
    else if(oleObject.IsDataAvailable(CF_TEXT))
    {
        hGlobal = oleObject.GetGlobalData(CF_TEXT);
        srcString = (CHAR*)GlobalLock(hGlobal);
        TRACE(_T("ExtractDataFromClipboard:CF_TEXT: 0x%04x\n"),GlobalSize(hGlobal));
        TRACE(_T("ExtractDataFromClipboard:CF_TEXT: %s\n"),srcString);
        GlobalUnlock(hGlobal);
    }
    else
    {
        TRACE(_T("ExtractDataFromClipboard: Unsupported format\n"));
        return FALSE;
    }
    GlobalFree(hGlobal);
    return TRUE;
}

BOOL CSGMMaskEdit::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        TRACE(_T("PreTranslateMessage -> 0x%03x\n"),pMsg->lParam);
        if(GetKeyState(VK_CONTROL))
        {
            switch(pMsg->wParam)
            {
            case 'A':
                TRACE(_T("CSGMMaskEdit::OnKeyDown -> CTRL + A\n"));
                SendMessage(WM_COMMAND,WM_EDIT_SELECT_ALL,0);
            return TRUE;

            case 'C':
                TRACE(_T("CSGMMaskEdit::OnKeyDown -> CTRL + C\n"));
                SendMessage(WM_COMMAND,WM_COPY,0);
            return TRUE;

            case 'X':
                TRACE(_T("CSGMMaskEdit::OnKeyDown -> CTRL + X\n"));
                SendMessage(WM_COMMAND,WM_CUT,0);
            return TRUE;

            case 'V':
                TRACE(_T("CSGMMaskEdit::OnKeyDown -> CTRL + V\n"));
                SendMessage(WM_COMMAND,WM_PASTE,0);
            return TRUE;

            case 'Z':
                TRACE(_T("CSGMMaskEdit::OnKeyDown -> CTRL + Z\n"));
                SendMessage(WM_COMMAND,WM_UNDO,0);
            return TRUE;

            default:
                break;
            }
        }
    }
    return CEdit::PreTranslateMessage(pMsg);
}


LRESULT CSGMMaskEdit::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    if(message == EM_UNDO)
    {
        TRACE("EM_UNDO\n");
    }
    return CEdit::WindowProc(message, wParam, lParam);
}
