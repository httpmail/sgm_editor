#pragma once

class CSGMTreeView : public CTreeCtrl
{
public:
    enum {
        CLASSICAL_STYLE = TVS_EDITLABELS|WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    };

public:
	CSGMTreeView();          
	virtual ~CSGMTreeView();

protected:
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};


