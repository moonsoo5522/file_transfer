
// MFCclientDlg.h : 헤더 파일
//

#pragma once
#include <string>
#include "afxwin.h"
#include "afxcmn.h"
#include "Sock.h"
#include "FileControl.h"
#include "MkdirDlg.h"

// CMFCclientDlg 대화 상자
class CMFCclientDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCclientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
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