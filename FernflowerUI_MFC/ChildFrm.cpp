
// ChildFrm.cpp: CChildFrame 类的实现
//

#include "stdafx.h"
#include "FernflowerUI_MFC.h"
#include "FernflowerUI_MFCView.h"
#include "CommonWrapper.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
	ON_WM_CREATE()
	ON_WM_MDIACTIVATE()
	ON_UPDATE_COMMAND_UI(ID_ENABLE_RECOVERY, &CChildFrame::OnUpdateEnableRecovery)
	ON_COMMAND(ID_ENABLE_RECOVERY, &CChildFrame::OnEnableRecovery)
	ON_COMMAND(ID_SELECT_FONT, &CChildFrame::OnSelectFont)
	ON_COMMAND(ID_QUICK_DECOMP, &CChildFrame::OnQuickDecomp)
	ON_UPDATE_COMMAND_UI(ID_QUICK_DECOMP, &CChildFrame::OnUpdateQuickDecomp)
	ON_COMMAND(ID_CLEAR_CACHE, &CChildFrame::OnClearCache)
	ON_UPDATE_COMMAND_UI(ID_CLEAR_CACHE, &CChildFrame::OnUpdateClearCache)
	ON_WM_DROPFILES()
	ON_REGISTERED_MESSAGE(WM_FINDSTRING,&CChildFrame::OnFindString)
	ON_COMMAND(ID_EDIT_FIND, &CChildFrame::OnEditFind)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, &CChildFrame::OnUpdateEditFind)
END_MESSAGE_MAP()

// CChildFrame 构造/析构

CChildFrame::CChildFrame():m_pFindDialog(nullptr),IsDialogTransparented(FALSE)
{
	EnableActiveAccessibility();
	m_pFindDialog = nullptr;
	// TODO: 在此添加成员初始化代码
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	return CMDIChildWndEx::OnCreateClient(lpcs, pContext);
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改 CREATESTRUCT cs 来修改窗口类或样式
	if (!CMDIChildWndEx::PreCreateWindow(cs))
		return FALSE;

	return TRUE;
}

// CChildFrame 诊断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 消息处理程序


void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_strTitle != L"")
	{
		SetWindowText(m_strTitle);
		if (theApp.Md5ofFile != L"")
		{
			if (theApp.Flag == CFernflowerUIMFCApp::DecompFlags::DecompJar)
			{
				CommonWrapper::GetMainFrame()->SetWindowText(theApp.JarFilePath + L" => " + m_strTitle + _T(" - FernFlowerUI"));
			}
			else
			{
				CommonWrapper::GetMainFrame()->SetWindowText(m_strTitle + _T(" - FernFlowerUI"));
			}
		}
	}
	else
	{
		CommonWrapper::GetMainFrame()->SetWindowText(L"FernFlowerUI");
	}
	//Calling base class' method is useless.
	//CMDIChildWndEx::OnUpdateFrameTitle(bAddToTitle);
}

static std::atomic_bool bSearchingAllWnds;
static std::atomic_bool bAbortSearch, FinishedWork = true;//Set to true to prevent dead loop in the first call of UpdateComboBox()

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	if (CommonWrapper::GetMainFrame()->m_pActiveFindDialog)
	{
		//CommonWrapper::GetMainFrame()->m_pActiveFindDialog->cntClassesSearched = 0;
		if ((!FinishedWork) && (bSearchingAllWnds))
		{
			//#error "Add some shutting down code here"
			bAbortSearch = true;
			CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []()->bool {return (bool)FinishedWork; }, 10,
				IsInChinese() ? L"正在关闭..." : L"Closing...");
			WaitDlg.DoModal();
			bAbortSearch = false;
			//	FinishedWork = false;
			CommonWrapper::GetMainFrame()->m_pActiveFindDialog->SearchingMultiDoc = false;
		}
	}

	return 0;
}


void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd)
{
	CMDIChildWndEx::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	// TODO: 在此处添加消息处理程序代码
	if ((pDeactivateWnd == this) && (m_pFindDialog))
	{
		m_pFindDialog->ShowWindow(SW_HIDE);
	}
	if ((pActivateWnd == this) && (m_pFindDialog))
	{
		m_pFindDialog->ShowWindow(SW_SHOW);
	}
	OnUpdateFrameTitle(true);
}


void CChildFrame::OnUpdateEnableRecovery(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EnableRecovery,0)!=WAIT_TIMEOUT);
}


void CChildFrame::OnEnableRecovery()
{
	// TODO: 在此添加命令处理程序代码
	if (WaitForSingleObject(theApp.EnableRecovery, 0) != WAIT_TIMEOUT)
	{
		ResetEvent(theApp.EnableRecovery);
	}
	else
	{
		SetEvent(theApp.EnableRecovery);
	}
}



BOOL CChildFrame::DestroyWindow()
{
	//insert worker-shutdown code here
	//to prevent data races
	if (!m_pFindDialog)
	{
		if ((!FinishedWork)&&(bSearchingAllWnds))
		{
			//#error "Add some shutting down code here"
			bAbortSearch = true;
			CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []() {return (bool)FinishedWork; }, 10,
				IsInChinese() ? L"正在关闭..." : L"Closing...");
			WaitDlg.DoModal();
			bAbortSearch = false;
			//	FinishedWork = false;
			if (CommonWrapper::GetMainFrame()->m_pActiveFindDialog)
				CommonWrapper::GetMainFrame()->m_pActiveFindDialog->SearchingMultiDoc = false;
		}
	}
	else
	{
		if (!FinishedWork)
		{
			//#error "Add some shutting down code here"
			bAbortSearch = true;
			CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []() {return (bool)FinishedWork; }, 10,
				IsInChinese() ? L"正在关闭..." : L"Closing...");
			WaitDlg.DoModal();
			bAbortSearch = false;
			//	FinishedWork = false;
		}
		m_pFindDialog->SearchingMultiDoc = false;
		m_pFindDialog->rEditHelper.DestroyWindow();
	}

	for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
	{
		if (it->second==this)
		{
			CommonWrapper::GetMainFrame()->m_MDIChildWndMap.erase(it);
			break;
		}
	}
	if (this == CommonWrapper::GetMainFrame()->m_pStartPage)
	{
		CommonWrapper::GetMainFrame()->m_pStartPage = nullptr;
	}

	// TODO: 在此处添加消息处理程序代码
	//if (CommonWrapper::GetMainFrame()->m_pActiveFindDialog)
	//	CommonWrapper::GetMainFrame()->m_pActiveFindDialog->SearchingMultiDoc = false;

	return CMDIChildWndEx::DestroyWindow();
}


void CChildFrame::OnSelectFont()
{
	// TODO: 在此添加命令处理程序代码
	CHARFORMAT2 Format;
	static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetDefaultCharFormat(Format);
	CFontDialog FontSelect(Format);
	if (FontSelect.DoModal() == IDOK)
	{
		lstrcpy(Format.szFaceName, theApp.FontFaceName = FontSelect.GetFaceName());
		theApp.FontSize = (Format.yHeight = FontSelect.GetSize() * 2) / 20;
		Format.dwEffects = Format.dwEffects&(~CFE_BOLD);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(0, -1);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetDefaultCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSelectionCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.GetDefaultCharFormat(Format);
//		lstrcpy(Format.szFaceName, theApp.FontFaceName = FontSelect.GetFaceName());
//		theApp.FontSize = (Format.yHeight = FontSelect.GetSize() * 2) / 20;
		Format.dwEffects = Format.dwEffects&(~CFE_BOLD);
		//Format.yHeight -= 17;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.SetSel(0, -1);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.SetSelectionCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.m_wndLineBox.SetDefaultCharFormat(Format);
		CFernflowerUIMFCView * pView = static_cast<CFernflowerUIMFCView*>(GetActiveView());
		pView->FinishHighLight = false;
		AfxGetMainWnd()->BeginWaitCursor();
		CStringW Contact;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetWindowTextW(Contact);
		std::future<void> SetHighLight = std::async(std::launch::async,[&](const CStringW & Str, CWnd * MainWnd) {
			pView->SetViewText(Str);
		}, Contact, AfxGetMainWnd());
		CommonWrapper::CProgressBar Progress(AfxGetMainWnd(),
			[&]()->int {AfxGetMainWnd()->RestoreWaitCursor(); return pView->FinishHighLight; },
			5, IsInChinese()?L"正在设置字体":L"Setting the new font", 0, 77);
		Progress.DoModal();
		pView->FinishHighLight = false;
		AfxGetMainWnd()->EndWaitCursor();
	}
}


void CChildFrame::OnQuickDecomp()
{
	// TODO: 在此添加命令处理程序代码
	theApp.IsQuickDecomp = !theApp.IsQuickDecomp;
}


void CChildFrame::OnUpdateQuickDecomp(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.IsQuickDecomp);
}


void CChildFrame::OnClearCache()
{
	// TODO: 在此添加命令处理程序代码
	if (WaitForSingleObject(theApp.EvtIsToRmCache, 0) != WAIT_TIMEOUT)
	{
		ResetEvent(theApp.EvtIsToRmCache);
	}
	else
	{
		SetEvent(theApp.EvtIsToRmCache);
	}
	CommonWrapper::GetMainFrame()->OnViewCaptionBar();
	OnUpdateFrameTitle(FALSE);
}


void CChildFrame::OnUpdateClearCache(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(WaitForSingleObject(theApp.EvtIsToRmCache, 0) != WAIT_TIMEOUT);
}


void CChildFrame::OnDropFiles(HDROP DropFile)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int DropCount = DragQueryFile(DropFile, -1, NULL, 0);//取得被拖动文件的数目  
	if (DropCount == 1)
	{
		WCHAR wcStr[MAX_PATH];
		DragQueryFile(DropFile, 0, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名 
		theApp.DoDecomplie(wcStr);
	}
	else
	{
		WCHAR ThisPath[MAX_PATH];
		GetModuleFileName(nullptr, ThisPath, MAX_PATH);
		for (int i = 1; i < DropCount; i++)
		{
			WCHAR wcStr[MAX_PATH];
			DragQueryFile(DropFile, i, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名  
			ShellExecute(nullptr, L"open", ThisPath, wcStr, nullptr, SW_SHOW);
		}
		WCHAR wcStr[MAX_PATH];
		DragQueryFile(DropFile, 0, wcStr, MAX_PATH);//获得拖曳的第i个文件的文件名  
		theApp.DoDecomplie(wcStr);
	}
	DragFinish(DropFile);
}

LRESULT CChildFrame::OnFindString(WPARAM wParam, LPARAM lParam)
{
	auto Available = [](const FINDTEXTEX & Fnd)
	{return !((Fnd.chrgText.cpMin == -1) && (Fnd.chrgText.cpMax == -1)); };
	FINDREPLACE * pFind = reinterpret_cast<LPFINDREPLACE>(lParam);
	if (!m_pFindDialog)
	{
		return 0;
	}
	if (m_pFindDialog->IsTerminating())
	{
		CHARRANGE TempRange;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(TempRange);
		int FirstVisible = static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetFirstVisibleLine();
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.LockWindowUpdate();
		m_pFindDialog->ShowWindow(SW_HIDE);
		CHARFORMAT2 Format;
		Format.dwMask = CFM_BACKCOLOR;
		Format.dwEffects |= CFE_AUTOBACKCOLOR;
		Format.crBackColor = COLOR_WINDOW;
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(m_pFindDialog->SelectedArea);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSelectionCharFormat(Format);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(TempRange);
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.LineScroll(FirstVisible - static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetFirstVisibleLine());
		static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.UnlockWindowUpdate();
		m_pFindDialog->rEditHelper.DestroyWindow();
		m_pFindDialog = nullptr;
		CommonWrapper::GetMainFrame()->m_pActiveFindDialog = nullptr;
		return 0;
	}
	if (m_pFindDialog->FindNext())
	{
		VERIFY(m_pFindDialog->m_pComboBox = static_cast<CFindComboBox*>(m_pFindDialog->GetDlgItem(IDC_CHOOSEAREA)));
		CFindDialog::SearchArea nArea = m_pFindDialog->m_vecComboList[m_pFindDialog->m_pComboBox->GetCurSel()];
		if (nArea!=CFindDialog::sCurrentSelect)
		{
			CHARRANGE TempRange;
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(TempRange);
			int FirstVisible = static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetFirstVisibleLine();
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.LockWindowUpdate();
			CHARFORMAT2 Format;
			Format.dwMask = CFM_BACKCOLOR;
			Format.dwEffects |= CFE_AUTOBACKCOLOR;
			Format.crBackColor = COLOR_WINDOW;
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(m_pFindDialog->SelectedArea);
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSelectionCharFormat(Format);
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(TempRange);
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.LineScroll(FirstVisible - static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetFirstVisibleLine());
			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.UnlockWindowUpdate();
		}
		switch (nArea)
		{
		case CFindDialog::sCurrentBlock:
			m_pFindDialog->SearchBegin = m_pFindDialog->m_rCurrBlock.cpMin;
			m_pFindDialog->SearchEnd = m_pFindDialog->m_rCurrBlock.cpMax;
		case CFindDialog::sCurrentDocument:
			{
				//m_pFindDialog->bIsSearching = true;
				FINDTEXTEX Find;
				DWORD dwFlags = 0;
				long nStart, nEnd;
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(nStart, nEnd);
				if (m_pFindDialog->SearchDown())
				{
					Find.chrg.cpMin = (nEnd - nStart) ? nStart + 1 : nEnd;
					Find.chrg.cpMax = m_pFindDialog->SearchEnd;
					dwFlags = FR_DOWN;
				}
				else
				{
					Find.chrg.cpMin = (nEnd - nStart) ? nEnd - 1 : nStart;
					Find.chrg.cpMax = m_pFindDialog->SearchBegin;
				}
				if (m_pFindDialog->MatchCase())
				{
					dwFlags |= FR_MATCHCASE;
				}
				if (m_pFindDialog->MatchWholeWord())
				{
					dwFlags |= FR_WHOLEWORD;
				}
				CString Str = m_pFindDialog->GetFindString();
				Find.lpstrText = Str;
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.FindText(dwFlags, &Find);
				if (!Available(Find))
				{
					MessageBox(IsInChinese() ? L"该文档中找不到其他搜索项!" : L"Could not find other results in the document!",
						IsInChinese() ? L"搜索完毕" : L"Done", MB_ICONINFORMATION);
					//m_pFindDialog->bIsSearching = false;
				}
				else
				{
					static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(Find.chrgText);
				}
			}
			break;
		case CFindDialog::sCurrentSelect:
			/*if (m_pFindDialog->SelectedArea.cpMin == 0 && m_pFindDialog->SelectedArea.cpMax == 0)
			{
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(m_pFindDialog->SelectedArea);
				CHARFORMAT2 Format;
				Format.dwMask = CFM_BACKCOLOR;
				Format.crBackColor = RGB(174, 238, 238);
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSelectionCharFormat(Format);
				m_pFindDialog->SearchBegin = m_pFindDialog->SelectedArea.cpMin;
				m_pFindDialog->SearchEnd = m_pFindDialog->SelectedArea.cpMax;
			}*/
			{
				m_pFindDialog->bIsSearching = true;
				if (((m_pFindDialog->SearchBegin*m_pFindDialog->SearchEnd) == 0)||m_pFindDialog->IsOptionChanged())
				{
					m_pFindDialog->SearchBegin = m_pFindDialog->SelectedArea.cpMin;
					m_pFindDialog->SearchEnd = m_pFindDialog->SelectedArea.cpMax;
					m_pFindDialog->SetPrevOptions();
				}
				
				m_pFindDialog->SetPrevOptions();
				FINDTEXTEX Find;
				DWORD dwFlags = 0;
				long nStart, nEnd;
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(nStart, nEnd);
				//nStart = m_pFindDialog->SearchBegin;
				if (m_pFindDialog->SearchDown())
				{
					Find.chrg.cpMin = nStart;
					Find.chrg.cpMax = m_pFindDialog->SearchEnd;
					dwFlags = FR_DOWN;
				}
				else
				{
					Find.chrg.cpMin = nEnd;
					Find.chrg.cpMax = m_pFindDialog->SearchBegin;
				}
				if (m_pFindDialog->MatchCase())
				{
					dwFlags |= FR_MATCHCASE;
				}
				if (m_pFindDialog->MatchWholeWord())
				{
					dwFlags |= FR_WHOLEWORD;
				}
				CString Str = m_pFindDialog->GetFindString();
				Find.lpstrText = Str;
				static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.FindText(dwFlags, &Find);
				if (!Available(Find))
				{
					MessageBox(IsInChinese() ? L"该区域中找不到其他搜索项!" : L"Could not find other results in the region!",
						IsInChinese() ? L"搜索完毕" : L"Done", MB_ICONINFORMATION);
					m_pFindDialog->bIsSearching = false;
					m_pFindDialog->SetFocus();
				}
				else
				{
					static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetSel(Find.chrgText);
				}
			}
			break;
		case CFindDialog::sAllDocument:
		{
			if (m_pFindDialog->IsOptionChanged())
			{
				if (!FinishedWork)
				{
					//#error "Add some shutting down code here"
					bAbortSearch = true;
					CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []() {return (bool)FinishedWork; }, 10,
						IsInChinese() ? L"正在准备..." : L"Setting up...");
					WaitDlg.DoModal();
					bAbortSearch = false;
					//FinishedWork = false;
				}
				m_pFindDialog->SetPrevOptions();
				m_pFindDialog->SearchingMultiDoc = false;
			}
			if (!m_pFindDialog->SearchingMultiDoc)
			{
				m_pFindDialog->bIsSearching = true;
				m_pFindDialog->SetPrevOptions();
				bSearchingAllWnds = true;
				m_pFindDialog->m_mapSearchResult.clear();
				m_pFindDialog->m_classNameQueue.clear();
				m_pFindDialog->cntResultsShown = 0;
				auto SearchProc = [Available](CFindDialog::SearchOptions PrevOptions,CFindDialog * m_pFindDialog,CChildFrame * pThisWnd) {
					const CObList& TabList = CommonWrapper::GetMainFrame()->GetMDITabGroups();
					CObList WndList;
					{
						POSITION Pos = TabList.GetHeadPosition();
						while (Pos)
						{
							const CObject * pObjTmp = TabList.GetAt(Pos);
							CObject * pObj = const_cast<CObject*>(pObjTmp);
							CMFCTabCtrl * pTabCtrl = static_cast<CMFCTabCtrl*>(pObj);
							for (int i = 0; i < pTabCtrl->GetTabsNum(); i++)
							{
								if (static_cast<CChildFrame*>(pTabCtrl->GetTabWnd(i))->m_strPath != L"")
									WndList.AddTail(pTabCtrl->GetTabWnd(i));
							}
							TabList.GetNext(Pos);
						}
					}
					int cntClassesSearched = 0;
					POSITION Pos = WndList.Find(pThisWnd);
					while (cntClassesSearched < WndList.GetCount())
					{
						const CChildFrame * Tempptr = static_cast<const CChildFrame*>(WndList.GetAt(Pos));
						CChildFrame * pWnd = const_cast<CChildFrame*>(Tempptr);
						CLineNumEdit & rEdit = static_cast<CFernflowerUIMFCView*>(pWnd->GetActiveView())->m_wndEdit;
						CString className;
						{
							//std::lock_guard<std::mutex> locker(CommonWrapper::GetMainFrame()->m_mtxChildWndMap);
							for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
							{
								if (it->second == pWnd)
								{
									className = it->first;
									break;
								}
							}
							{
								std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
								m_pFindDialog->m_classNameQueue.push_back(className);
							}
						}

						FINDTEXTEX Find = { 0 };
						long nOrigStart, nOrigEnd;
						rEdit.GetSel(nOrigStart, nOrigEnd);
						long nOriginal = PrevOptions.bSearchDown ? nOrigStart : nOrigEnd;
						unsigned int nStartPos = 0;
						Find.chrg.cpMin = 0;
						Find.chrg.cpMax = -1;
						while (Available(Find))
						{
							if (bAbortSearch)
							{
								FinishedWork = true;
								m_pFindDialog->bIsSearching = false;
								return;
							}
							DWORD dwFlags = FR_DOWN;
							long nStart, nEnd;
							rEdit.GetSel(nStart, nEnd);
							if (PrevOptions.bMatchCase)
							{
								dwFlags |= FR_MATCHCASE;
							}
							if (PrevOptions.bWholeWord)
							{
								dwFlags |= FR_WHOLEWORD;
							}
							CString Str = m_pFindDialog->GetFindString();
							Find.lpstrText = Str;
							rEdit.FindText(dwFlags, &Find);
							Find.chrg.cpMin = Find.chrgText.cpMin + 1;
							if (Available(Find))
							{
								if (Find.chrgText.cpMin < nOriginal)
								{
									nStartPos = m_pFindDialog->m_mapSearchResult[className].size();
								}
								{
									std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
									m_pFindDialog->m_mapSearchResult[className].push_back(Find.chrgText);
								}
							}
						}
						if (!PrevOptions.bSearchDown)
						{
							nStartPos = (nStartPos > 0) ? nStartPos - 1 : m_pFindDialog->m_mapSearchResult[className].size() - 1;
						}
						m_pFindDialog->m_mapResultStartPos[className] = nStartPos;
						cntClassesSearched++;
						{
							if (PrevOptions.bSearchDown)
								WndList.GetNext(Pos);
							else WndList.GetPrev(Pos);
						}
						if (Pos == nullptr)
						{
							if (PrevOptions.bSearchDown)
								Pos = WndList.GetHeadPosition();
							else
							{
								Pos = WndList.GetTailPosition();
								WndList.GetPrev(Pos);
							}
						}
					}
					FinishedWork = true;
					bSearchingAllWnds = false;
				};
				FinishedWork = false;
				m_pFindDialog->SearchingMultiDoc = true;
				std::thread Search_Worker = std::thread(SearchProc, m_pFindDialog->PrevOptions,this->m_pFindDialog,this);
				Search_Worker.detach();
				{
					std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
					m_pFindDialog->nqueCurr = 0;
				}
			}
			auto LookUpProc = [this]()->void {
				while (true)
				{
					{
						std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
						if (FinishedWork || (m_pFindDialog->m_classNameQueue.size() > m_pFindDialog->nqueCurr + 1))
							break;
					}
					Sleep(500);
				}
			};
			std::future<void> Looker = std::async(std::launch::async, LookUpProc);
			auto WaiterProc = [&]() { return Looker.wait_for(std::chrono::milliseconds(0)) != std::future_status::timeout; };
			CommonWrapper::CWaitDlg Wait(CommonWrapper::GetMainFrame(), WaiterProc, 5, IsInChinese() ? L"正在查找..." : L"Searching...");
			Wait.DoModal();
			{
				std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
				if (m_pFindDialog->m_classNameQueue.size() == m_pFindDialog->nqueCurr)
				{
					MessageBox(IsInChinese() ? L"该区域中找不到其他搜索项!" : L"Could not find other results in the region!",
						IsInChinese() ? L"搜索完毕" : L"Done", MB_ICONINFORMATION);
					m_pFindDialog->bIsSearching = false;
					m_pFindDialog->SetFocus();
					return 0;
				}
				unsigned thePosition;
				if (m_pFindDialog->PrevOptions.bSearchDown)
				{
					thePosition = m_pFindDialog->cntResultsShown + m_pFindDialog->m_mapResultStartPos[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]];
					if (thePosition >= m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size())
					{
						thePosition = thePosition - m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size();
					}
				}
				else
				{
					thePosition = m_pFindDialog->m_mapResultStartPos[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]] - m_pFindDialog->cntResultsShown;
					if (int(thePosition)<0)
					{
						thePosition = m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size() + thePosition;
						//thePosition -= 2;//I still don't know why
					}
				}
				//CommonWrapper::GetMainFrame()->m_MDIChildWndMap[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]->MDIActivate();
				CViewTree::OpenJavaFile(m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr], CommonWrapper::GetMainFrame()->m_wndClassView.m_mapClassFile[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]);
				static_cast<CFernflowerUIMFCView*>(CommonWrapper::GetMainFrame()->m_MDIChildWndMap[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]->GetActiveView())->
					m_wndEdit.SetSel(m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]][thePosition]);
				CFindDialog * pFind = m_pFindDialog;
				pFind->TransferOwner(static_cast<CChildFrame*>(CommonWrapper::GetMainFrame()->m_MDIChildWndMap[pFind->m_classNameQueue[pFind->nqueCurr]]));
				if (pFind->m_mapSearchResult[pFind->m_classNameQueue[pFind->nqueCurr]].size() > pFind->cntResultsShown + 1)
				{
					pFind->cntResultsShown++;
				}
				else
				{
					pFind->nqueCurr++;
					pFind->cntResultsShown = 0;
				}
				pFind->ShowWindow(SW_SHOW);
				pFind->SetFocus();
			}
		}
		break;
		case CFindDialog::sCurrentPackage:
		case CFindDialog::sCurrentJar:
		{
			if (m_pFindDialog->IsOptionChanged())
			{
				if (!FinishedWork)
				{
					//#error "Add some shutting down code here"
					bAbortSearch = true;
					CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []() {return (bool)FinishedWork; }, 10,
						IsInChinese() ? L"正在准备..." : L"Setting up...");
					WaitDlg.DoModal();
					bAbortSearch = false;
					//FinishedWork = false;
				}
				m_pFindDialog->SetPrevOptions();
				m_pFindDialog->SearchingMultiDoc = false;
			}
			if (!m_pFindDialog->SearchingMultiDoc)
			{
				m_pFindDialog->bIsSearching = true;
				bSearchingAllWnds = true;
				m_pFindDialog->m_mapSearchResult.clear();
				m_pFindDialog->m_classNameQueue.clear();
				m_pFindDialog->cntResultsShown = 0;
				CString FindPath;
				if (nArea == CFindDialog::sCurrentPackage)
				{
					for (const auto & pair : CommonWrapper::GetMainFrame()->m_MDIChildWndMap)
					{
						if (pair.second == this)
						{
							FindPath = pair.first;
							break;
						}
					}
					VERIFY(FindPath != L"");
					FindPath.Delete(FindPath.ReverseFind(L'\\'), FindPath.GetLength() - FindPath.ReverseFind(L'\\'));
					FindPath += L"\\*.java";
					CFileFind Find;
					bool b = Find.FindFile(FindPath);
					while (b)
					{
						b = Find.FindNextFile();
						if (!Find.IsDots())
						{
							CString strJava = Find.GetFilePath();
							m_pFindDialog->listFiles.push_back(strJava);
						}
					}
					Find.Close();
				}
				else
				{
					m_pFindDialog->listFiles = CommonWrapper::GetMainFrame()->m_wndClassView.lstjavaFileName;
				}
				auto SearchProc = [Available](CFindDialog::SearchOptions PrevOptions,CFindDialog * m_pFindDialog,CChildFrame * pThisWnd) {
					CStringList listUnopenedFile;
					for (const auto & x : m_pFindDialog->listFiles)
					{
						listUnopenedFile.AddTail(x);
					}
					CList<CChildFrame*> listOpened;
					
					const CObList& TabList = CommonWrapper::GetMainFrame()->GetMDITabGroups();
					CObList RawWndList;
					{
						POSITION Pos = TabList.GetHeadPosition();
						while (Pos)
						{
							const CObject * pObjTmp = TabList.GetAt(Pos);
							CObject * pObj = const_cast<CObject*>(pObjTmp);
							CMFCTabCtrl * pTabCtrl = static_cast<CMFCTabCtrl*>(pObj);
							for (int i = 0; i < pTabCtrl->GetTabsNum(); i++)
							{
								if (static_cast<CChildFrame*>(pTabCtrl->GetTabWnd(i))->m_strPath != L"")
									RawWndList.AddTail(pTabCtrl->GetTabWnd(i));
							}
							TabList.GetNext(Pos);
						}
					}
					{
						POSITION Pos = RawWndList.Find(pThisWnd);
						int cntWnd = 0;
						VERIFY(Pos);
						while (cntWnd<RawWndList.GetCount())
						{
							POSITION posFound;
							if (posFound = listUnopenedFile.Find(static_cast<CChildFrame*>(RawWndList.GetAt(Pos))->m_strPath))
							{
								listUnopenedFile.RemoveAt(posFound);
								listOpened.AddTail(static_cast<CChildFrame*>(RawWndList.GetAt(Pos)));
							}
							cntWnd++;
							if (PrevOptions.bSearchDown)
							{
								RawWndList.GetNext(Pos);
								if (Pos == nullptr)
								{
									Pos = RawWndList.GetHeadPosition();
								}
							}
							else
							{
								RawWndList.GetPrev(Pos);
								if (Pos == nullptr)
								{
									Pos = RawWndList.GetTailPosition();
									RawWndList.GetPrev(Pos);
								}
							}
						}
					}
					int cntClassesSearched = 0;
					POSITION Pos = listOpened.Find(pThisWnd);
					while (cntClassesSearched < listOpened.GetCount())
					{
						const CChildFrame * Tempptr = static_cast<const CChildFrame*>(listOpened.GetAt(Pos));
						CChildFrame * pWnd = const_cast<CChildFrame*>(Tempptr);
						CLineNumEdit & rEdit = static_cast<CFernflowerUIMFCView*>(pWnd->GetActiveView())->m_wndEdit;
						CString className;
						{
							//std::lock_guard<std::mutex> locker(CommonWrapper::GetMainFrame()->m_mtxChildWndMap);
							for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
							{
								if (it->second == pWnd)
								{
									className = it->first;
									break;
								}
							}
							{
								std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
								m_pFindDialog->m_classNameQueue.push_back(className);
							}
						}

						FINDTEXTEX Find = { 0 };
						long nOrigStart, nOrigEnd;
						rEdit.GetSel(nOrigStart, nOrigEnd);
						long nOriginal = PrevOptions.bSearchDown ? nOrigStart : nOrigEnd;
						unsigned int nStartPos = 0;
						Find.chrg.cpMin = 0;
						Find.chrg.cpMax = -1;
						while (Available(Find))
						{
							if (bAbortSearch)
							{
								FinishedWork = true;
								m_pFindDialog->bIsSearching = false;
								return;
							}
							DWORD dwFlags = FR_DOWN;
							long nStart, nEnd;
							rEdit.GetSel(nStart, nEnd);
							if (PrevOptions.bMatchCase)
							{
								dwFlags |= FR_MATCHCASE;
							}
							if (PrevOptions.bWholeWord)
							{
								dwFlags |= FR_WHOLEWORD;
							}
							CString Str = m_pFindDialog->GetFindString();
							Find.lpstrText = Str;
							rEdit.FindText(dwFlags, &Find);
							Find.chrg.cpMin = Find.chrgText.cpMin + 1;
							if (Available(Find))
							{
								if (Find.chrgText.cpMin < nOriginal)
								{
									nStartPos = m_pFindDialog->m_mapSearchResult[className].size();
								}
								{
									std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
									m_pFindDialog->m_mapSearchResult[className].push_back(Find.chrgText);
								}
							}
						}
						if (!PrevOptions.bSearchDown)
						{
							nStartPos = (nStartPos > 0) ? nStartPos - 1 : m_pFindDialog->m_mapSearchResult[className].size() - 1;
						}
						m_pFindDialog->m_mapResultStartPos[className] = nStartPos;
						cntClassesSearched++;
						{
							if (PrevOptions.bSearchDown)
								listOpened.GetNext(Pos);
							else listOpened.GetPrev(Pos);
						}
						if (Pos == nullptr)
						{
							if (PrevOptions.bSearchDown)
								Pos = listOpened.GetHeadPosition();
							else 
							{
								Pos = listOpened.GetTailPosition();
								listOpened.GetPrev(Pos);
							}
						}
					}

					///<summary> This is very important!!! </summary>
					bSearchingAllWnds = false;

					cntClassesSearched = 0;
					Pos = listUnopenedFile.GetHeadPosition();
					while (cntClassesSearched < listUnopenedFile.GetCount())
					{
						CString className = listUnopenedFile.GetAt(Pos);
						CFile File;
						File.Open(className, CStdioFile::modeNoTruncate | CStdioFile::modeRead | CStdioFile::shareDenyNone);
						size_t Len = File.GetLength();
						char * buf = new char[Len + 1];
						memset(buf, 0, Len + 1);
						File.Read(buf, Len);
						for (size_t i = 0; i < Len; i++)
						{
							if (buf[i] == '\0')
							{
								buf[i] = ' ';
							}
						}
						buf[Len] = '\0';
						int nConLen = MultiByteToWideChar(CP_UTF8, 0, buf, -1, nullptr, 0);
						wchar_t * wbuf = new wchar_t[nConLen + 1]{ 0 };
						MultiByteToWideChar(CP_UTF8, 0, buf, Len, wbuf, nConLen);
						delete buf;
						CStringW str = wbuf;
						delete wbuf;
						BOOL b = ::SetWindowText(m_pFindDialog->rEditHelper.m_hWnd, str);
						{
							std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
							m_pFindDialog->m_classNameQueue.push_back(className);
						}
						
						FINDTEXTEX Find = { 0 };
						long nOrigStart, nOrigEnd;
						m_pFindDialog->rEditHelper.GetSel(nOrigStart, nOrigEnd);
						long nOriginal = PrevOptions.bSearchDown ? nOrigStart : nOrigEnd;
						unsigned int nStartPos = 0;
						Find.chrg.cpMin = 0;
						Find.chrg.cpMax = -1;
						while (Available(Find))
						{
							if (bAbortSearch)
							{
								FinishedWork = true;
								m_pFindDialog->bIsSearching = false;
								return;
							}
							DWORD dwFlags = FR_DOWN;
							long nStart, nEnd;
							m_pFindDialog->rEditHelper.GetSel(nStart, nEnd);
							if (PrevOptions.bMatchCase)
							{
								dwFlags |= FR_MATCHCASE;
							}
							if (PrevOptions.bWholeWord)
							{
								dwFlags |= FR_WHOLEWORD;
							}
							CString Str = m_pFindDialog->GetFindString();
							Find.lpstrText = Str;
							m_pFindDialog->rEditHelper.FindText(dwFlags, &Find);
							Find.chrg.cpMin = Find.chrgText.cpMin + 1;
							if (Available(Find))
							{
								if (Find.chrgText.cpMin < nOriginal)
								{
									nStartPos = m_pFindDialog->m_mapSearchResult[className].size();
								}
								{
									std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
									m_pFindDialog->m_mapSearchResult[className].push_back(Find.chrgText);
								}
							}
						}
						if (!PrevOptions.bSearchDown)
						{
							nStartPos = (nStartPos > 0) ? nStartPos - 1 : m_pFindDialog->m_mapSearchResult[className].size() - 1;
						}
						m_pFindDialog->m_mapResultStartPos[className] = nStartPos;
						cntClassesSearched++;
						{
							if (PrevOptions.bSearchDown)
								listUnopenedFile.GetNext(Pos);
							else listUnopenedFile.GetPrev(Pos);
						}
						if (Pos == nullptr)
						{
							if (PrevOptions.bSearchDown)
								Pos = listUnopenedFile.GetHeadPosition();
							else 
							{
								Pos = listUnopenedFile.GetTailPosition();
								listUnopenedFile.GetPrev(Pos);
							}
						}
					}
					FinishedWork = true;
				};
				FinishedWork = false;
				m_pFindDialog->SearchingMultiDoc = true;
				std::thread Search_Worker = std::thread(SearchProc, m_pFindDialog->PrevOptions,this->m_pFindDialog,this);
				Search_Worker.detach();
				{
					std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
					m_pFindDialog->nqueCurr = 0;
				}
			}
			auto LookUpProc = [this]()->void {
				while (true)
				{
					{
						std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
						if (FinishedWork || (m_pFindDialog->m_classNameQueue.size() > m_pFindDialog->nqueCurr + 1))
							break;
					}
					Sleep(500);
				}
			};
			std::future<void> Looker = std::async(std::launch::async, LookUpProc);
			auto WaiterProc = [&]() { return Looker.wait_for(std::chrono::milliseconds(0)) != std::future_status::timeout; };
			CommonWrapper::CWaitDlg Wait(CommonWrapper::GetMainFrame(), WaiterProc, 5, IsInChinese() ? L"正在查找..." : L"Searching...");
			Wait.DoModal();
			{
				std::lock_guard<std::mutex> lock(m_pFindDialog->MtxDataStorage);
				if (m_pFindDialog->m_classNameQueue.size() == m_pFindDialog->nqueCurr)
				{
					MessageBox(IsInChinese() ? L"该区域中找不到其他搜索项!" : L"Could not find other results in the region!",
						IsInChinese() ? L"搜索完毕" : L"Done", MB_ICONINFORMATION);
					m_pFindDialog->bIsSearching = false;
					m_pFindDialog->SetFocus();
					return 0;
				}
				unsigned nPos;
				if (m_pFindDialog->PrevOptions.bSearchDown)
				{
					nPos = m_pFindDialog->cntResultsShown + m_pFindDialog->m_mapResultStartPos[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]];
					if (nPos >= m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size())
					{
						nPos = nPos - m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size();
					}
				}
				else
				{
					nPos = m_pFindDialog->m_mapResultStartPos[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]] - m_pFindDialog->cntResultsShown;
					if (int(nPos) < 0)
					{
						nPos = m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]].size() + nPos;
					}
				}
				//CommonWrapper::GetMainFrame()->m_MDIChildWndMap[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]->MDIActivate();
				CViewTree::OpenJavaFile(m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr], CommonWrapper::GetMainFrame()->m_wndClassView.m_mapClassFile[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]);
				static_cast<CFernflowerUIMFCView*>(CommonWrapper::GetMainFrame()->m_MDIChildWndMap[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]]->GetActiveView())->
					m_wndEdit.SetSel(m_pFindDialog->m_mapSearchResult[m_pFindDialog->m_classNameQueue[m_pFindDialog->nqueCurr]][nPos]);
				CFindDialog * pFind = m_pFindDialog;
				pFind->TransferOwner(static_cast<CChildFrame*>(CommonWrapper::GetMainFrame()->m_MDIChildWndMap[pFind->m_classNameQueue[pFind->nqueCurr]]));
				if (pFind->m_mapSearchResult[pFind->m_classNameQueue[pFind->nqueCurr]].size() > pFind->cntResultsShown + 1)
				{
					pFind->cntResultsShown++;
				}
				else
				{
					pFind->nqueCurr++;
					pFind->cntResultsShown = 0;
				}
				pFind->ShowWindow(SW_SHOW);
				pFind->SetFocus();
			}
		}
		break;
		}
	}
	return 0;
}

static CHARRANGE Range;

void CChildFrame::OnEditFind()
{
	// TODO: 在此添加命令处理程序代码
	for (auto it = CommonWrapper::GetMainFrame()->m_MDIChildWndMap.begin(); it != CommonWrapper::GetMainFrame()->m_MDIChildWndMap.end(); it++)
	{
		if (static_cast<CChildFrame*>(it->second)->m_pFindDialog!=nullptr&&it->second!=this)
		{
			static_cast<CChildFrame*>(it->second)->m_pFindDialog->DestroyWindow();
			static_cast<CChildFrame*>(it->second)->m_pFindDialog = nullptr;
			CommonWrapper::GetMainFrame()->m_pActiveFindDialog = nullptr;
		}
	}
	if (m_pFindDialog)
	{
		if (static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText().GetLength()==0||static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText().Find(L'\r') != -1)
		{
			m_pFindDialog->UpdateComboBox(true);
			m_pFindDialog->ShowWindow(SW_SHOW);
//			static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetFocus();
			return;
		}
		else
		{
			m_pFindDialog->DestroyWindow();
			m_pFindDialog = nullptr;
			CommonWrapper::GetMainFrame()->m_pActiveFindDialog = nullptr;
		}
	}
	m_pFindDialog = new CFindDialog(this);
	static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSel(Range);
	if (!m_pFindDialog->Create(TRUE, static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.GetSelText(), nullptr, FR_DOWN | FR_ENABLETEMPLATE))
	{
		AfxGetMainWnd()->MessageBox(IsInChinese() ? L"搜索对话框启动失败!" : L"Failed to create the Find Dialog!", IsInChinese() ? L"错误" : L"Error", MB_ICONERROR);
		delete m_pFindDialog;
		m_pFindDialog = nullptr;
		CommonWrapper::GetMainFrame()->m_pActiveFindDialog = nullptr;
		return;
	}
	CommonWrapper::GetMainFrame()->m_pActiveFindDialog = m_pFindDialog;
	CRect Rect;
	m_pFindDialog->GetWindowRect(&Rect);
	CRect ViewRect;
	GetActiveView()->GetWindowRect(&ViewRect);
	m_pFindDialog->SetWindowPos(&wndTop, ViewRect.right - Rect.Width(), ViewRect.top, Rect.Width(), Rect.Height(), SWP_HIDEWINDOW);
	m_pFindDialog->ShowWindow(SW_SHOW);
//	static_cast<CFernflowerUIMFCView*>(GetActiveView())->m_wndEdit.SetFocus();
}


void CChildFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_pFindDialog)
	{
		if (nChar==VK_CONTROL)
		{
			m_pFindDialog->ModifyStyleEx(0, WS_EX_TRANSPARENT | WS_EX_LAYERED);
			m_pFindDialog->SetLayeredWindowAttributes(0, 125, LWA_ALPHA);
		}
		else
		{
			m_pFindDialog->ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
		}
	}
	CMDIChildWndEx::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CChildFrame::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_pFindDialog)
	{
		m_pFindDialog->ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
	}
	CMDIChildWndEx::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CFindDialog::OnInitDialog()
{
	CFindReplaceDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	VERIFY(rEditHelper.Create(WS_CHILD | ES_MULTILINE, CRect(0, 0, 200, 200), CommonWrapper::GetMainFrame(), 0));
	rEditHelper.ShowWindow(SW_HIDE);

	m_pComboBox = new CFindComboBox;
	VERIFY(m_pComboBox->Attach(::GetDlgItem(m_hWnd, IDC_CHOOSEAREA)));


	m_pComboBox->AddString(IsInChinese() ? L"当前文档" : L"Current Document");
	m_pComboBox->AddString(IsInChinese() ? L"所有打开的文档" : L"All Open Document");
	m_pComboBox->AddString(IsInChinese() ? L"当前包" : L"Current Package");
	m_pComboBox->AddString(IsInChinese() ? L"整个Jar文件" : L"The Whole Jar File");

	UpdateComboBox(true);

	//m_pComboBox->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void CFindDialog::UpdateComboBox(bool bResetData)
{
	m_pComboBox->Detach();
	VERIFY(m_pComboBox->Attach(::GetDlgItem(m_hWnd, IDC_CHOOSEAREA)));
	CLineNumEdit & rEdit = static_cast<CFernflowerUIMFCView*>(m_pParentFrame->GetActiveView())->m_wndEdit;
	//int CurSel = m_pComboBox->GetCurSel();
	if(bResetData)
		ResetData();
	else
	{
		CHARRANGE TempRange;
		unsigned int nStartPos = 0;
		rEdit.GetSel(TempRange);
		long nOriginal = PrevOptions.bSearchDown ? TempRange.cpMin : TempRange.cpMax;
		for (unsigned int i = 0; i < m_mapSearchResult[m_classNameQueue[nqueCurr]].size(); i++)
		{
			if (m_mapSearchResult[m_classNameQueue[nqueCurr]][i].cpMin < nOriginal)//WTF is this?
			{
				nStartPos = i + 1;
			}
		}
		if (!PrevOptions.bSearchDown)
		{
			nStartPos = (nStartPos > 0) ? nStartPos - 1 : 0;
		}
		m_mapResultStartPos[m_classNameQueue[nqueCurr]] = nStartPos;
		cntResultsShown = 0;
	}
	SetPrevOptions();//just for handling PrevOptions
	m_vecComboList.clear();
	m_vecComboList = { sCurrentDocument,sAllDocument,sCurrentPackage,sCurrentJar };
	auto & rScopeMap = CommonWrapper::GetMainFrame()->GetClassView()->m_mapScopeRange;
	if((Range.cpMin*Range.cpMax)==0)
		rEdit.GetSel(Range);
	CString str;
	rEdit.GetWindowText(str);
	CHARRANGE BestMatch{ 0,str.GetLength() };
	HTREEITEM hItem = m_pParentFrame->m_hClassItem;
	std::queue<HTREEITEM> SearchQueue;
	SearchQueue.push(hItem);
	CString NameSpace;
	while (!SearchQueue.empty())
	{
		hItem = SearchQueue.front();
		SearchQueue.pop();
		if (CommonWrapper::GetMainFrame()->GetClassView()->GetTreeView()->ItemHasChildren(hItem))
		{
			HTREEITEM hChildItem = CommonWrapper::GetMainFrame()->GetClassView()->GetTreeView()->GetChildItem(hItem);
			while (hChildItem)
			{
				SearchQueue.push(hChildItem);
				hChildItem = CommonWrapper::GetMainFrame()->GetClassView()->GetTreeView()->GetNextItem(hChildItem, TVGN_NEXT);
			}
		}
		if (rScopeMap.count(hItem))
		{
			if (rScopeMap[hItem].second.cpMin <= Range.cpMin&&rScopeMap[hItem].second.cpMax >= Range.cpMax
				&& Range.cpMin - rScopeMap[hItem].second.cpMin <= Range.cpMin - BestMatch.cpMin 
				&& rScopeMap[hItem].second.cpMax - Range.cpMax <= BestMatch.cpMax - Range.cpMax)
			{
				BestMatch = rScopeMap[hItem].second;
				NameSpace = rScopeMap[hItem].first;
			}
		}
	}
	for (int i = m_pComboBox->GetCount() - 5; i >= 0; i--)
	{
		m_pComboBox->DeleteString(i);
	}
	m_rCurrBlock = BestMatch;
	if (Range.cpMax - Range.cpMin >= 1)
	{
		m_vecComboList.insert(m_vecComboList.begin(), sCurrentSelect);
		m_pComboBox->InsertString(0,IsInChinese() ? L"所选内容" : L"Selected Content");
		if (((SelectedArea.cpMin*SelectedArea.cpMax) == 0)||(Range.cpMin!=SelectedArea.cpMin)||(Range.cpMax!=SelectedArea.cpMax))
		{
			int FirstVisible = rEdit.GetFirstVisibleLine();
			rEdit.LockWindowUpdate();
			CHARFORMAT2 Format;
			memset(&Format, 0, sizeof Format);
			Format.dwMask = CFM_BACKCOLOR;
			Format.dwEffects |= CFE_AUTOBACKCOLOR;
			Format.crBackColor = COLOR_WINDOW;
			rEdit.SetSel(SelectedArea);
			rEdit.SetSelectionCharFormat(Format);
			rEdit.SetSel(Range);
			Format.dwEffects &= ~CFE_AUTOBACKCOLOR;
			Format.crBackColor = RGB(174, 238, 238);
			rEdit.SetSelectionCharFormat(Format);
			rEdit.LineScroll(FirstVisible - rEdit.GetFirstVisibleLine());
			rEdit.UnlockWindowUpdate();
		}
		SelectedArea = Range;
		//CurSel = 0;
	}
	else
	{
		int FirstVisible = rEdit.GetFirstVisibleLine();
		rEdit.LockWindowUpdate();
		CHARFORMAT2 Format;
		memset(&Format, 0, sizeof Format);
		Format.dwMask = CFM_BACKCOLOR;
		Format.dwEffects |= CFE_AUTOBACKCOLOR;
		Format.crBackColor = COLOR_WINDOW;
		rEdit.SetSel(SelectedArea);
		rEdit.SetSelectionCharFormat(Format);
		rEdit.SetSel(Range);
		rEdit.LineScroll(FirstVisible - rEdit.GetFirstVisibleLine());
		rEdit.UnlockWindowUpdate();
		//if (SelectedArea.cpMin*SelectedArea.cpMax != 0)
		//{
		//	CurSel--;
		//	CurSel = CurSel >= 0 ? CurSel : 0;
		//}
		SelectedArea.cpMin = SelectedArea.cpMax = 0;
	}
	SearchBegin = 0;
	SearchEnd = -1;
	memset(&Range, 0, sizeof Range);
	//Copied from MSDN
	if (NameSpace != L"")
	{
		m_vecComboList.insert(m_vecComboList.begin(), sCurrentBlock);
		m_pComboBox->InsertString(0, IsInChinese() ? (L"当前块( " + NameSpace + L" )") : (L"Current Code Block( " + NameSpace + L" )"));
		//if (m_strNamespace == L"")
		//{
		//	CurSel = 0;
		//}
	}
	//else if (m_strNamespace != L"")
	//{
	//	CurSel--;
	//	CurSel = CurSel >= 0 ? CurSel : 0;
	//}
	m_strNamespace = NameSpace;
	
	CSize      sz;
	int        dx = 0;
	TEXTMETRIC tm;
	CDC*       pDC = m_pComboBox->GetDC();
	CFont*     pFont = m_pComboBox->GetFont();

	// Select the listbox font, save the old font
	CFont* pOldFont = pDC->SelectObject(pFont);
	// Get the text metrics for avg char width
	pDC->GetTextMetrics(&tm);

	for (int i = 0; i < m_pComboBox->GetCount(); i++)
	{
		m_pComboBox->GetLBText(i, str);
		sz = pDC->GetTextExtent(str);

		// Add the avg width to prevent clipping
		sz.cx += tm.tmAveCharWidth;

		if (sz.cx > dx)
			dx = sz.cx;
	}
	// Select the old font back into the DC
	pDC->SelectObject(pOldFont);
	m_pComboBox->ReleaseDC(pDC);

	// Adjust the width for the vertical scroll bar and the left and right border.
//	dx += ::GetSystemMetrics(SM_CXVSCROLL) + 2 * ::GetSystemMetrics(SM_CXEDGE);

	// Set the width of the list box so that every item is completely visible.
	m_pComboBox->SetDroppedWidth(dx);
	int CurSel, OrigSel = (std::find(m_vecComboList.begin(), m_vecComboList.end(), PrevOptions.nArea) != m_vecComboList.end()) ?
		std::find(m_vecComboList.begin(), m_vecComboList.end(), PrevOptions.nArea) - m_vecComboList.begin() : 0;
	CurSel = OrigSel;
	if (rEdit.GetSelText().Trim().Find(L' ') != -1)
	{
		std::vector<SearchArea>::iterator itCurSel;
		VERIFY((itCurSel = std::find(m_vecComboList.begin(), m_vecComboList.end(), sCurrentSelect)) != m_vecComboList.end());
		CurSel = itCurSel - m_vecComboList.begin();
	}
	else
	{
		int FirstVisible = rEdit.GetFirstVisibleLine();
		rEdit.LockWindowUpdate();
		CHARFORMAT2 Format;
		memset(&Format, 0, sizeof Format);
		Format.dwMask = CFM_BACKCOLOR;
		Format.dwEffects |= CFE_AUTOBACKCOLOR;
		Format.crBackColor = COLOR_WINDOW;
		rEdit.SetSel(SelectedArea);
		rEdit.SetSelectionCharFormat(Format);
		rEdit.LineScroll(FirstVisible - rEdit.GetFirstVisibleLine());
		rEdit.UnlockWindowUpdate();
	}
	m_pComboBox->SetCurSel(bResetData ? CurSel : OrigSel);
	SetFocus();
}

void CFindDialog::ResetData()
{
	if (!FinishedWork&&IsOptionChanged())
	{
		//#error "Add some shutting down code here"
		bAbortSearch = true;
		CommonWrapper::CWaitDlg WaitDlg(CommonWrapper::GetMainFrame(), []() {return (bool)FinishedWork; }, 10,
			IsInChinese() ? L"正在准备..." : L"Setting up...");
		WaitDlg.DoModal();
		bAbortSearch = false;
		//FinishedWork = true;
	}
	SearchingMultiDoc = false;
}


BEGIN_MESSAGE_MAP(CFindComboBox, CComboBox)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, &CFindComboBox::OnCbnSelchange)
END_MESSAGE_MAP()



void CFindComboBox::OnCbnSelchange()
{
	// TODO: 在此添加控件通知处理程序代码
	CFindDialog * pDialog = static_cast<CFindDialog*>(GetParent());
	CLineNumEdit & rEdit = static_cast<CFernflowerUIMFCView*>(pDialog->m_pParentFrame->GetActiveView())->m_wndEdit;
	if (pDialog->m_vecComboList[GetCurSel()] != CFindDialog::sCurrentSelect)
	{
		int FirstVisible = rEdit.GetFirstVisibleLine();
		rEdit.LockWindowUpdate();
		CHARFORMAT2 Format;
		memset(&Format, 0, sizeof Format);
		Format.dwMask = CFM_BACKCOLOR;
		Format.dwEffects |= CFE_AUTOBACKCOLOR;
		Format.crBackColor = COLOR_WINDOW;
		rEdit.SetSel(pDialog->SelectedArea);
		rEdit.SetSelectionCharFormat(Format);
		rEdit.LineScroll(FirstVisible - rEdit.GetFirstVisibleLine());
		rEdit.UnlockWindowUpdate();
	}
	else
	{
		int FirstVisible = rEdit.GetFirstVisibleLine();
		rEdit.LockWindowUpdate();
		CHARFORMAT2 Format;
		memset(&Format, 0, sizeof Format);
		Format.dwMask = CFM_BACKCOLOR;
		Format.dwEffects &= ~CFE_AUTOBACKCOLOR;
		Format.crBackColor = RGB(174, 238, 238);
		rEdit.SetSel(pDialog->SelectedArea);
		rEdit.SetSelectionCharFormat(Format);
		rEdit.LineScroll(FirstVisible - rEdit.GetFirstVisibleLine());
		rEdit.UnlockWindowUpdate();
	}
	
}
BEGIN_MESSAGE_MAP(CFindDialog, CFindReplaceDialog)
	ON_WM_ACTIVATE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()





void CFindDialog::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CFindReplaceDialog::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
	if (nState == WA_CLICKACTIVE)
		UpdateComboBox(!bIsSearching);
}


void CFindDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_CONTROL)
	{
		ModifyStyleEx(0, WS_EX_TRANSPARENT | WS_EX_LAYERED);
		SetLayeredWindowAttributes(0, 125, LWA_ALPHA);
	}
	else
	{
		ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
	}
	CFindReplaceDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CFindDialog::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
	CFindReplaceDialog::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CFindDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		UINT nChar = pMsg->wParam;
		if (nChar == VK_CONTROL)
		{
			ModifyStyleEx(0, WS_EX_TRANSPARENT | WS_EX_LAYERED);
			SetLayeredWindowAttributes(0, 125, LWA_ALPHA);
		}
		else
		{
			ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
		}
	}
	if (pMsg->message == WM_KEYUP)
	{
		ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
	}
	return CFindReplaceDialog::PreTranslateMessage(pMsg);
}


void CChildFrame::OnUpdateEditFind(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	if (this!=CommonWrapper::GetMainFrame()->m_pStartPage)
	{
		pCmdUI->Enable();
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}
