#pragma once

#include "..\..\SERHIY_INCLUDE\crviewer.h"
#include "afxdtctl.h"
#include "afxwin.h"

// ======================================================================================

#define			POSTFIX_REPORT							_T("\\Reports")

// ======================================================================================

#define			ERR_REPORT_TYPE(ReportType) (ReportType < 0 || ReportType >= REPORT_TYPE_COUNT)

// ======================================================================================

const int		REPORT_TYPE_DIAG_SOURCE			= 0,
						REPORT_TYPE_DIAG_MESSAGE		= 1,
						REPORT_TYPE_DISCRETE_INPUT	= 2,
						REPORT_TYPE_ANALOG_INPUT		= 3,
						REPORT_TYPE_EEPROM_PARAM		= 4;

const int		REPORT_TYPE_COUNT						= 5;

const TCHAR* const ReportFileNameStr[REPORT_TYPE_COUNT] = 
{
	_T("Source.rpt"),
	_T("DiagMsgStr.rpt"),
	_T("DiscreteInput.rpt"),
	_T("AnalogInput.rpt"),
	_T("MemoryParam.rpt"),
};

const TCHAR* const ReportHeaderStr[REPORT_TYPE_COUNT] = 
{
	_T("Отчет источников"),
	_T("Отчет диагностических сообщений"),
	_T("Отчет дискретных входов"),
	_T("Отчет аналоговых входов"),
	_T("Отчет параметов"),
};

// ======================================================================================

#define	SQL_DIAG_MSG_STR  "SELECT `MessageD`.`TIME`, `MessageD`.`MSG`, `MessageD`.`DATE`, `MessageD`.`TIME_CODE`, `Sources`.`NAME` " \
													"FROM   `Sources` `Sources` INNER JOIN `MessageD` `MessageD` ON `Sources`.`SOURCE_ID`=`MessageD`.`SOURCE_ID`"

// CDlgReport dialog
// ======================================================================================

class CDlgReport : public CDialog
{
	DECLARE_DYNAMIC(CDlgReport)

public:
	CDlgReport(int ReportType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReport();

// Dialog Data
	enum { IDD = IDD_REPORT };

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	int								m_ReportType;

	IApplicationPtr		pApplication;																																			// интерфес связи отчетов с просмотырщиком
	IReportPtr				pReport;	

	CStatic						m_HeaderFrom;
	CDateTimeCtrl			m_DateFrom;
	CDateTimeCtrl			m_TimeFrom;
	CStatic						m_HeaderTo;
	CDateTimeCtrl			m_DateTo;
	CDateTimeCtrl			m_TimeTo;
	CButton						m_btUpdate;
	CCrviewer10				m_Viewer;

	virtual BOOL			OnInitDialog();

	CString						GetTimeRange();
	afx_msg void			OnReportUpdate();

	afx_msg void			OnSize(UINT nType, int cx, int cy);
};
// ======================================================================================