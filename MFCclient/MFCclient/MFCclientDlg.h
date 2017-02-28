
// MFCclientDlg.h : ��� ����
//

#pragma once
#include <string>
#include "afxwin.h"
#include "afxcmn.h"
#include "Sock.h"
#include "FileControl.h"
#include "MkdirDlg.h"

// CMFCclientDlg ��ȭ ����
class CMFCclientDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CMFCclientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl mCtrlList;
	afx_msg void OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	CEdit fPath;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();

	void renewList();
	static unsigned int __stdcall recvPacket(void * arg);
	CProgressCtrl mProgress;
	Sock		  soc;
	FileControl files;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();

	CMkdirDlg mkd;
	CButton recvBtn;
	CButton sendBtn;
};