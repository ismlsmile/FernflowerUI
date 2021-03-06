
// FernflowerUI_MFCView.cpp: CFernflowerUIMFCView 类的实现
//

#include "stdafx.h"
#include "Resource.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "FernflowerUI_MFC.h"
#endif

#include "FernflowerUI_MFCDoc.h"
#include "FernflowerUI_MFCView.h"
#include "CommonWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const CStringW CFernflowerUIMFCView::KeyWord[50] = {
	L"abstract",L"assert" ,L"boolean" ,L"break" ,L"byte" ,L"case" ,L"catch" ,L"char" ,L"class" ,L"const" ,
	L"continue",L"default" ,L"do" ,L"double" ,L"else" ,L"enum" ,L"extends" ,L"final" ,L"finally" ,L"float" ,
	L"for",L"goto" ,L"if" ,L"implements" ,L"import" ,L"instanceof" ,L"int" ,L"interface" ,L"long" ,L"native" ,
	L"new",L"package" ,L"private" ,L"protected" ,L"public" ,L"return" ,L"short" ,L"static" ,L"strictfp" ,L"super" ,
	L"switch",L"synchronized" ,L"this" ,L"throw" ,L"throws" ,L"transient" ,L"try" ,L"void" ,L"volatile" ,L"while"
};
const CStringW CFernflowerUIMFCView::Operators[24] = { L"[",L"]",L"(",L")",L"+",L"-",L"*",L"/",L"%",L"<",L">",L"!",L"&",L"|",L"~",L"^",L"?",L":",L".",L"=",L",",L";",L"{",L"}" };
const CStringW CFernflowerUIMFCView::Strings[2]={L"\'",L"\""};

// CFernflowerUIMFCView

IMPLEMENT_DYNCREATE(CFernflowerUIMFCView, CView)

BEGIN_MESSAGE_MAP(CFernflowerUIMFCView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, &CFernflowerUIMFCView::OnEditCopy)
	ON_COMMAND(ID_EDIT_SELECT_ALL, &CFernflowerUIMFCView::OnEditSelectAll)
END_MESSAGE_MAP()

// CFernflowerUIMFCView 构造/析构

CFernflowerUIMFCView::CFernflowerUIMFCView():CView()
{
	// TODO: 在此处添加构造代码

}

void CFernflowerUIMFCView::SetViewText(const CStringW & Contact, CHARRANGE SelectedRange)
{
	auto Available = [](const FINDTEXTEX & Fnd)
	{return !((Fnd.chrgText.cpMin == -1) && (Fnd.chrgText.cpMax == -1)); };
	FinishHighLight = 0;
	m_wndEdit.ShowWindow(SW_HIDE);
	m_wndEdit.m_wndLineBox.ShowWindow(SW_HIDE);
	m_wndEdit.SetWindowTextW(Contact);
	m_wndEdit.ShowLineNum();
	for (size_t i = 0; i < 50; i++)
	{
		FINDTEXTEX Range = { 0 };
		FinishHighLight++;
		Range.lpstrText = KeyWord[i];
		Range.chrg.cpMin = 0;
		Range.chrg.cpMax = -1;
		m_wndEdit.FindTextW(FR_DOWN | FR_MATCHCASE, &Range);
		while (!((Range.chrgText.cpMin == -1) && (Range.chrgText.cpMax == -1)))
		{
			m_wndEdit.SetSel(Range.chrgText.cpMin - 1, Range.chrgText.cpMin);
			CStringW Front = m_wndEdit.GetSelText();
			if ((Front == L"(") || Front == L" " || Front == L"\n" || Front == L"\r" || Front == L"\t" || Range.chrgText.cpMin == 0)
			{
				m_wndEdit.SetSel(Range.chrgText.cpMax, Range.chrgText.cpMax + 1);
				CStringW Rear = m_wndEdit.GetSelText();
				if ((Rear == L")") || Rear == L" " || Rear == L"\n" || Rear == L"\r" || Rear == L"\t" || Rear == L"." || Range.chrgText.cpMax == -1)
				{
					m_wndEdit.SetSel(Range.chrgText);
					CHARFORMAT2 Format;
					memset(&Format, 0, sizeof Format);
					Format.cbSize = sizeof Format;
					Format.dwMask = CFM_BOLD;
					Format.dwEffects = CFE_BOLD;
					m_wndEdit.SetSelectionCharFormat(Format);
#ifdef _DEBUG
					TRACE(L"%s", m_wndEdit.GetSelText() + L"\n");
#endif // _DEBUG

				}
			}
			Range.chrg.cpMin = Range.chrgText.cpMin + 1;
			m_wndEdit.FindTextW(FR_DOWN | FR_WHOLEWORD | FR_MATCHCASE, &Range);
		}
	}
	for (size_t i = 0; i < 24; i++)
	{
		FINDTEXTEX Range = { 0 };
		Range.lpstrText = Operators[i];
		Range.chrg.cpMin = 0;
		Range.chrg.cpMax = -1;
		FinishHighLight++;
		m_wndEdit.FindTextW(FR_DOWN, &Range);
		while (!((Range.chrgText.cpMin == -1) && (Range.chrgText.cpMax == -1)))
		{
			m_wndEdit.SetSel(Range.chrgText);
			CHARFORMAT2 Format;
			memset(&Format, 0, sizeof Format);
			Format.cbSize = sizeof Format;
			Format.dwMask = CFM_COLOR | CFM_BOLD;
			Format.dwEffects = CFE_BOLD;
			Format.crTextColor = RGB(255, 0, 0);
			m_wndEdit.SetSelectionCharFormat(Format);
#ifdef _DEBUG
			TRACE(L"%s",m_wndEdit.GetSelText() + L"\n");
#endif // _DEBUG
			Range.chrg.cpMin = Range.chrgText.cpMin + 1;
			m_wndEdit.FindTextW(FR_DOWN, &Range);
		}
	}
	for (size_t i = 0; i < 2; i++)
	{
		FINDTEXTEX Range;
		Range.lpstrText = Strings[i];
		Range.chrg.cpMin = 0;
		Range.chrg.cpMax = -1;
		FinishHighLight++;
		m_wndEdit.FindTextW(FR_DOWN, &Range);
		while (!((Range.chrgText.cpMin == -1) && (Range.chrgText.cpMax == -1)))
		{
			int Front = Range.chrgText.cpMin;
			Range.chrg.cpMin = Range.chrgText.cpMin + 1;
			m_wndEdit.FindTextW(FR_DOWN, &Range);
			if (!Available(Range))
			{
				break;
			}
			int Rear = Range.chrgText.cpMax;
			Range.chrg.cpMin = Range.chrgText.cpMin + 1;
			m_wndEdit.FindTextW(FR_DOWN, &Range);
			m_wndEdit.SetSel(Front, Rear);
			CHARFORMAT2 Format;
			memset(&Format, 0, sizeof Format);
			Format.cbSize = sizeof Format;
			Format.dwMask = CFM_COLOR;
			Format.crTextColor = RGB(0, 0, 255);
			m_wndEdit.SetSelectionCharFormat(Format);
#ifdef _DEBUG
			TRACE(L"%s", m_wndEdit.GetSelText() + L"\n");
#endif // _DEBUG
		}
	}
	{
		FINDTEXTEX Range;
		Range.lpstrText = L"@";
		Range.chrg.cpMin = 0;
		Range.chrg.cpMax = -1;
		FinishHighLight++;
		m_wndEdit.FindTextW(FR_DOWN, &Range);
		while (!((Range.chrgText.cpMin == -1) && (Range.chrgText.cpMax == -1)))
		{
			{
				CHARFORMAT2 ChrClr;
				m_wndEdit.SetSel(Range.chrgText);
				m_wndEdit.GetSelectionCharFormat(ChrClr);
				if (ChrClr.crTextColor == RGB(0, 0, 255))
				{
					Range.chrg.cpMin = Range.chrgText.cpMax + 1;
					m_wndEdit.FindTextW(FR_DOWN, &Range);
					continue;
				}
			}
			int End = m_wndEdit.LineIndex(m_wndEdit.LineFromChar(Range.chrgText.cpMin) + 1) - 1;
			int Last = 0;
			FINDTEXTEX Annoate;
			Annoate.lpstrText = L"(";
			Annoate.chrg.cpMin = Range.chrgText.cpMin;
			Annoate.chrg.cpMax = End;
			FINDTEXTEX EndChar;
			int nCount = 0;
			m_wndEdit.FindTextW(FR_DOWN, &Annoate);
			if (Available(Annoate))
			{
				EndChar.lpstrText = L")";
				EndChar.chrg.cpMin = Range.chrgText.cpMin;
				EndChar.chrg.cpMax = -1;
				m_wndEdit.FindTextW(FR_DOWN, &EndChar);
				if (Available(EndChar))
				{
					Annoate.chrg.cpMax = EndChar.chrgText.cpMax;
					m_wndEdit.FindTextW(FR_DOWN, &Annoate);
					while (Available(Annoate))
					{
						CHARFORMAT2 Fmt;
						m_wndEdit.SetSel(Annoate.chrgText);
						m_wndEdit.GetSelectionCharFormat(Fmt);
						if (Fmt.crTextColor == RGB(255, 0, 0))
						{
							nCount++;
						}
						Annoate.chrg.cpMin = Annoate.chrgText.cpMax + 1;
						m_wndEdit.FindTextW(FR_DOWN, &Annoate);
					}
				}
			}
			else
			{
				m_wndEdit.SetSel(Range.chrgText.cpMin, End);
				CHARFORMAT2 Format;
				memset(&Format, 0, sizeof Format);
				Format.cbSize = sizeof Format;
				Format.dwMask = CFM_COLOR;
				Format.crTextColor = GetSysColor(COLOR_GRAYTEXT);
				m_wndEdit.SetSelectionCharFormat(Format);
				Range.chrg.cpMin = Range.chrgText.cpMax + 1;
				m_wndEdit.FindTextW(FR_DOWN, &Range);
				continue;
			}
			EndChar.lpstrText = L")";
			EndChar.chrg.cpMin = Range.chrgText.cpMin;
			EndChar.chrg.cpMax = -1;
			m_wndEdit.FindTextW(FR_DOWN, &EndChar);
			for (int i = 1; i <= nCount; i++)
			{
				m_wndEdit.SetSel(EndChar.chrgText.cpMin - 10, EndChar.chrgText.cpMax + 10);
				CStringW Str = m_wndEdit.GetSelText();
				m_wndEdit.SetSel(EndChar.chrgText);
				CHARFORMAT2 Fmt;
				memset(&Fmt, 0, sizeof Fmt);
				m_wndEdit.GetSelectionCharFormat(Fmt);
				if (Fmt.crTextColor != RGB(255, 0, 0))
				{
					i--;
				}
				EndChar.chrg.cpMin = EndChar.chrgText.cpMax + 1;
				Last = EndChar.chrgText.cpMax;
				m_wndEdit.FindTextW(FR_DOWN, &EndChar);
			}
			m_wndEdit.SetSel(Range.chrgText.cpMin, Last);
#ifdef _DEBUG
			CStringW Str = m_wndEdit.GetSelText(); // 调试语句
#endif // _DEBUG
			CHARFORMAT2 Format;
			memset(&Format, 0, sizeof Format);
			Format.cbSize = sizeof Format;
			Format.dwMask = CFM_COLOR;
			Format.crTextColor = GetSysColor(COLOR_GRAYTEXT);
			m_wndEdit.SetSelectionCharFormat(Format);
			Range.chrg.cpMin = Range.chrgText.cpMax + 1;
			m_wndEdit.FindTextW(FR_DOWN, &Range);
		}
	}
	{
		FINDTEXTEX Range;
		Range.lpstrText = L"//";
		Range.chrg.cpMin = 0;
		Range.chrg.cpMax = -1;
		FinishHighLight++;
		m_wndEdit.FindTextW(FR_DOWN, &Range);
		while (Available(Range))
		{
			int iStart = Range.chrgText.cpMin;
			int iEnd = m_wndEdit.LineIndex(m_wndEdit.LineFromChar(Range.chrgText.cpMin) + 1) - 1;
			CHARFORMAT2 Format;
			memset(&Format, 0, sizeof Format);
			Format.dwMask = CFM_COLOR | CFM_ITALIC;
			Format.dwEffects = CFE_ITALIC;
			Format.cbSize = sizeof Format;
			Format.crTextColor = RGB(174, 238, 238);
			m_wndEdit.SetSel(iStart, iEnd);
			m_wndEdit.SetSelectionCharFormat(Format);
			Range.chrg.cpMin = Range.chrgText.cpMax + 1;
			m_wndEdit.FindTextW(FR_DOWN | FR_WHOLEWORD | FR_MATCHCASE, &Range);
		}
	}
	m_wndEdit.ShowWindow(SW_SHOW);
	m_wndEdit.m_wndLineBox.ShowWindow(SW_SHOW);
	m_wndEdit.HideSelection(false, true);
	if(!CommonWrapper::GetMainFrame()->m_pActiveFindDialog)
		m_wndEdit.SetFocus();
	m_wndEdit.SetSel(SelectedRange);

	FinishHighLight = -1;
}

CFernflowerUIMFCView::~CFernflowerUIMFCView()
{
}

BOOL CFernflowerUIMFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式
	return CView::PreCreateWindow(cs);
}

// CFernflowerUIMFCView 绘图

void CFernflowerUIMFCView::OnDraw(CDC* /*pDC*/)
{
/*	CFernflowerUIMFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
		*/
	// TODO: 在此处为本机数据添加绘制代码
}

void CFernflowerUIMFCView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CFernflowerUIMFCView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CFernflowerUIMFCView 诊断

#ifdef _DEBUG
void CFernflowerUIMFCView::AssertValid() const
{
	CView::AssertValid();
}

void CFernflowerUIMFCView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CFernflowerUIMFCDoc* CFernflowerUIMFCView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFernflowerUIMFCDoc)));
	return (CFernflowerUIMFCDoc*)m_pDocument;
}
#endif //_DEBUG



int CFernflowerUIMFCView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	this->GetWindowRect(&rectDummy);
	if (!this->m_wndEdit.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_HSCROLL | WS_VSCROLL, 
		CRect(0, 0, rectDummy.Width(), rectDummy.Height()), this, ID_EDITWINDOW))
	{
		TRACE0("未能创建视图");
		return -1;
	}
	m_wndEdit.SetReadOnly();
	CHARFORMAT2 Format;
	m_wndEdit.GetDefaultCharFormat(Format);
	lstrcpy(Format.szFaceName, theApp.FontFaceName);
	Format.yHeight = theApp.FontSize * 20;
	//Format.yHeight -= 18;
	m_wndEdit.m_wndLineBox.SetDefaultCharFormat(Format);
	//UINT Flags = m_wndEdit.m_wndLineBox.SendMessage(EM_GETEDITSTYLEEX, 0, 0);
	//m_wndEdit.m_wndLineBox.SendMessage(EM_SETEDITSTYLEEX, Flags | SES_EX_USEMOUSEWPARAM, 0);
	//Format.yHeight += 18;
	Format.dwEffects &= ~CFE_AUTOBACKCOLOR;
	Format.dwMask |= CFM_BACKCOLOR;
	Format.crBackColor = RGB(255, 255, 255);
	m_wndEdit.SetDefaultCharFormat(Format);
	m_wndEdit.SetAutoURLDetect();
	m_wndEdit.LimitText(LONG_MAX);
	m_wndEdit.SendMessage(EM_SETZOOM, 0, 0);
	

	return 0;
}


void CFernflowerUIMFCView::OnDestroy()
{
	m_wndEdit.DestroyWindow();
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CFernflowerUIMFCView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	CRect rectDummy;
	rectDummy.SetRectEmpty();
	this->GetWindowRect(&rectDummy);
	m_wndEdit.MoveWindow(CRect(50, 0, rectDummy.Width(), rectDummy.Height()));
	m_wndEdit.m_wndLineBox.MoveWindow(CRect(0, 0, 50, rectDummy.Height()));
	m_wndEdit.m_wndLineBox.BringWindowToTop();
	// TODO: 在此处添加消息处理程序代码
}


void CFernflowerUIMFCView::OnEditCopy()
{
	// TODO: 在此添加命令处理程序代码
	m_wndEdit.Copy();
}


void CFernflowerUIMFCView::OnEditSelectAll()
{
	// TODO: 在此添加命令处理程序代码
	m_wndEdit.SetSel(0, -1);
}
BEGIN_MESSAGE_MAP(CLineNumEdit, CRichEditCtrl)
	ON_WM_PAINT()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


BOOL CLineNumEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: 在此添加专用代码和/或调用基类
	CRect MainRect = rect;
	MainRect.left += 50;
	if (!CRichEditCtrl::Create(dwStyle, MainRect, pParentWnd, nID))
	{
		return false;
	}
	if (!m_wndLineBox.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | ES_MULTILINE,CRect(rect.left,rect.top,MainRect.left,rect.bottom),GetParent(),ID_LINEBOX))
	{
		return false;
	}
	m_wndLineBox.EnableWindow(FALSE);
	m_wndLineBox.SetReadOnly();
	PARAFORMAT ParaFmt;
	GetParaFormat(ParaFmt);
	ParaFmt.wAlignment = PFA_CENTER;
	m_wndLineBox.SetParaFormat(ParaFmt);
	PARAFORMAT2 Para;
	memset(&Para, 0, sizeof Para);
	Para.dwMask = PFM_LINESPACING;
	Para.bLineSpacingRule = 5;
	Para.dyLineSpacing = 20;
	m_wndLineBox.SetParaFormat(Para);
	return true;
}

void CLineNumEdit::ShowLineNum()
{
	CStringW Content;
	CStringW str;
	for (int i = 0; i < GetLineCount() - 1; i++)
	{
		str.Format(L"%d\n", i + 1);
		Content += str;
	}
	m_wndLineBox.SetWindowTextW(Content);
}


void CLineNumEdit::OnPaint()
{
//	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 为绘图消息调用 CRichEditCtrl::OnPaint()
	CRichEditCtrl::OnPaint();
	m_wndLineBox.LineScroll(GetFirstVisibleLine() - m_wndLineBox.GetFirstVisibleLine());
}


void CLineNumEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CommonWrapper::GetMainFrame()->MDIGetActive()->SendMessage(this->GetCurrentMessage()->message, this->GetCurrentMessage()->wParam, this->GetCurrentMessage()->lParam);
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CLineNumEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CommonWrapper::GetMainFrame()->MDIGetActive()->SendMessage(this->GetCurrentMessage()->message, this->GetCurrentMessage()->wParam, this->GetCurrentMessage()->lParam);
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CLineNumEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//m_wndLineBox.SendMessage(this->GetCurrentMessage()->message, this->GetCurrentMessage()->wParam, this->GetCurrentMessage()->lParam);
	if (nFlags & MK_CONTROL)
	{
		/*CHARFORMAT2 DefFormat, Format;
		memset(&DefFormat, 0, sizeof DefFormat);
		memset(&Format, 0, sizeof Format);
		GetDefaultCharFormat(DefFormat);
		Format.dwMask = CFM_SIZE;
		Format.yHeight = DefFormat.yHeight - 17;
		m_wndLineBox.SetDefaultCharFormat(Format);
		m_wndLineBox.Invalidate();*/
		return TRUE;
	}
	return CRichEditCtrl::OnMouseWheel(nFlags, zDelta, pt);
}
