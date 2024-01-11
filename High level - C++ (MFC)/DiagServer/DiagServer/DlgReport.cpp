// DlgReport.cpp : implementation file
//

#include "stdafx.h"
#include "DiagServer.h"
#include "DlgReport.h"


// CDlgReport dialog
// -------------------------------------------------------------------------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CDlgReport, CDialog)

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgReport::CDlgReport(int ReportType, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReport::IDD, pParent)
	, m_ReportType( ReportType )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CDlgReport::~CDlgReport()
{
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgReport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REPORT_DATE_FROM_H,	m_HeaderFrom);
	DDX_Control(pDX, IDC_REPORT_DATE_FROM,		m_DateFrom);
	DDX_Control(pDX, IDC_REPORT_TIME_FROM,		m_TimeFrom);
	DDX_Control(pDX, IDC_REPORT_DATE_TO_H,		m_HeaderTo);
	DDX_Control(pDX, IDC_REPORT_DATE_TO,			m_DateTo);
	DDX_Control(pDX, IDC_REPORT_TIME_TO,			m_TimeTo);
	DDX_Control(pDX, ID_REPORT_UPDATE,				m_btUpdate);
	DDX_Control(pDX, IDC_REPORT_VIEWER,				m_Viewer);
	
	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

BEGIN_MESSAGE_MAP(CDlgReport, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(ID_REPORT_UPDATE, &CDlgReport::OnReportUpdate)
END_MESSAGE_MAP()

// -------------------------------------------------------------------------------------------------------------------------------------------------

BOOL CDlgReport::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (ERR_REPORT_TYPE(m_ReportType) )
	{
		EndDialog(0);
		return FALSE;
	}

	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	CTime TimeFrom;
	TimeFrom = CTime::GetCurrentTime() - CTimeSpan(0, 1, 0, 0);

	m_DateFrom.SetTime(&CTime::GetCurrentTime());	
	m_TimeFrom.SetTime(&TimeFrom);
	m_DateTo.SetTime(&CTime::GetCurrentTime());	
	m_TimeTo.SetTime(&CTime::GetCurrentTime());	


	TCHAR PathReportName[MAX_PATH];
	
	_tcscpy_s(PathReportName, theApp.CurrentDirectoryStr);

	_tcscat_s(PathReportName, POSTFIX_REPORT);
	_tcscat_s(PathReportName, _T("\\"));
	_tcscat_s(PathReportName, ReportFileNameStr[m_ReportType]);

	SetWindowText( ReportHeaderStr[m_ReportType] );

	try
	{
		HRESULT hResult = pApplication.CreateInstance(_T("CrystalRuntime.Application.10"));
		pReport = pApplication->OpenReport( PathReportName );
		m_Viewer->put_ReportSource(pReport);
		pReport->DiscardSavedData();

		if (m_ReportType == REPORT_TYPE_DIAG_MESSAGE)
		{
			BSTR SelStr;
			_bstr_t _bstr = SQL_DIAG_MSG_STR;
			SelStr = _bstr +  "\n WHERE \n" + GetTimeRange().GetBuffer();
			pReport->put_SQLQueryString(SelStr);
			//pReport->get_SQLQueryString(&SelStr);
			//CString str = SelStr;
			//CFile file;
			//file.Open(_T("report_sql.txt"), CFile::modeCreate | CFile::modeWrite);
			//file.Write(CStringA( str ), str.GetLength());
			//file.Close();
		}
		
		m_Viewer->ViewReport();
	}
	catch (_com_error& comerr)
	{
		IErrorInfo* pErrorInfo = comerr.ErrorInfo();
		HRESULT hr = comerr.Error();
		if (pErrorInfo)	
		{
			BSTR bsDesc = NULL;
			pErrorInfo->GetDescription( &bsDesc );
			_bstr_t sDesc( bsDesc, false );
			MessageBox( sDesc.operator LPCTSTR() );
			pErrorInfo->Release();
		}

		return FALSE;
	}

	CRect	wndRect;
	GetClientRect(&wndRect);
	OnSize(-1, wndRect.Width(), wndRect.Height());

	int nCmdShow = SW_HIDE;

	if (m_ReportType == REPORT_TYPE_DIAG_MESSAGE)
		nCmdShow = SW_SHOW;

	m_HeaderFrom.ShowWindow(nCmdShow);
	m_DateFrom.ShowWindow(nCmdShow);
	m_TimeFrom.ShowWindow(nCmdShow);
	m_HeaderTo.ShowWindow(nCmdShow);
	m_DateTo.ShowWindow(nCmdShow);
	m_TimeTo.ShowWindow(nCmdShow);
	m_btUpdate.ShowWindow(nCmdShow);

	return TRUE; 
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

CString CDlgReport::GetTimeRange()
{
	CString ResultString;

	CTime DateFrom, TimeFrom, DateTo, TimeTo;

	m_DateFrom.GetTime(DateFrom);
	m_TimeFrom.GetTime(TimeFrom);
	m_DateTo.GetTime(DateTo);
	m_TimeTo.GetTime(TimeTo);

	DIAG_MSG_TIME_CODE MsgTimeFrom(TimeFrom, DateFrom), MsgTimeTo(TimeTo, DateTo);

	ResultString.AppendFormat(_T("(`MessageD`.`TIME_CODE` >= %u AND `MessageD`.`TIME_CODE` <= %u)"), MsgTimeFrom.TimeCode, MsgTimeTo.TimeCode);	

	return ResultString;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgReport::OnReportUpdate()
{
	try
	{
		BSTR SelStr;
		_bstr_t _bstr = SQL_DIAG_MSG_STR;

		SelStr = _bstr + "\n WHERE \n" + GetTimeRange().GetBuffer();

		pReport->put_SQLQueryString(SelStr);
		
		m_Viewer->Refresh();
	}
	catch (_com_error& comerr)
	{
		IErrorInfo* pErrorInfo = comerr.ErrorInfo();
		HRESULT hr = comerr.Error();
		if (pErrorInfo){
			BSTR bsDesc = NULL;
			pErrorInfo->GetDescription( &bsDesc );
			_bstr_t sDesc( bsDesc, false );
			MessageBox( sDesc.operator LPCTSTR() );
			pErrorInfo->Release();
		}
	}	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

void CDlgReport::OnSize(UINT nType, int cx, int cy)
{
	if (nType != -1 )
		CDialog::OnSize(nType, cx, cy);
	
	CRect	wndRect;
	
	if (m_Viewer.m_hWnd)
	{
		if (m_ReportType == REPORT_TYPE_DIAG_MESSAGE)
			wndRect.top		= 50;				
		else
			wndRect.top		= 10;		

		wndRect.left		= 10;				
		wndRect.right		= cx - 10;	
		wndRect.bottom	= cy - 10;
		
		m_Viewer.MoveWindow(&wndRect);
	}

	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------

