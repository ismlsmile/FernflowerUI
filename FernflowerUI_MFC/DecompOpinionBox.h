#pragma once


// DecompOpinionBox 对话框

class CDecompOpinionBox : public CDialogEx
{
	DECLARE_DYNAMIC(CDecompOpinionBox)

public:
	CDecompOpinionBox(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDecompOpinionBox();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPINIONBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	CTreeCtrl * m_pTreeCtrl;
	CFont m_Font;

	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnInitDialog(WPARAM wParam, LPARAM lParam);
};
