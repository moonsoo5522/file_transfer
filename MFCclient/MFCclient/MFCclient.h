
// MFCclient.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMFCclientApp:
// �� Ŭ������ ������ ���ؼ��� MFCclient.cpp�� �����Ͻʽÿ�.
//

class CMFCclientApp : public CWinApp
{
public:
	CMFCclientApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMFCclientApp theApp;