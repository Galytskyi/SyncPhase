#pragma once

class CColorListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CColorListCtrl)

public:
	CColorListCtrl();
	virtual ~CColorListCtrl();

protected:
	DECLARE_MESSAGE_MAP()

	CHeaderCtrl*	pHeaderCtrl;					
	int						nColumnCount;

public:
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


