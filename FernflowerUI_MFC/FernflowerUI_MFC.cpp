
// FernflowerUI_MFC.cpp: 定义应用程序的类行为。
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "FernflowerUI_MFC.h"
#include "MainFrm.h"
#include "CommonWrapper.h"
#include "ChildFrm.h"
#include "FernflowerUI_MFCDoc.h"
#include "FernflowerUI_MFCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFernflowerUIMFCApp

BEGIN_MESSAGE_MAP(CFernflowerUIMFCApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CFernflowerUIMFCApp::OnAppAbout)
	// 基于文件的标准文档命令
	ON_COMMAND(ID_FILE_OPEN, &CFernflowerUIMFCApp::OpenFile)
	ON_COMMAND(ID_EDIT_COPY, &CFernflowerUIMFCApp::OnEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CFernflowerUIMFCApp::OnEditSelectAll)
	// 标准打印设置命令
END_MESSAGE_MAP()

bool AccessJar(HWND hwnd);
bool DownloadJar();

// CFernflowerUIMFCApp 构造

CFernflowerUIMFCApp::CFernflowerUIMFCApp()
{
	m_bHiColorIcons = TRUE;

	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// 如果应用程序是利用公共语言运行时支持(/clr)构建的，则: 
	//     1) 必须有此附加设置，“重新启动管理器”支持才能正常工作。
	//     2) 在您的项目中，您必须按照生成顺序向 System.Windows.Forms 添加引用。
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("6168218c.FernflowerUI.FernflowerUIMFC.3.2.0.1"));

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CFernflowerUIMFCApp 对象

CFernflowerUIMFCApp theApp;


// CFernflowerUIMFCApp 初始化

BOOL CFernflowerUIMFCApp::InitInstance()
{
	Initing = true;
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES | ICC_TREEVIEW_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	SetLastError(0);
	mtx = ::CreateMutex(nullptr, true, _T("6168218c.FernflowerUI.FernflowerUIMFC"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
#ifdef _DEBUG
		AfxMessageBox(L"已有一个实例在运行");
#endif // _DEBUG
		return false;
	}
	else if (mtx == nullptr)
	{
		return false;
	}

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(L"OLE库初始化失败!");
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();
	CommonWrapper::CLaunchDialog * LaunchDlg = new CommonWrapper::CLaunchDialog;
	LaunchDlg->Create(IDD_LAUNCHBAR);
	LaunchDlg->ShowWindow(SW_SHOW);
	CoInitialize(nullptr);

	// 使用 RichEdit 控件需要 AfxInitRichEdit2()
	AfxInitRichEdit5();

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("6168218c"));
	LoadStdProfileSettings(4);  // 加载标准 INI 文件选项(包括 MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// 注册应用程序的文档模板。  文档模板
	// 将用作文档、框架窗口和视图之间的连接
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_FernflowerUITYPE,
		RUNTIME_CLASS(CFernflowerUIMFCDoc),
		RUNTIME_CLASS(CChildFrame), // 自定义 MDI 子框架
		RUNTIME_CLASS(CFernflowerUIMFCView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// 创建主 MDI 框架窗口
	m_bDeferShowOnFirstWindowPlacementLoad = TRUE;
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE | WS_MINIMIZE))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->DragAcceptFiles();


	// 分析标准 shell 命令、DDE、打开文件操作的命令行
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// 调度在命令行中指定的命令。  如果
	// 用 /RegServer、/Register、/Unregserver 或 /Unregister 启动应用程序，则返回 FALSE。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	CommonWrapper::GetMainFrame()->MDIGetActive()->MDIDestroy();
	CStringW Path = this->GetProfileStringW(L"ShellViewPath", L"PathName", L"Not Found");
	UINT Enabled = this->GetProfileIntW(L"ShellViewPath", L"EnableRecovery", -1);
	if (Enabled != -1)
	{
		EnableRecovery = Enabled;
	}
	if (EnableRecovery)
	{
		EnableRecovery = false;
	}
	else
	{
		EnableRecovery = true;
	}
	m_pMainWnd->SendMessage(WM_COMMAND, ID_ENABLE_RECOVERY);
	if (Path != L"Not Found")
	{
		if (EnableRecovery)
		{
			static_cast<CMainFrame*>(m_pMainWnd)->m_wndFileView.m_ShellTreeView.ShowWindow(SW_HIDE);
			static_cast<CMainFrame*>(m_pMainWnd)->m_wndFileView.m_ShellTreeView.SelectPath(Path);
			static_cast<CMainFrame*>(m_pMainWnd)->m_wndFileView.m_ShellTreeView.ShowWindow(SW_SHOW);
		}
		PathToSave = Path;
	}
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		static_cast<CMainFrame*>(AfxGetMainWnd())->m_wndOutput.OutputLog((const wchar_t*)cmdInfo.m_strFileName);
	}
	m_pMainWnd->SetWindowText(L"FernFlowerUI");
	LaunchDlg->ShowWindow(SW_HIDE);
	LaunchDlg->DestroyWindow();
	delete LaunchDlg;
	Initing = false;

	// 主窗口已初始化，因此显示它并对其进行更新
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	AccessJar(AfxGetMainWnd()->m_hWnd);

	return TRUE;
}

void CFernflowerUIMFCApp::DoDecomplie(const CStringW & Source)
{
	DecompFlags Flag;
	SHFILEINFO FileInfo;
	//I think these code looks like Java,especially in "if"
	if (Source.Right(6).CompareNoCase(L".class") == 0)
	{
		Flag = DecompFlags::DecompClass;
		if (SHGetFileInfo(Source, 0, &FileInfo, sizeof FileInfo, SHGFI_DISPLAYNAME) == 0)
		{
			AfxGetMainWnd()->MessageBox(L"所选文件不存在!", L"错误", MB_ICONERROR);
			return;
		}
	}
	else if ((Source.Right(4).CompareNoCase(L".jar") == 0) || (Source.Right(4).CompareNoCase(L".zip") == 0))
	{
		Flag = DecompFlags::DecompJar;
	}
	static_cast<CMainFrame*>(AfxGetMainWnd())->m_wndOutput.OutputLog((const wchar_t *)(Source), Flag, FileInfo.szDisplayName);
}

int CFernflowerUIMFCApp::ExitInstance()
{
	//TODO: 处理可能已添加的附加资源
	::CoUninitialize();
	if (mtx != nullptr)
	{
		CloseHandle(mtx);
	}
	CStringW Path = PathToSave;
	this->WriteProfileStringW(L"ShellViewPath", L"PathName", Path);
	this->WriteProfileInt(L"ShellViewPath", L"EnableRecovery", EnableRecovery);
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CFernflowerUIMFCApp 消息处理程序


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CFernflowerUIMFCApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CFernflowerUIMFCApp::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	CFernflowerUIMFCView * View = static_cast<CFernflowerUIMFCView*>(static_cast<CMainFrame*>(AfxGetMainWnd())->MDIGetActive()->GetActiveView());
	View->m_wndEdit.Copy();
}

void CFernflowerUIMFCApp::OnEditSelectAll()
{
	// TODO: 在此添加命令处理程序代码
	CFernflowerUIMFCView * View = static_cast<CFernflowerUIMFCView*>(static_cast<CMainFrame*>(AfxGetMainWnd())->MDIGetActive()->GetActiveView());
	View->m_wndEdit.SetSel(0, -1);
}

void CFernflowerUIMFCApp::OpenFile()
{
	if (!AccessJar(AfxGetMainWnd()->m_hWnd))
	{
		return;
	}
	CFileDialog OpenDlg(true, nullptr, nullptr, OFN_EXPLORER | OFN_FORCESHOWHIDDEN, L"Jar文件 (*.jar)|*.jar|Zip文件 (*.zip)|*.zip|Java类 (*.class)|*.class||", m_pMainWnd);
	static_cast<CMainFrame*>(AfxGetMainWnd())->m_wndStatusBar.SetPaneText(ID_SEPARATOR, L"请选择反编译源文件");
	if (OpenDlg.DoModal() == IDOK)
	{
		DoDecomplie(OpenDlg.GetPathName());
	}
	else
	{
		return;
	}
}

// CFernflowerUIMFCApp 自定义加载/保存方法

void CFernflowerUIMFCApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	//bNameValid = strName.LoadString(IDS_EXPLORER);
	//ASSERT(bNameValid);
	//GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}

void CFernflowerUIMFCApp::LoadCustomState()
{
}

void CFernflowerUIMFCApp::SaveCustomState()
{
}

// CFernflowerUIMFCApp 消息处理程序


bool AccessJar(HWND hwnd)
{
	char * java_home;
	size_t tmp;
	errno_t er = _dupenv_s(&java_home, &tmp, "JAVA_HOME");
	if (er || (java_home == nullptr))
	{
		if (MessageBoxA(hwnd, "发现未安装Java虚拟机,本程序需要Java虚拟机才能正常使用,是否打开下载链接?", "警告", MB_ICONWARNING | MB_OKCANCEL) == IDOK)
		{
			ShellExecuteA(NULL, "open", "http://www.oracle.com/technetwork/java/javase/overview/index.html", NULL, NULL, SW_SHOW);
			return false;
		}
	}
	free(java_home);
	char * userprofile;
	size_t len;
	if (_dupenv_s(&userprofile, &len, "USERPROFILE"))
	{
		return false;
	}
	std::string saccess = userprofile;
	free(userprofile);
	saccess += "\\AppData\\Local\\FernFlowerUI";
	if (_access(saccess.c_str(), 0) == -1)
	{
		_mkdir(saccess.c_str());
		if (MessageBoxA(hwnd, "检测到是第一次使用，是否要下载fernflower.jar?", "", MB_OKCANCEL) == IDOK)
			return DownloadJar();
		else
		{
			return false;
		}
	}
	else if (_access((saccess + "\\fernflower.jar").c_str(), 0) == -1)
	{
		if (MessageBoxA(hwnd, "找不到%USERPROFILE%\\AppData\\Local\\FernFlowerUI\\fernflower.jar,是否要下载?", "", MB_OKCANCEL) == IDOK)
			return DownloadJar();
		else
		{
			return false;
		}
	}
	else
	{
		HANDLE hDecomp = CreateFileA((saccess + "\\fernflower.jar").c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if ((hDecomp == INVALID_HANDLE_VALUE) || (GetFileSize(hDecomp, nullptr) < 248683))
		{
			CloseHandle(hDecomp);
			if (MessageBoxA(hwnd, "找不到%USERPROFILE%\\AppData\\Local\\FernFlowerUI\\fernflower.jar,是否要下载?", "", MB_OKCANCEL) == IDOK)
				return DownloadJar();
			else
			{
				return false;
			}
		}
		CloseHandle(hDecomp);
	}
	return true;
}

const int MAXBLOCKSIZE = 1024;
char Buffer[MAXBLOCKSIZE];
unsigned long Number = 1;
int Byte;
//FILE* stream;
HINTERNET hSession, handle;
bool Finished;

bool DownloadJar()
{
	std::ofstream fout;
	std::ios_base::sync_with_stdio(false);
	hSession = InternetOpenA("FernFlowerUI2.1", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
	try
	{
		if (hSession)
		{
			handle = InternetOpenUrlA(hSession, "https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar",
				nullptr, 0, INTERNET_FLAG_DONT_CACHE, 0);
			if (handle)
			{
				char * filestr;
				size_t length;
				errno_t err = _dupenv_s(&filestr, &length, "USERPROFILE");
				if (err)
				{
					MessageBoxA(nullptr, "无法打开%USERPROFILE%", "错误", MB_ICONERROR);
					return false;
				}
				std::string filename = filestr;
				free(filestr);
				filename += "\\AppData\\Local\\FernFlowerUI\\fernflower.jar";
				fout.open(filename.c_str(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
				if (fout.is_open())
				{
					/*CWaitDlg wait(AfxGetMainWnd(), []() ->bool
					{
					InternetReadFile(handle, Buffer, MAXBLOCKSIZE - 1, &Number);
					fwrite(Buffer, sizeof(char), Number, stream);
					bool Downloaded = !(Number > 0);
					return Downloaded;
					},);
					wait.DoModal();
					*/
					Byte = 0;
					CommonWrapper::CProgressBar Progress(AfxGetMainWnd(), []()->int {return Finished ? -1 : Byte; }, 5, L"正在下载fernflower.jar", 1, 243);
					std::future<void> Download = std::async(std::launch::async, [&]() {
						while (Number > 0)
						{
							InternetReadFile(handle, Buffer, MAXBLOCKSIZE - 1, &Number);
							//fwrite(Buffer, sizeof(char), Number, stream);
							fout.write(Buffer, Number);
							Byte++;
						}
						Finished = true;
					});
					Progress.DoModal();
					//fclose(stream);
					fout.close();
				}
				InternetCloseHandle(handle);
				handle = nullptr;
			}
			else
			{
				InternetCloseHandle(hSession);
				hSession = nullptr;
				throw connection_error();
			}
			InternetCloseHandle(hSession);
			hSession = nullptr;
		}
		else
		{
			throw connection_error();
		}
	}
	catch (const std::exception& ex)
	{
		MessageBox(nullptr, CStringW(L"已引发异常:") + CStringW(ex.what()) + L"下载失败,尝试手动下载,链接:https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar", L"下载失败", MB_ICONERROR);
		ShellExecuteA(NULL, "open", "https://raw.githubusercontent.com/6168218c/Fernflower---forge/master/fernflower.jar", NULL, NULL, SW_SHOW);
		std::ios_base::sync_with_stdio(true);
		return false;
	}
	std::ios_base::sync_with_stdio(true);
	return true;
}
