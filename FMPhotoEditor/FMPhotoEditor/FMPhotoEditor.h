
// FMPhotoEditor.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFMPhotoEditorApp:
// �� Ŭ������ ������ ���ؼ��� FMPhotoEditor.cpp�� �����Ͻʽÿ�.
//

class CFMPhotoEditorApp : public CWinApp
{
public:
	CFMPhotoEditorApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFMPhotoEditorApp theApp;