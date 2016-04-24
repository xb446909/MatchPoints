
// MatchPointsTestDlg.h : header file
//

#pragma once

#include <vector>
#include "opencv2\opencv.hpp"
#include "..\MatchPoints\MatchPoints.h"

using namespace std;
using namespace cv;

// CMatchPointsTestDlg dialog
class CMatchPointsTestDlg : public CDialogEx
{
// Construction
public:
	CMatchPointsTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MATCHPOINTSTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnProc();
	void ShowPoints(Mat &src, vector<vector<Point64>> vec, double ratio, Point64 shift, Scalar colar = Scalar::all(255));

};
