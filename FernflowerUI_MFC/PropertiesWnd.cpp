
#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "FernflowerUI_MFC.h"
#include "CommonWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFernflowerUIGridProperty
class CFernflowerUIMFCGridProperty:public CMFCPropertyGridProperty
{
	friend class CFernflowerUIMFCGridCtrl;
protected:
	
public:
	using CMFCPropertyGridProperty::CMFCPropertyGridProperty;
};

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	/*ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)*/
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight/* + cyTlb*/, rectClient.Width(), rectClient.Height() -(m_nComboHeight/*+cyTlb*/), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	/*if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("未能创建属性组合 \n");
		return -1;      // 未能创建
	}*/

	/*m_wndObjectCombo.AddString(_T("应用程序"));
	m_wndObjectCombo.AddString(_T("属性窗口"));
	m_wndObjectCombo.SetCurSel(0);*/

	//CRect rectCombo;
	//m_wndObjectCombo.GetClientRect (&rectCombo);

	//m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	m_wndPropList.EnableDescriptionArea();

	InitPropList();

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	//m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	/*m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);*/

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: 在此处添加命令处理程序代码
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: 在此处添加命令更新 UI 处理程序代码
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();

	CFernflowerUIMFCGridProperty* pGroup = new CFernflowerUIMFCGridProperty(IsInChinese()?_T("反编译参数"):_T("Decompile Arguments"));

	//pGroup->AddSubItem(new CFernflowerUIMFCGridProperty(_T("三维外观"), (_variant_t) false, _T("指定窗口的字体不使用粗体，并且控件将使用三维边框")));

	for (auto it = theApp.m_DecompOption.OptionMap.begin(); it != theApp.m_DecompOption.OptionMap.end(); it++)
	{
		if (it->first == L"log")
		{
			CFernflowerUIMFCGridProperty * pProp = new CFernflowerUIMFCGridProperty(it->first, it->second, theApp.m_DecompOption.TooltipMap[it->first]);
			pProp->AddOption(L"ERROR");
			pProp->AddOption(L"WARN");
			pProp->AddOption(L"INFO");
			pProp->AddOption(L"TRACE");
			pProp->AllowEdit();
			pGroup->AddSubItem(pProp);
		}
		else if (it->first == L"ind")
		{
			CFernflowerUIMFCGridProperty * pProp = new CFernflowerUIMFCGridProperty(it->first, _variant_t((short)6), theApp.m_DecompOption.TooltipMap[it->first]);
			pProp->EnableSpinControl(TRUE, 1, 10);
			pGroup->AddSubItem(pProp);
		}
		else
		{
			pGroup->AddSubItem(new CFernflowerUIMFCGridProperty(it->first, (_variant_t)(it->second == L"1"), theApp.m_DecompOption.TooltipMap[it->first]));
		}
	}
	m_wndPropList.AddProperty(pGroup);
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	//m_wndObjectCombo.SetFont(&m_fntPropList);
}




void CPropertiesWnd::UpdateOptionData()
{
	// TODO: 在此处添加实现代码.
	int nCount = m_wndPropList.GetPropertyCount();
	CMFCPropertyGridProperty * pGroup = m_wndPropList.GetProperty(0);
	int nSubCount = pGroup->GetSubItemsCount();
	for (int i = 0; i < nSubCount; i++)
	{
		CMFCPropertyGridProperty * pProp = pGroup->GetSubItem(i);
		CString Name = pProp->GetName();
		if (Name == L"log")
		{
			CString str = pProp->GetValue().bstrVal;
			theApp.m_DecompOption.OptionMap[Name] = str;
		}
		else if (Name == L"ind")
		{
			short nCount = pProp->GetValue().iVal;
			theApp.m_DecompOption.OptionMap[Name] = L'\"' + CString(L' ', nCount) + L'\"';
		}
		else
		{
			BOOL  b = pProp->GetValue().boolVal;
			CString str = b ? L"1" : L"0";
			theApp.m_DecompOption.OptionMap[Name] = str;
		}
	}
}


void CPropertiesWnd::OnDestroy()
{
	UpdateOptionData();
	CDockablePane::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CPropertiesWnd::SyncWithOptionData()
{
	// TODO: 在此处添加实现代码.
	int nCount = m_wndPropList.GetPropertyCount();
	CMFCPropertyGridProperty * pGroup = m_wndPropList.GetProperty(0);
	int nSubCount = pGroup->GetSubItemsCount();
	for (int i = 0; i < nSubCount; i++)
	{
		CMFCPropertyGridProperty * pProp = pGroup->GetSubItem(i);
		CString Name = pProp->GetName();
		if (Name == L"log")
		{
			pProp->SetValue(theApp.m_DecompOption.OptionMap[Name]);
			
		}
		else if (Name == L"ind")
		{
			pProp->SetValue(_variant_t((short)(theApp.m_DecompOption.OptionMap[Name].GetLength() - 2)));
		}
		else
		{
			pProp->SetValue((_variant_t)(theApp.m_DecompOption.OptionMap[Name] == L"1"));
		}
	}
}



void CFernflowerUIMFCGridCtrl::AdjustLayout()
{
	// TODO: 在此添加专用代码和/或调用基类
	if (m_pSel)
	{
		CRect rectDescr;
		GetClientRect(rectDescr);
		rectDescr.DeflateRect(AFX_TEXT_MARGIN + 1, AFX_TEXT_MARGIN + 1);
		CClientDC dc(this);
		HFONT hfontOld = SetCurrFont(&dc);
		dc.SetTextColor(GetTextColor());
		dc.SetBkMode(TRANSPARENT);
		TEXTMETRIC tm;
		dc.GetTextMetrics(&tm);
		m_nRowHeight = tm.tmHeight + 4;
		HFONT hOldFont = (HFONT) ::SelectObject(dc.GetSafeHdc(), this->m_fontBold.GetSafeHandle());
		int nLineHeight = dc.DrawText(m_pSel->GetName(), rectDescr, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
		::SelectObject(dc.GetSafeHdc(), hOldFont);
		GetClientRect(rectDescr);
		dc.DrawText(m_pSel->GetDescription(), rectDescr, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL);
		int DescrHeight = rectDescr.Height() + nLineHeight * 2 + 2;
		m_nDescrHeight = max(DescrHeight, m_nDescrHeight);
		m_nDescrRows = (m_nDescrHeight%m_nRowHeight) ? (m_nDescrHeight / m_nRowHeight + 1) : (m_nDescrHeight / m_nRowHeight);
		m_nDescrHeight = m_nDescrRows * m_nRowHeight;
		::SelectObject(dc.GetSafeHdc(), hfontOld);
		Invalidate();
	}
	return CMFCPropertyGridCtrl::AdjustLayout();
}
BEGIN_MESSAGE_MAP(CFernflowerUIMFCGridCtrl, CMFCPropertyGridCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()


void CFernflowerUIMFCGridCtrl::OnPaint()
{
	CPaintDC dcPaint(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CMFCPropertyGridCtrl::OnPaint()

	OnDraw(&dcPaint);
}

void CFernflowerUIMFCGridCtrl::OnDraw(CDC * pDCSrc)
{
	ASSERT_VALID(pDCSrc);

	CMemDC memDC(*pDCSrc, this);
	CDC* pDC = &memDC.GetDC();

	m_clrGray = CMFCVisualManager::GetInstance()->GetPropertyGridGroupColor(this);

	CRect rectClient;
	GetClientRect(rectClient);

	OnFillBackground(pDC, rectClient);

	HFONT hfontOld = SetCurrFont(pDC);
	pDC->SetTextColor(GetTextColor());
	pDC->SetBkMode(TRANSPARENT);

	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	m_nRowHeight = tm.tmHeight + 4;

	if (m_pSel)
	{
		CRect rectDescr = rectClient;
		rectDescr.DeflateRect(AFX_TEXT_MARGIN + 1, AFX_TEXT_MARGIN + 1);
		HFONT hOldFont = (HFONT) ::SelectObject(pDC->GetSafeHdc(), this->m_fontBold.GetSafeHandle());
		int nLineHeight = pDC->DrawText(m_pSel->GetName(), rectDescr, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
		::SelectObject(pDC->GetSafeHdc(), hOldFont);
		rectDescr = rectClient;
		pDC->DrawText(m_pSel->GetDescription(), rectDescr, DT_CALCRECT | DT_NOPREFIX | DT_WORDBREAK | DT_EDITCONTROL);
		int DescrHeight = rectDescr.Height() + nLineHeight*2 + 2;
		m_nDescrHeight = max(DescrHeight, m_nDescrHeight);
		m_nDescrRows = (m_nDescrHeight%m_nRowHeight) ? (m_nDescrHeight / m_nRowHeight + 1) : (m_nDescrHeight / m_nRowHeight);
		m_nDescrHeight = m_nDescrRows * m_nRowHeight;

		m_rectList = rectClient;
		m_rectList.bottom -= m_nDescrHeight;
		if (HasDrawnTwice)
		{
			HasDrawnTwice = false;
		}
		else
		{
			HasDrawnTwice = true;
			Invalidate();
		}
	}

	OnDrawList(pDC);

	if (m_bDescriptionArea)
	{
		CRect rectDescr = rectClient;
		rectDescr.top = m_rectList.bottom;
		if (rectDescr.Height() > 0)
		{
			rectDescr.DeflateRect(1, 1);
			OnDrawDescription(pDC, rectDescr);
		}
	}

	::SelectObject(pDC->GetSafeHdc(), hfontOld);
}
