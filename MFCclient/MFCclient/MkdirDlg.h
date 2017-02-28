#pragma once
#include "afxwin.h"


// CMkdirDlg 대화 상자입니다.

class CMkdirDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMkdirDlg)

public:
	CMkdirDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMkdirDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit dirName;
};
