// SGMMaskEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication9.h"
#include "SGMMaskEdit.h"


// CSGMMaskEdit

IMPLEMENT_DYNAMIC(CSGMMaskEdit, CEdit)

CSGMMaskEdit::CSGMMaskEdit( LPCTSTR lpszValid /*= NULL*/, BOOL bNoCase /*= TRUE*/ ):
m_MaskStr(lpszValid),m_bNoCase(bNoCase)
{
    m_MaskStr = _T("0123456789abcdefghijklmnopqrstuvwxyz_");
}

CSGMMaskEdit::~CSGMMaskEdit()
{
}

BOOL CSGMMaskEdit::IsLegalCharacters( WCHAR wChar ) const
{
    if(NeedCheckLegalCharacters())
    {
        CString refStr(m_MaskStr);
        CString srcStr(wChar);
        if(m_bNoCase)
        {
            refStr.MakeLower();
            srcStr.MakeLower();
        }
        return IsInRange(refStr,srcStr);
    }

    return TRUE;
}

BOOL CSGMMaskEdit::IsLegalCharacters( const CString &srcString ) const
{
    if( NeedCheckLegalCharacters() && (!srcString.IsEmpty()) )
    {
        CString refStr(m_MaskStr);
        CString srcStr(srcString);
        if(m_bNoCase)
        {
            refStr.MakeLower();
            srcStr.MakeLower();
        }
        return IsInRange(refStr,srcStr);
    }
    return TRUE;
}

BOOL CSGMMaskEdit::IsInRange( const CString &refString, const CString &srcString ) const
{
    TRACE(_T("refString -> %s\n"),refString);
    TRACE(_T("srcString -> %s\n"),srcString);
    if( (!refString.IsEmpty()) && (!srcString.IsEmpty()) )
    {
        int length = srcString.GetLength();
        for( int i = 0; i < length; i++)
        {
            if(-1 == refString.Find(srcString[i]))
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
        TRACE(_T("Length: 0x%04x\n"),strFromClipboard.GetLength());
    }
}

void CSGMMaskEdit::OnUndo( void )
{
    TRACE(_T("CSGMMaskEdit::OnUndo\n"));
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
    TRACE(_T("CSGMMaskEdit::CopyEditDataToClipboard %s\n"),srcStr.Mid(start,end));

    HGLOBAL hGlobal = GlobalAlloc(GPTR,srcStr.Mid(start,end).GetLength());
    if(hGlobal)
    {
        return FALSE;
    }

    WCHAR *pBuffer = (WCHAR*)(GlobalLock(hGlobal));
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
        TRACE(_T("ExtractDataFromClipboard:CF_TEXT: 0x%04x\n"),GlobalSize(hGlobal));
        GlobalUnlock(hGlobal);
        return TRUE;
    }
    else if(oleObject.IsDataAvailable(CF_TEXT))
    {
        hGlobal = oleObject.GetGlobalData(CF_TEXT);
        srcString = (CHAR*)GlobalLock(hGlobal);
        TRACE(_T("ExtractDataFromClipboard:CF_TEXT: 0x%04x\n"),GlobalSize(hGlobal));
        GlobalUnlock(hGlobal);
        return TRUE;
    }
    else
    {
        TRACE(_T("ExtractDataFromClipboard: Unsupported format\n"));
        return FALSE;
    }
}



