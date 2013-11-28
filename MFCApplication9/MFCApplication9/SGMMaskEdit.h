#pragma once


// CSGMMaskEdit

class CSGMMaskEdit : public CEdit
{
	DECLARE_DYNAMIC(CSGMMaskEdit)

public:
	CSGMMaskEdit(LPCTSTR lpszValid = NULL, BOOL bNoCase = TRUE);
	virtual ~CSGMMaskEdit();

public:
    void SetValidString(LPCTSTR lpszValid, BOOL bNoCase = TRUE);

protected:
    BOOL NeedCheckLegalCharacters() const;
    BOOL IsLegalCharacters(WCHAR wChar) const;
    BOOL IsLegalCharacters(const CString &srcString) const;
    BOOL IsInRange(const CString &refString, const CString &srcString) const;

private:
    CString m_MaskStr;
    BOOL    m_bNoCase;
    PEDITBALLOONTIP m_IllegalTip;
    PEDITBALLOONTIP m_LimitedTip;

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnEnMaxtext();
};


