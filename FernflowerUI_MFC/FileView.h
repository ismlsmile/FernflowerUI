#pragma once

#include "ShellTreeView.h"

// CFileView

class CFileView : public CDockablePane
{
	DECLARE_DYNAMIC(CFileView)

public:
	CFileView();
	virtual ~CFileView();
	friend class COutputWnd;
	friend class CFernflowerUIMFCApp;
	friend class CSubThread;
	friend class CShellTreeView;

protected:
	DECLARE_MESSAGE_MAP()
	CShellTreeView m_ShellTreeView;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
protected:
	void AdjustLayout();
};


class CSubThread :public CWinThread
{
public:
	friend class CFileView;
	CSubThread();
	~CSubThread();
	CSubThread(AFX_THREADPROC pfnThreadProc, LPVOID pParam);
protected:
	BOOL InitInstance();
	HHOOK m_CreationHook;
	void HookWindowCreate();
	bool UnHookWindowCreate();
	DECLARE_DYNCREATE(CSubThread)
protected:
	CFileView * pParentView;
	HWND MainWnd;
	bool Created;
};