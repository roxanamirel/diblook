#pragma once


// ErrorSelect dialog

class ErrorSelect : public CDialogEx
{
	DECLARE_DYNAMIC(ErrorSelect)

public:
	enum ErrorVal{
		one,two,three,four

	};
	ErrorSelect(CWnd* pParent = NULL);   // standard constructor
	virtual ~ErrorSelect();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int errorValue;
	CSliderCtrl m_slider;
//	CEdit m_sliderValue;
	CString m_sliderValues;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
