// ErrorSelect.cpp : implementation file
//

#include "stdafx.h"
#include "diblook.h"
#include "ErrorSelect.h"
#include "afxdialogex.h"


// ErrorSelect dialog

IMPLEMENT_DYNAMIC(ErrorSelect, CDialogEx)

ErrorSelect::ErrorSelect(CWnd* pParent /*=NULL*/)
	: CDialogEx(ErrorSelect::IDD, pParent)
	, errorValue(0)
{

	m_sliderValues = _T("");
}

ErrorSelect::~ErrorSelect()
{
}

void ErrorSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_Error1, errorValue);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
	//  DDX_Control(pDX, IDC_EDIT2, m_sliderValue);
	DDX_Text(pDX, IDC_EDIT2, m_sliderValues);
}


BEGIN_MESSAGE_MAP(ErrorSelect, CDialogEx)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// ErrorSelect message handlers


BOOL ErrorSelect::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_slider.SetRangeMin(1, false);
    m_slider.SetRangeMax(100, false);
    m_sliderValues = "1";
    UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void ErrorSelect::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(nSBCode == SB_THUMBPOSITION) {
		m_sliderValues.Format("%ld", nPos);
	    UpdateData(false);
    }

    else {
        CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
    }

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
