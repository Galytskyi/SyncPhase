// ColorListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ColorListCtrl.h"

// CColorListCtrl

IMPLEMENT_DYNAMIC(CColorListCtrl, CListCtrl)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CColorListCtrl::CColorListCtrl()
: pHeaderCtrl(NULL)
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CColorListCtrl::~CColorListCtrl()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CColorListCtrl, CListCtrl)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CColorListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTVIEW);

	int nItem = lpDrawItemStruct->itemID;
	if (nItem == CB_ERR)
		return;
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	if (!pDC)
		return;

	COLORREF crOldTextColor = pDC->GetTextColor();
	COLORREF crOldBkColor		= pDC->GetBkColor();
	
	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_INDENT;
	lvi.iItem = nItem;
	GetItem(&lvi);

	COLORREF crColor = (COLORREF) lvi.iIndent;
		
	if (!pHeaderCtrl)
	{
		pHeaderCtrl = GetHeaderCtrl();
		nColumnCount = pHeaderCtrl->GetItemCount();
	}

	CRect rcItem(lpDrawItemStruct->rcItem);
	CRect rcSubItem;

						
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
    (lpDrawItemStruct->itemState  & ODS_SELECTED))
  {
    pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    pDC->SetBkColor(::GetSysColor(COLOR_HIGHLIGHT));
    pDC->FillSolidRect(&rcItem, ::GetSysColor(COLOR_HIGHLIGHT));
  }
  else
		pDC->FillSolidRect(&rcItem, !crColor ? crOldBkColor : crColor);

	CString str;
	LV_COLUMN Column;
	for(int nSubItem = 0; nSubItem < nColumnCount; nSubItem++)
	{
		int fmt = 0;
		Column.mask = LVCF_FMT;
		GetColumn(nSubItem, &Column);
		if (Column.fmt & LVCFMT_LEFT ) fmt = DT_LEFT;
		if (Column.fmt & LVCFMT_CENTER ) fmt = DT_CENTER;
		if (Column.fmt & LVCFMT_RIGHT ) fmt = DT_RIGHT;


		str = GetItemText(nItem, nSubItem);
		GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcSubItem);
		rcSubItem.DeflateRect(4,0,4,0);
		pDC->DrawText(str, str.GetLength(), rcSubItem,	fmt|DT_SINGLELINE| DT_VCENTER);
	}
	
  pDC->SetTextColor(crOldTextColor);
  pDC->SetBkColor(crOldBkColor);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

