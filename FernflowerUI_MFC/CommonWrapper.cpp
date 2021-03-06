#include "stdafx.h"
#include "CommonWrapper.h"
using namespace CommonWrapper;

CWaitDlg::CWaitDlg(CWnd * MainWnd, std::function<bool()> c, DWORD Sec, const wchar_t * Text, DWORD MarSec,UINT nIDTemplate,bool EnableParent)
	:Condit(c), CDialogEx(nIDTemplate, MainWnd), WaitSeconds(Sec), StaticText(Text),MarqueeSecond(MarSec),bEnableParent(EnableParent)
{
	WaitSeconds = (WaitSeconds == 0) ? 1 : WaitSeconds;
	MarqueeSecond = (MarqueeSecond == 0) ? 20 : MarqueeSecond;
}

LRESULT CWaitDlg::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT bRet = HandleInitDialog(wParam, lParam);
	m_pParentWnd->EnableWindow(bEnableParent);
	SetTimer(ID_TIMER_DOPROGRESS, WaitSeconds, nullptr);
	m_wndProgressBar = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS1));
	if (m_wndProgressBar != nullptr)
	{
		m_wndProgressBar->SetRange(0, 100);
		m_wndProgressBar->SetStep(1);
		m_wndProgressBar->SetMarquee(true, MarqueeSecond);
	}
	m_pStaticText = static_cast<CStatic*>(GetDlgItem(ID_MESSAGE_NOTICE));
	if (m_pStaticText!=nullptr)
	{
		m_pStaticText->SetWindowTextW(StaticText);
	}
	if (IsWindow(CDialogEx::GetParent()->GetSafeHwnd())&&(!theApp.Initing))
	{
		static_cast<CMainFrame*>(CDialogEx::GetParent())->GetStatusBar()->SetPaneText(ID_SEPARATOR, L"正在执行操作");
	}
	if (m_Font.CreatePointFont(120, L"Microsoft YaHei"))
	{
		if(static_cast<CStatic*>(GetDlgItem(IDC_GROUPTITLE))!=nullptr)
			static_cast<CStatic*>(GetDlgItem(IDC_GROUPTITLE))->SetFont(&m_Font);
	}
	return bRet;
}

void CWaitDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	theApp.RunningDialog = this;
	CDialogEx::OnShowWindow(bShow, nStatus);
	PostMessage(WM_COMMAND, ID_DOPROGRESS);
}

void CWaitDlg::DoProgress()
{
	if (!Condit())
	{
		return;
	}
	KillTimer(ID_TIMER_DOPROGRESS);
	if (IsWindow(CDialogEx::GetParent()->GetSafeHwnd())&&(!theApp.Initing))
	{
		static_cast<CMainFrame*>(CDialogEx::GetParent())->GetStatusBar()->SetPaneText(ID_SEPARATOR, IsInChinese()?L"就绪":L"Ready");
	}
	theApp.RunningDialog = nullptr;
	EndDialog(true);
}

void CWaitDlg::DoDataExchange(CDataExchange * pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CWaitDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_DOPROGRESS)
	{
		SendMessage(WM_COMMAND, ID_DOPROGRESS);
	}
	else
		CDialogEx::OnTimer(nIDEvent);
}

BEGIN_MESSAGE_MAP(CWaitDlg, CDialogEx)
	ON_MESSAGE(WM_INITDIALOG, &CWaitDlg::OnInitDialog)
	ON_COMMAND(ID_DOPROGRESS, &CWaitDlg::DoProgress)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

CProgressBar::CProgressBar(CWnd * MainWnd, std::function<int()> c, DWORD Sec, const wchar_t * Text,int LowBound,int HighBound) 
	:Condit(c), CDialogEx(IDD_PROGRESSBAR, MainWnd), WaitSeconds(Sec), StaticText(Text),iLow(LowBound),iHigh(HighBound)
{
	WaitSeconds = (WaitSeconds == 0) ? 1 : WaitSeconds;
#ifdef _DEBUG
	TRACE(L"CProgressBar object constructed\n");
#endif // _DEBUG
}

LRESULT CProgressBar::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
	TRACE(L"CProgressBar dialog initing...\n");
#endif // _DEBUG
	LRESULT bRet = HandleInitDialog(wParam, lParam);
	SetTimer(ID_TIMER_DOPROGRESS, WaitSeconds, nullptr);
	m_wndProgressBar = static_cast<CProgressCtrl*>(GetDlgItem(IDC_PROGRESS1));
	m_wndProgressBar->SetRange(iLow,iHigh);
	m_wndProgressBar->SetStep(1);
	m_pStaticText = static_cast<CStatic*>(GetDlgItem(ID_MESSAGE_NOTICE));
	m_pStaticText->SetWindowTextW(StaticText);
	static_cast<CMainFrame*>(CDialogEx::GetParent())->GetStatusBar()->SetPaneText(ID_SEPARATOR, L"正在打开文件");
	if (m_Font.CreatePointFont(120, L"Microsoft YaHei"))
	{
		static_cast<CStatic*>(GetDlgItem(IDC_GROUPTITLE))->SetFont(&m_Font);
	}
	return bRet;
}

void CProgressBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	theApp.RunningDialog = this;
	CDialogEx::OnShowWindow(bShow, nStatus);
	PostMessage(WM_COMMAND, ID_DOPROGRESS);
}

void CProgressBar::DoProgress()
{
	if ((Pos=Condit())!=-1)
	{
		m_wndProgressBar->SetPos(Pos);
		return;
	}
	KillTimer(ID_TIMER_DOPROGRESS);
	static_cast<CMainFrame*>(CDialogEx::GetParent())->GetStatusBar()->SetPaneText(ID_SEPARATOR, IsInChinese() ? L"就绪" : L"Ready");
	theApp.RunningDialog = nullptr;
	EndDialog(true);
}

void CProgressBar::DoDataExchange(CDataExchange * pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CProgressBar::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == ID_TIMER_DOPROGRESS)
	{
		SendMessage(WM_COMMAND, ID_DOPROGRESS);
	}
	else
		CDialogEx::OnTimer(nIDEvent);
}

BEGIN_MESSAGE_MAP(CProgressBar, CDialogEx)
	ON_MESSAGE(WM_INITDIALOG, &CProgressBar::OnInitDialog)
	ON_COMMAND(ID_DOPROGRESS, &CProgressBar::DoProgress)
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CLaunchDialog, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_INITDIALOG, &CLaunchDialog::OnInitDialog)
END_MESSAGE_MAP()


HBRUSH CLaunchDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(RGB(255, 255, 255)); //设置字体颜色  
		pDC->SetBkMode(TRANSPARENT);      //设置背景透明  
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}

LRESULT CLaunchDialog::OnInitDialog(WPARAM wParam, LPARAM lParam)
{
	LRESULT bRet = HandleInitDialog(wParam, lParam);
	SetBackgroundColor(RGB(50, 50, 50));
	m_pStatic = static_cast<CStatic*>(GetDlgItem(IDC_LAUNCHTEXT));
	m_pStatic->SetWindowTextW(L"FernFlowerUI");
	if (m_Font.CreatePointFont(375, L"Microsoft YaHei"))
	{
		m_pStatic->SetFont(&m_Font);
	}
	HICON hIcon = theApp.LoadIcon(IDR_MAINFRAME);
	if (hIcon)
	{
		SetIcon(hIcon, false);
		SetIcon(hIcon, true);
	}
	return bRet;
}

void COpenFileDialog::OnCheckButtonToggled(DWORD dwIDCtl, BOOL bChecked)
{
	if (dwIDCtl == ID_CLASSES_INSIDE_JAR)
	{
		if (bChecked)
		{
			theApp.bDecompInsideJar = bChecked;
		}
		else
		{
			theApp.bDecompInsideJar = bChecked;
		}
	}
}

void CommonWrapper::COpenFileDialog::OnFolderChange()
{
	if (m_strRootDir != L"")
	{
		CString CurrPath = GetFolderPath();
		if ((CurrPath.GetLength() < m_strRootDir.GetLength()) || (CurrPath.Left(m_strRootDir.GetLength()) != m_strRootDir))
		{
			ITEMIDLIST * pidl;
			theApp.GetShellManager()->ItemFromPath(m_strRootDir, pidl);
			CComPtr<IShellItem> pItem;
			SHCreateItemFromIDList(pidl, IID_PPV_ARGS(&pItem));
			theApp.GetShellManager()->FreeItem(pidl);
			GetIFileOpenDialog()->SetFolder(pItem);
		}
	}
}

void CommonWrapper::COpenFileDialog::OnTypeChange()
{
	if (m_ofn.nFilterIndex == 1)
	{
		SetControlState(ID_CLASSES_INSIDE_JAR, CDCS_ENABLEDVISIBLE);
	}
	else
	{
		HideControl(ID_CLASSES_INSIDE_JAR);
	}
	/*HRESULT hr;
	IFileDialog * pDlg = GetIFileOpenDialog();
	CComPtr<IFileOpenDialog> pDialog;
	pDlg->QueryInterface(&pDialog);
	if (m_ofn.nFilterIndex == 3)
	{
		DWORD Option;
		VERIFY(SUCCEEDED(hr = pDialog->GetOptions(&Option)));
		VERIFY(SUCCEEDED(hr = pDialog->SetOptions(Option | FOS_ALLOWMULTISELECT)));
	}
	else
	{
		DWORD Option;
		VERIFY(SUCCEEDED(hr = pDialog->GetOptions(&Option)));
		Option &= ~FOS_ALLOWMULTISELECT;
		VERIFY(SUCCEEDED(hr = pDialog->SetOptions(Option)));
	}*/
}
