#pragma once
#include "afxwin.h"


// CMkdirDlg ��ȭ �����Դϴ�.

class CMkdirDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMkdirDlg)

public:
	CMkdirDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMkdirDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit dirName;
};
