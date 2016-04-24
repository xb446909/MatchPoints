
// MatchPointsTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MatchPointsTest.h"
#include "MatchPointsTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "opencv_core2411.lib")
#pragma comment(lib, "opencv_highgui2411.lib")
#pragma comment(lib, "..\\Release\\MatchPoints.lib")

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMatchPointsTestDlg dialog




CMatchPointsTestDlg::CMatchPointsTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMatchPointsTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMatchPointsTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMatchPointsTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PROC, &CMatchPointsTestDlg::OnBnClickedBtnProc)
END_MESSAGE_MAP()


// CMatchPointsTestDlg message handlers

BOOL CMatchPointsTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMatchPointsTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMatchPointsTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMatchPointsTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMatchPointsTestDlg::ShowPoints(Mat &src, vector<vector<Point64>> vec, double ratio, Point64 shift, Scalar colar)
{
	for (size_t i = 0; i < vec.size(); i++)
	{
		for (size_t j = 0; j < vec[i].size(); j++)
		{
			circle(src, Point(int((vec[i][j].x + shift.x) * ratio), int((vec[i][j].y + shift.y) * ratio)), 1, colar, -1);
		}
	}
}


void CMatchPointsTestDlg::OnBnClickedBtnProc()
{
	// TODO: Add your control notification handler code here
	CString path;
	GetDlgItemText(IDC_MFCEDITBROWSE_PATH, path);

	CString str;

	FILE* pfile;
	Point64 pt;
	if (!fopen_s(&pfile, CW2A(path), "r"))
	{
		vector<Point64> vec_point;
		fseek(pfile, 0L, SEEK_SET);
		while (!feof(pfile))
		{
			if (fscanf_s(pfile, "%lf%lf", &pt.x, &pt.y) != EOF)
			{
				vec_point.push_back(pt);
			}
		}
		fclose(pfile);

		ClearPoints(1);
		for(int i = 0; i < vec_point.size() - 1; i++)
		{
			SetPoints(1, vec_point[i].x, vec_point[i].y, 0);
		}
		OpenLog();
		SetPoints(1, vec_point[vec_point.size() - 1].x, vec_point[vec_point.size() - 1].y, 1);
		str.Format(L"%d", MatchFromFile("E:\\XB\\CPU\\StandardPoints\\area1.txt", 1, 0.04, 90));
		MessageBox(str);
		CloseLog();

		CString str;
		vector<vector<Point64>> ccdpt, stdpt;
		vector<Point64> sub_vec;
		Point64 pt;
		int c = GetPointsCount(1);
		for (int i = 0; i < c; i++)
		{
			GetAllPoints(1, i, pt.x, pt.y);
			sub_vec.push_back(pt);
		}
		ccdpt.push_back(sub_vec);

		sub_vec.clear();
		c = GetPointsCount(2);
		for (int i = 0; i < c; i++)
		{
			GetAllPoints(2, i, pt.x, pt.y);
			sub_vec.push_back(pt);
		}
		stdpt.push_back(sub_vec);

		Point64 shift;
		shift.x = 10;
		shift.y = 0;

		Mat src(680, 1350, CV_8UC3, Scalar(0));
		ShowPoints(src, ccdpt, 20, shift, Scalar(0, 0, 255));
		ShowPoints(src, stdpt, 20, shift);
		imshow("Point", src);

	}
}
