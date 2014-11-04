
// FMPhotoEditorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FMPhotoEditor.h"
#include "FMPhotoEditorDlg.h"
#include "afxdialogex.h"
#include "PhotoToHistogramConvert.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "CvvImage.h"

#ifndef __SQLITE_NOT_USE__
#include "CppSQLite3U.h"
#endif

#include <opencv2\highgui\\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv_modules.hpp>
#include <opencv2\stitching\detail\exposure_compensate.hpp>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;
using namespace cv::detail;


#define OPENCV_ROOT		"C:/Program Files/opencv/sources"			
//#define TEN_OVER


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#if 0
IplImage	* srcImage;
IplImage	* dsrImage;
Mat			srcMat;
Mat			dsrMat;
Mat			resultMat;
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CFMPhotoEditorDlg 대화 상자



CFMPhotoEditorDlg::CFMPhotoEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFMPhotoEditorDlg::IDD, pParent)
	, imgFileName(_T(""))
	, m_strBlueEditor(_T(""))
	, m_strGreenEditor(_T(""))
	, m_strRedEditor(_T(""))
	, m_strAvageEditor(_T(""))
	, m_exposureValue(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFMPhotoEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SRC_PICTURECONTROL, picSrcPicture);
	DDX_Control(pDX, IDC_SRC_HISTOGRAM, picSrcHistorgamPicture);
	DDX_Control(pDX, IDC_IMGNAME_TEXTCONTROL, txtFileName);
	DDX_Text(pDX, IDC_IMGNAME_TEXTCONTROL, imgFileName);
	DDX_Control(pDX, IDC_BTN_HISTOGRAM, btnHistogram);
	DDX_Text(pDX, IDC_EXPOSURE_EDIT, m_exposureValue);
	DDX_Control(pDX, IDC_EXPOSURESLIDER, m_exposureSliderCtrl);
}

BEGIN_MESSAGE_MAP(CFMPhotoEditorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDOK, &CFMPhotoEditorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_HISTOGRAM, &CFMPhotoEditorDlg::OnBnClickedBtnHistogram)
	ON_BN_CLICKED(IDC_BTN_EXPOSURE, &CFMPhotoEditorDlg::OnBnClickedBtnExposure)
	ON_BN_CLICKED(IDC_BUTTON2, &CFMPhotoEditorDlg::OnBnClickedButton2)
//	ON_BN_CLICKED(IDC_RGBTOHSV, &CFMPhotoEditorDlg::OnBnClickedRgbtohsv)
	ON_BN_CLICKED(IDC_FACEFIND, &CFMPhotoEditorDlg::OnBnClickedFacefind)
//	ON_BN_CLICKED(IDC_HISTEQUALIZATION, &CFMPhotoEditorDlg::OnBnClickedHistequalization)
	ON_BN_CLICKED(IDC_FOLDER_SELECT, &CFMPhotoEditorDlg::OnBnClickedFolderSelect)
	ON_BN_CLICKED(IDC_RGBDATA_CONF, &CFMPhotoEditorDlg::OnBnClickedRgbdataConf)
END_MESSAGE_MAP()


// CFMPhotoEditorDlg 메시지 처리기

void CFMPhotoEditorDlg::OnClose()
{
#ifndef __SQLITE_NOT_USE__
	m_db.close();
#endif
	CDialogEx::OnClose();

}

BOOL CFMPhotoEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

#ifndef __SQLITE_NOT_USE__
	CString FilePath = _T("Database/NajakPhoto.sqlite");
	m_db.open(FilePath);
#endif

	m_exposureSliderCtrl.SetRange(0, 100);
	m_exposureSliderCtrl.SetPos(50);

	int nPos = m_exposureSliderCtrl.GetPos();
	CString str;
	str.Format(_T("%d"), nPos);
	m_exposureValue.SetString(str);
	UpdateData(false);

	m_exposureSliderCtrl.setExposureSlider(&m_exposureSliderCtrl);

	blueArray.SetSize(513);
	greenArray.SetSize(513);
	redArray.SetSize(513);

	X_Position_High.SetSize(X_ARRAY_VALUE);
	X_Position_Normal.SetSize(X_ARRAY_VALUE);
	X_Position.SetSize(X_ARRAY_VALUE);

	shadowMoveValue = 0;
	whiteMoveValue = 0;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFMPhotoEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFMPhotoEditorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFMPhotoEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

string utf16ToUTF8(const wstring &s)
{
	const int size = ::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, NULL, 0, 0, NULL);
	vector<char> buf(size);
	::WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, &buf[0], size, 0, NULL);

	return string(&buf[0]);
}

void CFMPhotoEditorDlg::MatFileSaved(CString readImgFile, CString currentTimeID, Mat readMat, CString readFilePath)
{
	CString _strExtension;
	CString _imgFileName;
	CString _strFileID;


	CFileFind _file;
	CString _strFile = _T("*.*");
	BOOL bResult = _file.FindFile(readFilePath + _strFile);

	if (!bResult)
	{
		bResult = CreateDirectory(readFilePath, NULL);
	}

	if (bResult)
	{
		CString _fileName;
		_fileName.Format(_T("%s/%s_%s"), readFilePath, currentTimeID, readImgFile);
		int strlen = WideCharToMultiByte(CP_ACP, 0, _fileName, -1, NULL, 0, NULL, NULL);
		char * _savedFileName = new char[strlen + 1];
		WideCharToMultiByte(CP_ACP, 0, _fileName, -1, _savedFileName, strlen, NULL, NULL);
		imwrite(_savedFileName, readMat);

		delete[] _savedFileName;
	}
}


/* File Name & Extension Save*/
bool CFMPhotoEditorDlg::ImageFileSaved(CString readImgFile, CString currentTimeID, IplImage * readImage, CString readFilePath)
{
	CString _strExtension;
	CString _imgFileName;
	CString _strFileID;


	CFileFind _file;
	CString _strFile = _T("*.*");
	BOOL bResult = _file.FindFile(readFilePath + _strFile);

	if (!bResult)
	{
		bResult = CreateDirectory(readFilePath, NULL);
	}

	if (bResult)
	{
		CString _fileName;
		_fileName.Format(_T("%s/%s_%s"), readFilePath, currentTimeID, readImgFile);
		int strlen = WideCharToMultiByte(CP_ACP, 0, _fileName, -1, NULL, 0, NULL, NULL);
		char * _savedFileName = new char[strlen + 1];
		WideCharToMultiByte(CP_ACP, 0, _fileName, -1, _savedFileName, strlen, NULL, NULL);
		cvSaveImage(_savedFileName, readImage);
	
		delete[] _savedFileName;
	}


#ifndef __SQLITE_NOT_USE__
	int extensionIndex = readImgFile.ReverseFind('.');

	if (extensionIndex)
	{
		_strExtension = readImgFile.Right(readImgFile.GetLength() - extensionIndex - 1);
		_imgFileName = readImgFile.Left(readImgFile.GetLength() - _strExtension.GetLength() - 1);
		_strFileID = _imgFileName + "_" + currentTimeID;

		str_FileID = _strFileID;


		CppSQLite3Query query;
		CString _insertQuery;
		_insertQuery.Format(_T("INSERT INTO PHOTO (p_FileWidth, p_FileHeight, p_FileWidthStep, p_FileChannles,p_FileName,p_FileFormat,p_FileID, p_originalFilePath, p_FileDepth) VALUES ('%d','%d','%d','%d','%s', '%s', '%s','%s', %d)"), readImage->width, readImage->height, readImage->widthStep, readImage->nChannels, _imgFileName,
			_strExtension.MakeUpper(), _strFileID, readFilePath, readImage->depth);
		query = m_db.execQuery(_insertQuery);

		return true;
	}
#endif

	return false;
}




void CFMPhotoEditorDlg::OnPhotoView(IplImage * viewIplImage)
{
	int		_rows = viewIplImage->width;
	int		_cols = viewIplImage->height;

	bool	_horizontal = _rows > _cols ? 1 : 0;
	CRect	_rect;
	int		MAX_LENGTH = 200;
	int		_difference = 0;

	if (_horizontal)				// 가로가 더 길다
	{
		_difference = _rows - _cols;
	}
	else
	{
		_difference = _cols - _rows;
	}
	if (_difference == 0)
		_rect = CRect(0, 0, 329, 320);
	else 
		_rect = CRect(0, 0, MAX_LENGTH * (_rows / _difference), MAX_LENGTH * (_cols / _difference));

	picSrcPicture.GetClientRect(&_rect);
	CDC * pDC = picSrcPicture.GetDC();
	CvvImage cvv_Img;
	cvv_Img.CopyOf(srcImage);
	cvv_Img.DrawToHDC(pDC->GetSafeHdc(), &_rect);
}

void CFMPhotoEditorDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG | All Files(*.*)|*.*||"), this);

	if (dlgFile.DoModal() == IDOK)
	{
		savedImgName = dlgFile.GetFileName();
		CT2CA strAtl(dlgFile.GetPathName());
		string strImagePath(strAtl);

		CString _str = dlgFile.GetPathName();
		CStringA _strA(_str);
		LPCSTR _szTemp = _strA;

		SetWindowTextA(txtFileName, _szTemp);

		srcMat = imread(strImagePath, CV_LOAD_IMAGE_COLOR);

		if (!srcMat.data)
			return;

		IplImage _srcMatToImage = (IplImage)srcMat;
		srcImage = &_srcMatToImage;
		OnPhotoView(srcImage);

		CPhotoToHistogramConvert * pHistogramConvert = new CPhotoToHistogramConvert;
		pHistogramConvert->ImageToHistogram(savedImgName, srcMat,true);

		delete pHistogramConvert;

		PhotoImageToAutoWhiteBalance(savedImgName, srcMat);
	}
}

void grayConversion(IplImage * srcImage1, float * ml, float * ma, float * mb, int p)
{
	*ma = 0;
	*mb = 0;
	*ml = 0;

	int nldx = 0;

	for (int i = 0; i < srcImage1->height; i++)
	{
		for (int j = 0; j < srcImage1->width; j++)
		{
			nldx = i * srcImage1->widthStep + j * srcImage1->nChannels;
#if 1
			float lc = pow(srcImage1->imageData[nldx], p);
			float ac = pow(srcImage1->imageData[nldx + 1], p);
			float bc = pow(srcImage1->imageData[nldx + 2], p);
#else
			Vec3b v1 = src1.at<Vec3b>(i, j);

			float lc = pow(v1.val[0], p);			// Blue
			float ac = pow(v1.val[1], p);			// Green
			float bc = pow(v1.val[2], p);			// Red
#endif
			*ma = *ma + ac;
			*mb = *mb + bc;
			*ml = *ml + lc;
		}
	}

	*ma = pow((float)*ma / (srcImage1->height *srcImage1->width), (float)1 / p);
	*mb = pow((float)*mb / (srcImage1->height *srcImage1->width), (float)1 / p);
	*ml = pow((float)*ml / (srcImage1->height *srcImage1->width), (float)1 / p);
}


int CFMPhotoEditorDlg::RGBCalculateDifference(int blueValue, int greenValue, int redValue, int second_blueValue, int second_greenValue, int second_redValue)
{
	int		exposureMoveValue = 0;

	if ((blueValue > greenValue) && (blueValue > redValue))				// Blue 값이 제일 높다.
	{
		exposureMoveValue = blueValue;
		MaxColorRGB = BLUECOLOR;
	}
	else if ((redValue > greenValue) && (redValue > blueValue))			// Red 값이 제일 높다
	{
		exposureMoveValue = redValue;
		MaxColorRGB = REDCOLOR;
	}
	else if ((greenValue > redValue) && (greenValue >= blueValue))		// Green 값이 제일 높다
	{
		exposureMoveValue = greenValue;
		MaxColorRGB = GREENCOLOR;
	}
	else
	{
		if (redValue == greenValue && redValue == blueValue)
		{
			if ((second_blueValue > second_greenValue) && (second_blueValue > second_redValue))				// Blue 값이 제일 높다.
			{
				exposureMoveValue = second_blueValue;
				MaxColorRGB = BLUECOLOR;
			}
			else if ((second_redValue >= second_greenValue) && (second_redValue > second_blueValue))			// Red 값이 제일 높다
			{
				exposureMoveValue = second_redValue;
				MaxColorRGB = REDCOLOR;
			}
			else if ((second_greenValue >= second_redValue) && (second_greenValue > second_blueValue))		// Green 값이 제일 높다
			{
				exposureMoveValue = second_greenValue;
				MaxColorRGB = GREENCOLOR;
			}
		}
	}

	return exposureMoveValue;
}

float CFMPhotoEditorDlg::AutoWhiteBalanceRGB()
{
	float alpha = 0;
	int	exposureMoveValue = 0;

	MaxColorRGB = BLUECOLOR;

	int		_blue_X = blueMax_X * 2;
	int		_green_X = greenMax_X * 2;
	int		_red_X = redMax_X * 2;
	unsigned char		_maxIndex = 0;

	CUIntArray	_rgbArray;

	_rgbArray.SetSize(3);				// 0 = blue, 1 = green, 2 = red

	int		_currentValue = 0;
	int		_greenValue = 0;
	int		_redValue = 0;

	if (_blue_X < blueArray.GetSize())
	{
		_rgbArray[BLUECOLOR] = blueArray[_blue_X];
		_currentValue = blueArray[_blue_X];
	}

	if (_green_X < greenArray.GetSize())
	{
		_rgbArray[GREENCOLOR] = greenArray[_green_X];
		_greenValue = greenArray[_green_X];
	}
	if (_red_X < redArray.GetSize())
	{
		_rgbArray[REDCOLOR] = redArray[_red_X];
		_redValue = redArray[_red_X];
	}
	for (int i = GREENCOLOR; i < _rgbArray.GetSize(); i++)
	{
		if (_currentValue < _rgbArray[i])
		{
			_maxIndex = i;
			_currentValue = _rgbArray[i];
		}
	}
#if 1
	TRACE("\n _rgbArray[BLUECOLOR]=%d, _rgbArray[GREENCOLOR]=%d, _rgbArray[REDCOLOR]=%d\n", _rgbArray[BLUECOLOR], _rgbArray[GREENCOLOR], _rgbArray[REDCOLOR]);
	TRACE("\n redMax_X=%d, greenMax_X=%d, blueMax_X=%d\n", redMax_X, greenMax_X, blueMax_X);
	TRACE("\n RedColor_Second_X=%d, GreenColor_Second_X=%d, BlueColor_Second_X=%d\n", redMax_Second_X, greenMax_Second_X, blueMax_Second_X);

	for (int i = 0; i < X_Position_High.GetSize(); i++)
	{
		TRACE("\n X_Position_High[%d]=%d\n", i, X_Position_High[i]);
	}

	for (int i = 0; i < X_Position_Normal.GetSize(); i++)
	{
		TRACE("\n X_Position_Normal[%d]=%d\n", i, X_Position_Normal[i]);
	}

	for (int i = 0; i < X_Position.GetSize(); i++)
	{
		TRACE("\n X_Position[%d]=%d\n", i, X_Position[i]);
	}

#endif

	if (_maxIndex == BLUECOLOR)
	{
		exposureMoveValue = RGBCalculateDifference(blueMax_X, greenMax_X, redMax_X, blueMax_Second_X, greenMax_Second_X, redMax_Second_X);
	}
	else if (_maxIndex == GREENCOLOR)								// Max Color => Green
	{
		exposureMoveValue = _rgbArray[GREENCOLOR];
		MaxColorRGB = GREENCOLOR;
	}
	else if (_maxIndex == REDCOLOR)									// Max Color => Red
	{
		exposureMoveValue = _rgbArray[REDCOLOR];
		MaxColorRGB = REDCOLOR;
	}

	int		alphaMax = -1;
	int		_MaxAlpha = X_Position_Normal[0];
	int		_SecondAlpha = 0;

	int		_zeroAlphaCount = 0;

	if (X_Position_Normal[0] == 0)
		_zeroAlphaCount++;

#if 1
	for (int i = 1; i < X_Position_Normal.GetSize(); i++)
	{
		int		_currentPosition = X_Position_Normal[i];

		if (X_Position_Normal[i] != 0)
		{
			if (_MaxAlpha <= X_Position_Normal[i])
			{
				alphaMax = i;
				_SecondAlpha = _MaxAlpha;
				_MaxAlpha = X_Position_Normal[i];
			}
			else
			{
				if (_SecondAlpha != 0)
				{
					if (_MaxAlpha <= (X_Position_Normal[i] * 2))
					{
						alphaMax = i;
						_SecondAlpha = X_Position_Normal[i];
					}
					else if ((_MaxAlpha - 5) <= X_Position_Normal[i])
					{
						alphaMax = i;
						_SecondAlpha = X_Position_Normal[i];
					}
				}
				else if (_SecondAlpha <= X_Position_Normal[i])
				{
					alphaMax = i;
					_SecondAlpha = X_Position_Normal[i];
				}
			}
		}
		else if (X_Position_Normal[i] == 0)
			_zeroAlphaCount++;
	}
#else
	for (int i = 1; i < X_Position_Normal.GetSize(); i++)
	{
		int		_currentPosition = X_Position_Normal[i];

		if (X_Position_Normal[i] != 0)
		{
			if (_MaxAlpha <= X_Position_Normal[i])
			{
				alphaMax = i;
				_SecondAlpha = _MaxAlpha;
				_MaxAlpha = X_Position_Normal[i];
			}
			else
			{
				if (_SecondAlpha != 0)
				{
					if (_MaxAlpha <= (X_Position_Normal[i] * 2))
					{
						alphaMax = i;
						_SecondAlpha = X_Position_Normal[i];
					}
					else if ((_MaxAlpha - X_REFERENCE_POINT) <= X_Position_Normal[i])
					{
						alphaMax = i;
						_SecondAlpha = X_Position_Normal[i];
					}
				}
				else if (_SecondAlpha <= X_Position_Normal[i])
				{
					alphaMax = i;
					_SecondAlpha = X_Position_Normal[i];
				}
			}
		}
		else if (X_Position_Normal[i] == 0)
			_zeroAlphaCount++;
	}
#endif

	if (alphaMax == -1 && X_Position_Normal[0] == 0)
	{
		if (_zeroAlphaCount == X_Position_Normal.GetSize())
		{
			alphaMax = LABEL_5;
			_MaxAlpha = X_Position_High[LABEL_5];
		}
		for (int i = 0; i < X_Position_High.GetSize(); i++)
		{
			if ((_MaxAlpha <= X_Position_High[i]) && (X_Position_High[i] != 0))
			{
				alphaMax = i;
				_MaxAlpha = X_Position_High[i];
			}
		}
	}
	else if (alphaMax == -1 && X_Position_Normal[0] != 0)
		alphaMax = LABEL_1;
	else if (alphaMax <= LABEL_5 && X_Position_Normal[X_Position_Normal.GetSize() - 1] != 0)
		alphaMax =  LABEL_6;


	switch (alphaMax)
	{
		case LABEL_1:
		case LABEL_2:
			alpha = ALPHA_LABEL_1;
			break;
		case LABEL_3:
			alpha = ALPHA_LABEL_2;
			break;
		case LABEL_4:
			alpha = ALPHA_LABEL_3;
			break;
		case LABEL_5:
			alpha = ALPHA_LABEL_4;
			break;
		case LABEL_6:
			alpha = ALPHA_LABEL_5;
			break;
		default:
			alpha = ALPHA_LABEL_ZERO;
			break;
	}

	return alpha;
}

inline Size getContinuousSize(const Mat& m1, const Mat& m2, int widthScale = 1)
{
	return (m1.flags & m2.flags & Mat::CONTINUOUS_FLAG) != 0 ?
		Size(m1.cols*m1.rows*widthScale, 1) : Size(m1.cols*widthScale, m1.rows);
}



void CFMPhotoEditorDlg::convertTo(OutputArray _dst, int _type, double alpha, double beta)
{
	bool noScale = fabs(alpha - 1) < DBL_EPSILON && fabs(beta) < DBL_EPSILON;

	if (_type < 0)
		_type = _dst.fixedType() ? _dst.type() : srcMat.type();

	int sdepth = srcMat.depth(), ddepth = CV_MAT_DEPTH(_type);
	if (sdepth == ddepth && noScale)
	{
		srcMat.copyTo(_dst);
		return;
	}

	Mat src = srcMat;

	BinaryFunc func = noScale ? getConvertFunc(sdepth, ddepth) : getConvertScaleFunc(sdepth, ddepth);
	double scale[] = { alpha, beta};
	int cn = srcMat.channels();
	CV_Assert(func != 0);


	if (srcMat.dims <= 2)
	{
		_dst.create(srcMat.size(), _type);
		Mat dst = _dst.getMat();
		Size sz = getContinuousSize(src, dst, cn);
		func(src.data, src.step, 0, 0, dst.data, dst.step, sz, scale);
	}
	else
	{
		_dst.create(srcMat.dims, srcMat.size, _type);
		Mat dst = _dst.getMat();
		const Mat* arrays[] = { &src, &dst, 0 };
		uchar* ptrs[2];
		NAryMatIterator it(arrays, ptrs);
		Size sz((int)(it.size*cn), 1);

		for (size_t i = 0; i < it.nplanes; i++, ++it)
			func(ptrs[0], 0, 0, 0, ptrs[1], 0, sz, scale);
	}
}

IplImage * CFMPhotoEditorDlg::PhotoBlackPoint(IplImage * _srcImage, InputArray _blueSrc, InputArray _greenSrc, InputArray _redSrc, double _blueMin, double _greenMin, double _redMin)
{
	IplImage *			_dscImage = cvCreateImage(cvGetSize(_srcImage), _srcImage->depth, _srcImage->nChannels);

	/* blue */
	IplImage _blueMat = (IplImage)_blueSrc.getMat();
	IplImage * _blueImage = &_blueMat;

	int		_ws = _blueImage->widthStep;
	int		_channel = _blueImage->nChannels;
	int		nldx;

	for (int i = 0; i < _blueImage->height; i++)
	{
		for (int j = 0; j < _blueImage->width; j++)
		{
			nldx = i * _ws + j * _channel;

			unsigned char _color = _blueImage->imageData[nldx];

			if (_color < 127 && _color > _blueMin)
				_blueImage->imageData[nldx] = _color - _blueMin;
		}
	}

	/* green */
	IplImage _greenMat = (IplImage)_greenSrc.getMat();
	IplImage * _greenImage = &_greenMat;

	_ws = _greenImage->widthStep;
	_channel = _greenImage->nChannels;

	for (int i = 0; i < _greenImage->height; i++)
	{
		for (int j = 0; j < _greenImage->width; j++)
		{
			nldx = i * _ws + j * _channel;

			unsigned char _color = _greenImage->imageData[nldx];

			if (_color < 127 && _color > _greenMin)
				_greenImage->imageData[nldx] = _color - _greenMin;
		}
	}

	/* red */
	IplImage _redMat = (IplImage)_redSrc.getMat();
	IplImage * _redImage = &_redMat;

	_ws = _redImage->widthStep;
	_channel = _redImage->nChannels;

	for (int i = 0; i < _redImage->height; i++)
	{
		for (int j = 0; j < _redImage->width; j++)
		{
			nldx = i * _ws + j * _channel;

			unsigned char _color = _redImage->imageData[nldx];

			if (_color < 127 && _color > _redMin)
				_redImage->imageData[nldx] = _color - _redMin;
		}
	}

	cvMerge(_blueImage, _greenImage, _redImage, 0, _dscImage);

	cvSaveImage("exposures_111.jpg", _dscImage);


	return  _dscImage;
}

void CFMPhotoEditorDlg::PhotoNormalize(InputArray _src, OutputArray _dst, double a, double b, int norm_type, int rtype, InputArray _mask)
{
	Mat src = _src.getMat();

#if 1
	Mat dst = _dst.getMat();

	IplImage _srcMatToImage = (IplImage)src;
	IplImage * _srcImage = &_srcMatToImage;

	int		_ws = _srcImage->widthStep;
	int		_channel = _srcImage->nChannels;
	int		nldx;

	for (int i = 0; i < _srcImage->height; i++)
	{
		for (int j = 0; j < _srcImage->width; j++)
		{
			nldx = i * _ws + j * _channel;

			unsigned char _color = _srcImage->imageData[nldx];
	
			if (_color < 127 && _color > a)
				_srcImage->imageData[nldx] = _color - a;
		}
	}
	dst = Mat(_srcImage, true);
#if 0
	for (int i = 0; i < src.cols; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			int value = src.at<uchar>(j, i);

			if (value < 50 && value > a)
				dst.at<uchar>(j, i) = value - a;
			else
				dst.at<uchar>(j, i) = value;
		}
		
	}
#endif

#else
	Mat mask = _mask.getMat();
	double scale = 1, shift = 0;

	if (norm_type == CV_MINMAX)
	{
		double smin = 0, smax = 0;
		double dmin = MIN(a, b), dmax = MAX(a, b);
		minMaxLoc(_src, &smin, &smax, 0, 0, mask);
		scale = (dmax - dmin)*(smax - smin > DBL_EPSILON ? 1. / (smax - smin) : 0);
		shift = dmin - smin*scale;
	}
	else if (norm_type == CV_L2 || norm_type == CV_L1 || norm_type == CV_C)
	{
		scale = norm(src, norm_type, mask);
		scale = scale > DBL_EPSILON ? a / scale : 0.;
		shift = 0;
	}
	else
		CV_Error(CV_StsBadArg, "Unknown/unsupported norm type");

	if (rtype < 0)
		rtype = _dst.fixedType() ? _dst.depth() : src.depth();

	_dst.create(src.dims, src.size, CV_MAKETYPE(rtype, src.channels()));
	Mat dst = _dst.getMat();

	if (!mask.data)
		src.convertTo(dst, rtype, scale, shift);
	else
	{
		Mat temp;
		src.convertTo(temp, rtype, scale, shift);
		temp.copyTo(dst, mask);
	}
#endif
}


void CFMPhotoEditorDlg::PhotoImageExposureEffect(CString fileName, IplImage * _srcImage, int RGBColor, float alpha)		/* RGBColor ( 0-Blue, 1-Green, 2-Red) */
{
	CTime _time = CTime::GetCurrentTime();
	ImageFileSaved(fileName, _time.Format("%y%m%d_%H%M%S"), _srcImage, _T("__WhiteBalance"));

	Mat _srcMat = cvarrToMat(_srcImage);
	Mat	_dscMat = Mat::zeros(_srcMat.size(), _srcMat.type());

	int		_channel = _srcImage->nChannels;
	int		nldx;
	int		_ws = _srcImage->widthStep;

	if (alpha != (float)ALPHA_LABEL_ZERO)
	{
		if (alpha == (float)ALPHA_LABEL_1)
		{
			_srcMat.convertTo(_dscMat, -1, alpha, BETA_LABEL_1);
		}
		else if (alpha == (float)ALPHA_LABEL_2)
		{
			_srcMat.convertTo(_dscMat, -1, alpha, BETA_LABEL_2);
		}
		else if (alpha == (float)ALPHA_LABEL_3)
		{
			_srcMat.convertTo(_dscMat, -1, alpha, BETA_LABEL_3);
		}
		else if (alpha == (float)ALPHA_LABEL_4)
		{
			_srcMat.convertTo(_dscMat, -1, alpha, BETA_LABEL_4);
		}
		else if (alpha == (float)ALPHA_LABEL_5)
		{
			_srcMat.convertTo(_dscMat, -1, alpha, BETA_LABEL_5);
		}
#if 1
		int		N = _dscMat.rows*_dscMat.cols;
		float	s1 = 1.5, s2 = 1.5;

		vector<Mat> bgr_planes;

		split(_dscMat, bgr_planes);

		int		histSize = MAX_RGB_COLOR + 1;
		float	range[] = { 0, MAX_RGB_COLOR + 1};
		const	float * histRange = { range };

		bool uniform = true, accumulate = false;

		Mat		b_hist, g_hist, r_hist;
		float	cbhist[MAX_RGB_COLOR + 1], cghist[MAX_RGB_COLOR + 1], crhist[MAX_RGB_COLOR + 1];
		int		vmin1 = 0, vmin2 = 0, vmin3 = 0;
		int		vmax1 = MAX_RGB_COLOR, vmax2 = MAX_RGB_COLOR, vmax3 = MAX_RGB_COLOR;

		vmax2 = vmax1;
		vmax3 = vmax1;

		calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);

		for (int i = 0; i < histSize; i++)
		{
			if (i == 0)
			{
				cbhist[i] = b_hist.at<float>(i);
				cghist[i] = g_hist.at<float>(i);
				crhist[i] = r_hist.at<float>(i);
			}
			else
			{
				cbhist[i] = (cbhist[i - 1] + b_hist.at<float>(i));
				cghist[i] = (cghist[i - 1] + g_hist.at<float>(i));
				crhist[i] = (crhist[i - 1] + r_hist.at<float>(i));
			}
		}

		/* Blue Min Value*/
		while (vmin1 < histSize - 1 && cbhist[vmin1] <= (float)N * s1 / 100)
		{
			vmin1 = vmin1 + 1;
		}

		/* Green Min Value */
		while (vmin2 < histSize - 1 && cghist[vmin2] <= (float)N * s1 / 100)
		{
			vmin2 = vmin2 + 1;
		}

		/* Red Min Value */
		while (vmin3 < histSize - 1 && crhist[vmin3] <= (float)N * s1 / 100)
		{
			vmin3 = vmin3 + 1;
		}

#if 0
		/* Blue Max Value */
		while (vmax1 < histSize && cbhist[vmax1] >(float)N * (1 - s2 / 100))
		{
			vmax1 = vmax1 - 1;
		}

		if (vmax1 < histSize - 1)
			vmax1 = vmax1 + 1;

		/* Green Max Value */
		while (vmax2 < histSize && cghist[vmax2] >(float)N * (1 - s2 / 100))
		{
			vmax2 = vmax2 - 1;
		}

		if (vmax2 < histSize - 1)
			vmax2 = vmax2 + 1;

		/* Red Max Value */
		while (vmax3 < histSize && crhist[vmax3] >(float)N * (1 - s2 / 100))
		{
			vmax3 = vmax3 - 1;
		}

		if (vmax3 < histSize - 1)
			vmax3 = vmax3 + 1;
#endif

#if 1

#if 0
		cvSaveImage("exposure.jpg", PhotoBlackPoint(_srcImage, bgr_planes[0], bgr_planes[1], bgr_planes[2], (vmin1 - (vmin1 * 0.8)), (vmin2 - (vmin2 * 0.8)), (vmin3 - (vmin3 * 0.8))));
#else
		PhotoNormalize(bgr_planes[0], bgr_planes[0], (vmin1 - (vmin1 * 0.8)), 255, NORM_MINMAX, -1, Mat());			// blue
		PhotoNormalize(bgr_planes[1], bgr_planes[1], (vmin2 - (vmin2 * 0.8)), 255, NORM_MINMAX, -1, Mat());			// green
		PhotoNormalize(bgr_planes[2], bgr_planes[2], (vmin3 - (vmin3 * 0.8)), 255, NORM_MINMAX, -1, Mat());			// red

		merge(bgr_planes, _dscMat);
		imwrite("exposure.jpg", _dscMat);
#endif

#else
		normalize(bgr_planes[0], bgr_planes[0], (vmin1 - (vmin1 * 0.8)), 255, NORM_MINMAX,-1, Mat());			// blue
		normalize(bgr_planes[1], bgr_planes[1], (vmin2 - (vmin2 * 0.8)), 255, NORM_MINMAX, -1, Mat());			// green
		normalize(bgr_planes[2], bgr_planes[2], (vmin3 - (vmin3 * 0.8)), 255, NORM_MINMAX, -1, Mat());			// red
#endif

#else
		imwrite("exposure.jpg", _dscMat);
#endif
	

	}
	return;
}

double xyz_to_lab(double c)
{
	return c > 216.0 / 24389.0 ? pow(c, 1.0 / 3.0) : c * (841.0 / 108.0) + (4.0 / 29.0);
}

void CFMPhotoEditorDlg::ImageResizeToSaveImage(CString fileName,IplImage * srcImage)
{
	CString  _fileName;

	_fileName.Format(_T("%s"), fileName);

	int		strlen = WideCharToMultiByte(CP_ACP, 0, _fileName, -1, NULL, 0, NULL, NULL);
	char	* _savedFileName = new char[strlen + 1];
	WideCharToMultiByte(CP_ACP, 0, _fileName, -1, _savedFileName, strlen, NULL, NULL);

#if 0
	Mat _savedMat = Mat(srcImage, true);

	vector<int> params;
	params.push_back(CV_IMWRITE_JPEG_QUALITY);
	params.push_back(35);
	imwrite(_savedFileName, _savedMat, params);
#else


	int		width = srcImage->width;
	int		height = srcImage->height;
	int		MAX_LENGTH = 80;

	if (width + height > 400)
	{
		bool	_horizontal = width > height ? true : false;
		int		_difference = 0;

		if (_horizontal)
		{
			_difference = width - height;
		}
		else
		{
			_difference = height - width;
		}

		width = MAX_LENGTH * (width / _difference);
		height = MAX_LENGTH * (height / _difference);
	}

	IplImage * dstImage = cvCreateImage(cvSize(width, height), srcImage->depth, srcImage->nChannels);

	cvResize(srcImage, dstImage, CV_INTER_CUBIC);

	cvSaveImage(_savedFileName, dstImage);

	cvReleaseImage(&dstImage);
#endif
}

void CFMPhotoEditorDlg::PhotoImageToAutoWhiteBalance(CString fileName, Mat readMat)
{
	if (readMat.data)
	{

#if 0
		vector<Mat> colors;
		split(readMat, colors);

		colors[0] += 69;			// blue
		colors[1] += 40;			// green
		colors[2] -= 23;			// red

		merge(colors, readMat);
		imwrite("color_balace.jpg", readMat);
#endif

		IplImage _srcMatToImage = (IplImage)readMat;
		IplImage * _srcImage = &_srcMatToImage;

#if 0
		vector<int> params;
		params.push_back(CV_IMWRITE_JPEG_QUALITY);
		params.push_back(35);
		imwrite("my.jpg", readMat, params);
#endif

#if 0				// Resize 에 대한 Test Code로 사용 후 바로 #if 1 > 0 으로 변환 요청
		int		extensionIndex = fileName.ReverseFind('.');
		CString _imgOnlyFileName;

		if (extensionIndex)
		{
			CString _extensionStr = fileName.Right(fileName.GetLength() - extensionIndex - 1);

			_imgOnlyFileName.Format(_T("%s_1.%s"), fileName.Left(fileName.GetLength() - _extensionStr.GetLength() - 1), _extensionStr);
		}

		ImageResizeToSaveImage(_imgOnlyFileName, _srcImage);

#endif

		int		_ws = _srcImage->widthStep;
		int		_channel = _srcImage->nChannels;
		int		nldx;

		float alpha = AutoWhiteBalanceRGB();

		if (MaxColorRGB == BLUECOLOR)
		{
			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _blue = _srcImage->imageData[nldx + BLUECOLOR];
					unsigned char _green = _srcImage->imageData[nldx + GREENCOLOR];
					unsigned char _red = _srcImage->imageData[nldx + REDCOLOR];

					if (_red > 225)
					{
						if (_red < (MAX_RGB_COLOR - 4))
							_red += 3;
						else
							_red = MAX_RGB_COLOR;
					}
					else
					{
						if (_red < (MAX_RGB_COLOR - 6))
							_red += 5;
						else
							_red = MAX_RGB_COLOR;
					}

					if (_green > 230)
					{
						if (_green < (MAX_RGB_COLOR - 5))
							_green += 4;
						else
							_green = MAX_RGB_COLOR;
					}
					else
					{
						if (_green < (MAX_RGB_COLOR - 6))
							_green += 5;
						else
							_green = MAX_RGB_COLOR;
					}

					_srcImage->imageData[nldx + BLUECOLOR] = _blue;
					_srcImage->imageData[nldx + GREENCOLOR] = _green;
					_srcImage->imageData[nldx + REDCOLOR] = _red;
				}
			}
		}
		else if (MaxColorRGB == REDCOLOR)
		{
			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _blue = _srcImage->imageData[nldx + BLUECOLOR];
					unsigned char _green = _srcImage->imageData[nldx + GREENCOLOR];
					unsigned char _red = _srcImage->imageData[nldx + REDCOLOR];


					if (_red > _blue)
					{
						if (_red > 240)
							_red -= 1;
						else if (_red > 6)
							_red -= 5;
					}

					_srcImage->imageData[nldx + REDCOLOR] = _red;

					if (MAX_RGB_COLOR - 5 > _green)
						_green += 5;
					_srcImage->imageData[nldx + GREENCOLOR] = _green;

					if ((MAX_RGB_COLOR - 20) > _blue)
						_blue += 10;
					else if((MAX_RGB_COLOR - 10) > _blue)
						_blue += 8;
					_srcImage->imageData[nldx + BLUECOLOR] = _blue;
				}
			}
		}
		else if (MaxColorRGB == GREENCOLOR)
		{
			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _blue = _srcImage->imageData[nldx + BLUECOLOR];
					unsigned char _green = _srcImage->imageData[nldx + GREENCOLOR];
					unsigned char _red = _srcImage->imageData[nldx + REDCOLOR];

#if 1
					if (_red < (MAX_RGB_COLOR - 16))
						_red += 15;
					else if (_red < (MAX_RGB_COLOR - 6))
						_red += 5;

					if (_green < (MAX_RGB_COLOR - 10))
						_green += 9;
					else if (_green < (MAX_RGB_COLOR - 3))
						_green += 2;
#else
					if (_red > 225)
					{
						if (_red < (MAX_RGB_COLOR - 6))
							_red += 5;
						else
							_red = MAX_RGB_COLOR;
					}
					else
					{
						if (_red < (MAX_RGB_COLOR - 16))
							_red += 15;
						else
							_red = MAX_RGB_COLOR;
					}

					if (_green > 230)
					{
						if (_green < (MAX_RGB_COLOR - 2))
							_green += 2;
						else
							_green = MAX_RGB_COLOR;
					}
					else
					{
						if (_green < (MAX_RGB_COLOR - 10))
							_green += 10;
					}
#endif
					if (_blue < (MAX_RGB_COLOR - 2))
						_blue += 2;

					_srcImage->imageData[nldx + BLUECOLOR] = _blue;
					_srcImage->imageData[nldx + GREENCOLOR] = _green;
					_srcImage->imageData[nldx + REDCOLOR] = _red;
				}
			}
		}

		PhotoImageExposureEffect(fileName, _srcImage, MaxColorRGB, alpha);

	}
}


int CFMPhotoEditorDlg::ToRGB(double n)
{
	double result = 255.0 * n;

	if (result < 0)
		return 0;
	if (result > 255)
		return 255;

	return result;
}

double CFMPhotoEditorDlg::PivotRgb(double n)
{
	return (0.04045 ? powf((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0;
}

double CFMPhotoEditorDlg::CubicRoot(double n)
{
	return powf(n, 1.0 / 3.0);
}

double CFMPhotoEditorDlg::PivotXyz(double n)
{
	double Epsilon = 0.008856;
	double Kappa = 903.3;

	return n > Epsilon ? CubicRoot(n) : (Kappa * n + 16) / 116;
}

void CFMPhotoEditorDlg::OnBnClickedBtnHistogram()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

#if 1			/* Fletamuto Najak 20140628 */

	int		_height = 5;
	int		_width = 5;

#ifndef __SQLITE_NOT_USE__
	CString _insertQuery;

	m_db.execQuery(_T("begin;"));
#endif

	int			db_index = 0;

	FILE	* fpX, * fpY, * fpZ;

	fopen_s(&fpX, "Najak_X.txt", "w");
	fopen_s(&fpY, "Najak_Y.txt", "w");
	fopen_s(&fpZ, "Najak_Z.txt", "w");

	for (int r = 0; r < 256; r++)
	{
		for (int g = 0; g < 256; g++)
		{
			for (int b = 0; b < 256; b++)
			{
#if 1
				double R = PivotRgb(r / 255.0);
				double G = PivotRgb(g / 255.0);
				double B = PivotRgb(b / 255.0);

				double X = R * 0.4124 + G * 0.3576 + B * 0.1805;
				double Y = R * 0.2126 + G * 0.7152 + B * 0.0722;
				double Z = R * 0.0193 + G * 0.1192 + B * 0.9505;

				double white_X = 95.047;
				double white_Y = 100.000;
				double white_Z = 108.883;

				double tempX = PivotXyz(X / white_X);
				double tempY = PivotXyz(Y / white_Y);
				double tempZ = PivotXyz(Z / white_Z);

				int _L = RoundOff(MAX(0, 116 * tempY - 16), 0);
				int _a = RoundOff(500 * (tempX - tempY), 0);
				int _b = RoundOff(200 * (tempY - tempZ), 0);

				double		_maxRGB = MAX(r, MAX(g, b));
				double		_minRGB = MIN(r, MIN(g, b));
				double		_delRGB = _maxRGB - _minRGB;

				double		_varR = r / 255.0;
				double		_varG = g / 255.0;
				double		_varB = b / 255.0;

				double		_varMin = MIN(_varR, MIN(_varG, _varB));
				double		_varMax = MAX(_varR, MAX(_varG, _varB));
				double		_delMax = _varMax - _varMin;


				int	V = RoundOff((_maxRGB / MAX_RGB_COLOR) * 100.0, 0);
				int H = 0;
				int S = 0;
				double _hue = 0.0;
				double _sat = 0.0;

				if(_delRGB == 0)
				{
					H = 0;
					S = 0;
				}
				else 
				{
					S = RoundOff(_delMax / _varMax * 100.0, 0);

					double delta = _varMax - _varMin;
					double hue = 0.0;

					if (_varR == _varMax)
						hue = (_varG - _varB) / delta;
					else if (_varG == _varMax)
						hue = 2.0 + (_varB - _varR) / delta;
					else if (_varB == _varMax)
						hue = 4.0 + (_varR - _varG) / delta;

					hue *= 60.0;

					if (hue < 0.0)
						hue += 360.0;

					H = RoundOff(hue, 0);
				}

#if 1
				fprintf_s(fpX, "%f,\n", tempX);
				fprintf_s(fpY, "%f,\n", tempY);
				fprintf_s(fpZ, "%f,\n", tempZ);
#else
				_insertQuery.Format(_T("INSERT INTO PhotoRGB (db_index, db_RED, db_GREEN, db_BLUE, db_L,db_a,db_b,db_Hue, db_Saturation, db_Value, db_X, db_Y, db_Z, temp_X, temp_Y, temp_Z) VALUES ('%d', '%d','%d','%d','%d','%d', '%d', '%d','%d', '%d', '%f', '%f', '%f', '%f','%f','%f')"), db_index, r, g, b, _L, _a, _b, H, S, V, X, Y, Z, tempX,tempY, tempZ);
				m_db.execQuery(_insertQuery);
#endif

				db_index++;

#else
				IplImage * _blue = cvCreateImage(cvSize(_height, _width), IPL_DEPTH_8U, 1);
				IplImage * _green = cvCreateImage(cvSize(_height, _width), IPL_DEPTH_8U, 1);
				IplImage * _red = cvCreateImage(cvSize(_height, _width), IPL_DEPTH_8U, 1);

				cvSet(_blue, cvScalar(b), NULL);
				cvSet(_green, cvScalar(g), NULL);
				cvSet(_red, cvScalar(r), NULL);

				IplImage * rgb_image = cvCreateImage(cvSize(_height, _width), IPL_DEPTH_8U, 3);
				cvCvtPlaneToPix(_blue, _green, _red, NULL, rgb_image);

				IplImage * lab_image = cvCreateImage(cvSize(_height, _width), IPL_DEPTH_8U, 3);

				cvCvtColor(rgb_image, lab_image, CV_BGR2Lab);

				Point3_ <uchar> pixelData;

				int			index = 0;

				uchar LValue = lab_image->imageData[index + BLUECOLOR];
				uchar AValue = lab_image->imageData[index + GREENCOLOR];
				uchar BValue = lab_image->imageData[index + REDCOLOR];

				uchar L = RoundOff(LValue / 255.0 * 100.0, 0);

				if(r == 23 && g == 6 && b == 20)
					TRACE("\n LValue=%d, AValue=%d, BValue=%d\n", L, AValue - Lab_A, BValue - Lab_B);

				cvReleaseImage(&_blue);
				cvReleaseImage(&_green);
				cvReleaseImage(&_red);
				cvReleaseImage(&rgb_image);
				cvReleaseImage(&lab_image);
#endif
			}
		}
	}

	fclose(fpX);
	fclose(fpY);
	fclose(fpZ);

#ifndef __SQLITE_NOT_USE__
	m_db.execQuery(_T("commit;"));
#endif

	return;
#endif


#if 1
	if (srcMat.data)
	{
		PhotoImageToAutoWhiteBalance(savedImgName, srcMat);
	}
#else

	if (srcMat.data)
	{
		Mat		_dstMat;
		Mat		_srcMat;
		Mat		_grayMat;

		srcMat.copyTo(_dstMat);
		srcMat.copyTo(_srcMat);

		int N = srcMat.rows * srcMat.cols;


#if 1
		double alpha = 1.0;
		int beta = 0;
		int _blueValue = 0, _greenValue = 0, _redValue = 0;
		int	_rgbValue = 0;

#if 1				// Temperature 와 Tint 적용 예정
		for (int i = 0; i < blueArray.GetSize(); i+=2)
		{
//			TRACE("\n blue[%d]=%d, green[%d]=%d, red[%d]=%d\n", i, blueArray[i], i, greenArray[i], i, redArray[i]);
		}

		IplImage * _resultImage;
		IplImage _srcMatToImage = (IplImage)srcMat;
		IplImage * _srcImage = &_srcMatToImage;

		int		_ws = _srcImage->widthStep;
		int		_channel = _srcImage->nChannels;
		int		nldx;

		_resultImage = cvCreateImage(cvSize(_srcImage->width, _srcImage->height), _srcImage->depth, _srcImage->nChannels);
		cvZero(_resultImage);

		beta = AutoWhiteBalanceRGB();

		if (MaxColorRGB == BLUECOLOR)
		{
			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _redValue = _srcImage->imageData[nldx + REDCOLOR];
					unsigned char _greenValue = _srcImage->imageData[nldx + GREENCOLOR];
					unsigned char _blueValue = _srcImage->imageData[nldx];

					if ((_redValue + beta) > 255 || (_greenValue + beta) > 255)
					{
						_resultImage->imageData[nldx + REDCOLOR] = _srcImage->imageData[nldx + REDCOLOR];
						_resultImage->imageData[nldx + GREENCOLOR] = _srcImage->imageData[nldx + GREENCOLOR];
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];
					}
					else
					{
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];

						for (int c = GREENCOLOR; c <= REDCOLOR ; c++)
						{
							unsigned char _changeRG = _srcImage->imageData[nldx + c] + beta;

							if (_changeRG > 255 || _changeRG < beta)
								_changeRG = _srcImage->imageData[nldx + c];
							else if (_changeRG < _blueValue)
								_changeRG = _blueValue;

							_resultImage->imageData[nldx + c] = _changeRG;
						}
					}
				}
			}
		}
		else if (MaxColorRGB == GREENCOLOR || MaxColorRGB == REDCOLOR)
		{
			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _redColor = _srcImage->imageData[nldx + REDCOLOR];			// red
					unsigned char _greenColor = _srcImage->imageData[nldx + GREENCOLOR];			// green
					unsigned char _blueColor = _srcImage->imageData[nldx];				// blue

					if (i == 917 && j == 2715)
						TRACE("blueColor=%d", _blueColor);

					_resultImage->imageData[nldx + REDCOLOR] = _srcImage->imageData[nldx + REDCOLOR];
					_resultImage->imageData[nldx + GREENCOLOR] = _srcImage->imageData[nldx + GREENCOLOR];

					if (_redColor == _blueColor || _greenColor == _blueColor)
					{
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];
					}
					else
					{
						unsigned char _changeBlueColor = _srcImage->imageData[nldx] + beta;			// green 이 blue 보다 32가 크다

						if (_changeBlueColor > 255 || _changeBlueColor < _blueColor)
							_changeBlueColor = _srcImage->imageData[nldx];

						_resultImage->imageData[nldx] = _changeBlueColor;
					}
				}
			}
		}

		CTime _time = CTime::GetCurrentTime();
		ImageFileSaved(savedImgName, _time.Format("%y%m%d_%H%M%S"), _resultImage, _T("__WhiteBalance"));
		cvReleaseImage(&_resultImage);
#else

		vector <Mat> bgr_planes;
		split(srcMat, bgr_planes);

		_dstMat = Mat::zeros(srcMat.size(), srcMat.type());

		beta = AutoWhiteBalanceRGB();

		if (MaxColorRGB == BLUECOLOR)
		{

			IplImage * _resultImage;
			IplImage _srcMatImage = (IplImage)srcMat;
			IplImage * _srcImage = &_srcMatImage;

			int		_ws = _srcImage->widthStep;
			int		_channel = _srcImage->nChannels;
			int		nldx;

			_resultImage = cvCreateImage(cvSize(_srcImage->width, _srcImage->height), _srcImage->depth, _srcImage->nChannels);

			cvZero(_resultImage);

			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					unsigned char _redValue = _srcImage->imageData[nldx + 2];
					unsigned char _greenValue = _srcImage->imageData[nldx + 1];
					unsigned char _blueValue = (int)_srcImage->imageData[nldx];

					if ((_redValue + beta) > 255 || (_greenValue + beta) > 255)
					{
						_resultImage->imageData[nldx + 2] = _srcImage->imageData[nldx + 2];
						_resultImage->imageData[nldx + 1] = _srcImage->imageData[nldx + 1];
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];
					}
					else
					{
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];

						for (int c = 1; c < 3; c++)
						{
							unsigned char _changeRG = _srcImage->imageData[nldx + c] + beta;

							if (_changeRG > 255)
								_changeRG = 255;

							_resultImage->imageData[nldx + c] = _changeRG;			// red
						}
					}
				}
			}

			CTime _time = CTime::GetCurrentTime();
			ImageFileSaved(savedImgName, _time.Format("%y%m%d_%H%M%S"), _resultImage, _T("__WhiteBalance"));
			cvReleaseImage(&_resultImage);
		}
		else 
		{

			IplImage * _resultImage;
			IplImage _srcMatImage = (IplImage)srcMat;
			IplImage * _srcImage = &_srcMatImage;

			int		_ws = _srcImage->widthStep;
			int		_channel = _srcImage->nChannels;
			int		nldx;

			_resultImage = cvCreateImage(cvSize(_srcImage->width, _srcImage->height), _srcImage->depth, _srcImage->nChannels);

			cvZero(_resultImage);

			for (int i = 0; i < _srcImage->height; i++)
			{
				for (int j = 0; j < _srcImage->width; j++)
				{
					nldx = i * _ws + j * _channel;

					if (_redValue == _blueValue && _redValue == _greenValue)
					{
						_resultImage->imageData[nldx + 2] = _srcImage->imageData[nldx + 2];
						_resultImage->imageData[nldx + 1] = _srcImage->imageData[nldx + 1];
						_resultImage->imageData[nldx] = _srcImage->imageData[nldx];
					}
					else
					{
						unsigned char _redValue = _srcImage->imageData[nldx + 2];
						unsigned char _greenValue = _srcImage->imageData[nldx + 1];
						unsigned char _blueValue = _srcImage->imageData[nldx];

						_resultImage->imageData[nldx + 2] = _srcImage->imageData[nldx + 2];			// red
						_resultImage->imageData[nldx + 1] = _srcImage->imageData[nldx + 1];

						if ((_blueValue + beta) > 255)
							_resultImage->imageData[nldx] = _srcImage->imageData[nldx];
						else
							_resultImage->imageData[nldx] = _srcImage->imageData[nldx] + beta;
					}
				}
			}	

			CTime _time = CTime::GetCurrentTime();
			ImageFileSaved(savedImgName, _time.Format("%y%m%d_%H%M%S"), _resultImage, _T("__WhiteBalance"));
			cvReleaseImage(&_resultImage);
		}
#endif
#else
		IplImage _srcMatImage = (IplImage)srcMat;
		IplImage * _srcImage = &_srcMatImage;

		grayConversion(_srcImage, &ml, &ma, &mb, p);

		float r = (ma + mb + ml) / 3;

		if (m == 1)
		{
			r = (ma + mb + ml) / 3;
			r = max(ma, mb);
			r = max(r, ml);
		}

#if 0
		CString _printValue;

		_printValue.Format(_T("%f"), ml);
		GetDlgItem(IDC_BLUE_EDIT)->SetWindowTextW(_printValue);

		_printValue.Format(_T("%f"), ma);
		GetDlgItem(IDC_GREEN_EDIT)->SetWindowTextW(_printValue);

		_printValue.Format(_T("%f"), mb);
		GetDlgItem(IDC_RED_EDIT)->SetWindowTextW(_printValue);

		_printValue.Format(_T("%f"), r);
		GetDlgItem(IDC_EDIT4)->SetWindowTextW(_printValue);

		_printValue.Format(_T("%d"), p);
		GetDlgItem(IDC_P_VALUE_EDIT)->SetWindowTextW(_printValue);

		_printValue.Format(_T("%d"), m);
		GetDlgItem(IDC_M_VALUE_EDIT)->SetWindowTextW(_printValue);

		Mat_<Vec3b>::const_iterator it = _srcMat.begin<Vec3b>();
		Mat_<Vec3b>::const_iterator itend = _srcMat.end<Vec3b>();
		Mat_<Vec3b>::iterator itout = _dstMat.begin<Vec3b>();
#endif

#if 1
		IplImage * _resultImage;

		_resultImage = cvCreateImage(cvSize(_srcImage->width, _srcImage->height), _srcImage->depth, _srcImage->nChannels);

		cvZero(_resultImage);

		for(int i = 0; i < _srcImage->height; i++)
		{
			for(int j = 0; j < _srcImage->width; j++)
			{
				int nldx = i * _srcImage->widthStep + j * _srcImage->nChannels;

				float l = _srcImage->imageData[nldx];
				float a = _srcImage->imageData[nldx + 1];
				float b = _srcImage->imageData[nldx + 2];

				a = a * (r / ma);
				b = b * (r / mb);
				l = l * (r / ml);

				if(a > 255)
					a = 255;
				if(b > 255)
					b = 255;
				if(l > 255)
					l = 255;

				_resultImage->imageData[nldx] = l;
				_resultImage->imageData[nldx + 1] = a;
				_resultImage->imageData[nldx + 2] = b;
			}
		}

		CTime _time = CTime::GetCurrentTime();
		ImageFileSaved(savedImgName, _time.Format("%y%m%d_%H%M%S"), _resultImage, _T("__WhiteBalance"));
		cvReleaseImage(&_resultImage);

#else
		for (; it != itend; ++it, ++itout)
		{
			Vec3b v1 = *it;

			float l = v1.val[0];
			float a = v1.val[1];
			float b = v1.val[2];

			a = a * (r / ma);
			b = b * (r / mb);
			l = l * (r / ml);

			if (a > 255)
				a = 255;
			if (b > 255)
				b = 255;
			if (l > 255)
				l = 255;

			v1.val[0] = l;
			v1.val[1] = a;
			v1.val[2] = b;
			*itout = v1;
		}
		_dstMat.copyTo(srcMat);
#endif

#endif


#if 0			// __whitebalance 폴더에 저장하는 부분 임시로 막음 (20140611_15:47)
		CString dir = _T("__WhiteBalance");
		CFileFind _file;
		CString _strFile = _T("*.*");
		BOOL bResult = _file.FindFile(dir + _strFile);

		if (!bResult)
		{
			bResult = CreateDirectory(dir, NULL);
		}

		if (bResult)
		{

			CString _whiteFileName;
			
			_whiteFileName.Format(_T("__WhiteBalance/w_p_%sm_%s_%s"), strP, strM, savedImgName);
			int strlen = _whiteFileName.GetLength();
			char * _savedFileName = new char[strlen + 1];

			WideCharToMultiByte(CP_UTF8, 0, _whiteFileName, -1, _savedFileName, strlen + 1, NULL, NULL);

			imwrite(_savedFileName, _dstMat);

#if 0				// Exposure 효과에 대해서 임시로 막음 (20140530_17:03)
			IplImage _dstImageMat = _dstMat;
			IplImage * _dstImage = &_dstImageMat;

			CvScalar avg = cvAvg(_dstImage);

			double meanVarR = avg.val[2];
			double meanVarG = avg.val[1];
			double meanVarB = avg.val[0];

			double alpha = 1.0;
			int beta = 0;

			if (meanVarR < 145.0)
			{
				beta = -5;
			}
			else if (meanVarR < 175.0)
			{
				beta = -10;
			}
			Mat new_imageExposure = Mat::zeros(_dstMat.size(), _dstMat.type());
			_dstMat.convertTo(new_imageExposure, -1, 1, beta);
			imwrite("new_imageExposure.jpg", new_imageExposure);
#endif
#if 0
			CString _histogramFileName = _T("White_Histogram_" + savedImgName);

			CPhotoToHistogramConvert * pHistogramConvert = new CPhotoToHistogramConvert;
			pHistogramConvert->ImageToHistogram(_histogramFileName, _dstMat, false);
			delete pHistogramConvert;
#endif
		}
#endif				// __whitebalance 폴더에 저장하는 부분 임시로 막음 (20140611_15:47)
	}
#endif

}


void CFMPhotoEditorDlg::OnBnClickedBtnExposure()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (srcMat.data)
	{
#if 0
		Mat imgH = srcMat + Scalar(10, 10, 10);		// increase a liughtness by 75 units ==> img.convertTo(imgH, -1, 1, 75);
		Mat imgL = srcMat + Scalar(-10, -10, -10);	// decrease a liughtness by 75 units ==> img.convertTo(imgL, -1, 1, -75);

		imwrite("brightness_High.jpg", imgH);
		imwrite("brightness_Low.jpg", imgL);

#else
		double alpha = 1.0;
		int beta = 5;

		Mat	new_imageHigh = Mat::zeros(srcMat.size(), srcMat.type());
		Mat new_imageLow = Mat::zeros(srcMat.size(), srcMat.type());
#if 1
		srcMat.convertTo(new_imageHigh, -1, 1, 5);
		srcMat.convertTo(new_imageLow, -1, 1, -5);
		imwrite("BrightenessHigh.jpg", new_imageHigh);
		imwrite("BrightenessLow.jpg", new_imageLow);
#else
		for (int y = 0; y < srcMat.rows; y++)
		{
			for (int x = 0; x < srcMat.cols; x++)
			{
				for (int c = 0; c < 3; c++)
				{
					new_imageHigh.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha *(srcMat.at<Vec3b>(y, x)[c]) + beta);
					new_imageHigh.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha *(srcMat.at<Vec3b>(y, x)[c]) - beta);
				}
			}
		}
		imwrite("BrightenessHigh.jpg", new_imageHigh);
		imwrite("BrighteenssLow.jpg", new_imageLow);
#endif
#endif
	}
}


void CFMPhotoEditorDlg::OnBnClickedButton1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (srcMat.data)
	{
		double sumR = 0.0;
		double sumG = 0.0;
		double sumB = 0.0;

		int _height, _width, _ws;

		IplImage _srcMatToImage = srcMat;
		IplImage * _srcImage = &_srcMatToImage;
		IplImage * _newImage = NULL;

		_newImage = cvCreateImage(cvSize(_srcImage->width, _srcImage->height), _srcImage->depth, _srcImage->nChannels);

		_height = _srcImage->height;
		_width = _srcImage->width;
		_ws = _srcImage->widthStep;

		CvScalar avg = cvAvg(_srcImage);

		double meanVarR = avg.val[2];
		double meanVarG = avg.val[1];
		double meanVarB = avg.val[0];

		TRACE("\n\n avgImage meanVar_Red=%f, meanVar_Green =%f, meanVar_Blue=%f, _ws=%d, _height=%d, _width=%d\n\n", meanVarR, meanVarG, meanVarB, _ws, _height, _width);

#ifndef __SQLITE_NOT_USE__
		CString _updateQuery;
		_updateQuery.Format(_T("UPDATE PHOTO SET p_avgRed = %f, p_avgGreen = %f, p_avgBlue = %f WHERE p_FileID = '%s'"), meanVarR, meanVarG, meanVarB, str_FileID);
		CppSQLite3Query _sqlResult = m_db.execQuery(_updateQuery);
#endif

#if 0
		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				sumR += pow(((BYTE)_srcImage->imageData[i * _ws + j * 3 + 2] - meanVarR), 2.0);
				sumG += pow(((BYTE)_srcImage->imageData[i * _ws + j * 3 + 1] - meanVarG), 2.0);
				sumB += pow(((BYTE)_srcImage->imageData[i * _ws + j * 3] - meanVarB), 2.0);
			}
		}
		
		double sqrtVarR = sqrt(sumR / (_height * _width));
		double sqrtVarG = sqrt(sumG / (_height * _width));
		double sqrtVarB = sqrt(sumB / (_height * _width));

		double addVarR = 255.0 - meanVarR - 2.0 * sqrtVarR;
		double addVarG = 255.0 - meanVarG - 2.0 * sqrtVarG;
		double addVarB = 255.0 - meanVarB - 2.0 * sqrtVarB;

		for (int i = 0; i < _height; i++)
		{
			for (int j = 0; j < _width; j++)
			{
				_newImage->imageData[i * _ws + j * 3 + 2] = (char)((BYTE)_srcImage->imageData[i * _ws + j * 3 + 2] + addVarR);
				_newImage->imageData[i * _ws + j * 3 + 1] = (char)((BYTE)_srcImage->imageData[i * _ws + j * 3 + 1] + addVarG);
				_newImage->imageData[i * _ws + j * 3] = (char)((BYTE)_srcImage->imageData[i * _ws + j * 3] + addVarB);
			}
		}

		cvSaveImage("_newImage.jpg", _newImage);
#endif

#if 0
		CString _selectQuery;
		_selectQuery.Format(_T("SELECT p_originalFilePath FROM PHOTO where p_FileID = '%s'"), str_FileID);

		CppSQLite3Query _sqlResult = m_db.execQuery(_selectQuery);

		while (!_sqlResult.eof())
		{
			AfxMessageBox(_sqlResult.fieldValue(0));
			_sqlResult.nextRow();
		}
#endif
	}
}


void CFMPhotoEditorDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (srcMat.data)
	{
		CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Image (*.BMP, *.GIF, *.JPG, *.PNG) | *.BMP;*.GIF;*.JPG;*.PNG | All Files(*.*)|*.*||"), this);

		if (dlgFile.DoModal() == IDOK)
		{
			CString _compareFilePath = dlgFile.GetPathName();
			CT2CA strAtl(dlgFile.GetPathName());
			string strImagePath(strAtl);

			CString _comparefileName = dlgFile.GetFileName();
#if 0
			CString _str = dlgFile.GetPathName();
			CStringA _strA(_str);
			LPCSTR _szTemp = _strA;

			SetWindowTextA(txtFileName, _szTemp);
#endif
			compareMat = imread(strImagePath);

			IplImage _srcImageMat = srcMat;
			IplImage * _srcImage = &_srcImageMat;

			IplImage _compareImage = compareMat;
			compareImage = &_compareImage;

			IplImage * _resultImage;

			CvScalar avg = cvAvg(compareImage);

			double meanVarR = avg.val[2];
			double meanVarG = avg.val[1];
			double meanVarB = avg.val[0];

#ifndef __SQLITE_NOT_USE__
			CString _updateQuery;
			_updateQuery.Format(_T("UPDATE PHOTO SET p_revisionFilePath = %s, p_revision_avgRed = %f, p_revision_avgGreen = %f, p_revision_avgBlue = %f WHERE p_FileID = '%s'"), _compareFilePath, meanVarR, meanVarG, meanVarB, str_FileID);
			CppSQLite3Query _sqlResult = m_db.execQuery(_updateQuery);
#endif

			int		_height, _width, _ws, _channels;

			_height = _srcImage->height;
			_width = _srcImage->width;
			_ws = _srcImage->widthStep;
			_channels = _srcImage->nChannels;

			TRACE("\n\n _height=%d, _width=%d, _ws=%d\n\n\n", _height, _width, _ws);

			_resultImage = cvCreateImage(cvSize(_width, _height), _srcImage->depth, _srcImage->nChannels);

			for (int i = 0; i < _height; i++)
			{
				for (int j = 0; j < _width; j++)
				{
#if 1
					int _originalBlue = 0;
					int _originalGreen = 0;
					int _originalRed = 0;

					int _compareBlue = 0;
					int _compareGreen = 0;
					int _compareRed = 0;

					_originalBlue = _srcImage->imageData[i * _ws + j * 3 + 2];
					_originalGreen = _srcImage->imageData[i * _ws + j * 3 + 1];
					_originalRed = _srcImage->imageData[i * _ws + j * 3];

					_compareBlue = compareImage->imageData[i * compareImage->widthStep + j * 3 + 2];
					_compareGreen = compareImage->imageData[i * compareImage->widthStep + j * 3 + 1];
					_compareRed = compareImage->imageData[i * compareImage->widthStep + j * 3];

#if 1
					_resultImage->imageData[i * _ws + j * 3 + 2] = _originalBlue - _compareBlue;
					_resultImage->imageData[i * _ws + j * 3 + 1] = _originalGreen - _compareGreen;
					_resultImage->imageData[i * _ws + j * 3] = _originalRed - _compareRed;

#else
					_originalBlue += 5;
					_originalGreen += 5;
					_originalRed += 5;
#endif
#else
					int _compareBlue = (_srcImage->imageData[i * _ws + j * 3 + 2] - compareImage->imageData[i * _ws + j * 3 + 2]);
					int _compareGreen = (_srcImage->imageData[i * _ws + j * 3 + 1] - compareImage->imageData[i * _ws + j * 3 + 1]);
					int _compareRed = (_srcImage->imageData[i * _ws + j * 3] - compareImage->imageData[i * _ws + j * 3]);
#endif
					_resultImage->imageData[i * _ws + j * 3 + 2] = _originalBlue;
					_resultImage->imageData[i * _ws + j * 3 + 1] = _originalGreen;
					_resultImage->imageData[i * _ws + j * 3] = _originalRed;
				}
			}
			cvSaveImage("compareImage_2.jpg", _resultImage);

			TRACE("\n\n\n\n COMPARE IMAGE END \n\n\n");
		}
	}
}


void CFMPhotoEditorDlg::OnBnClickedRgbtohsv()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


IplImage * CFMPhotoEditorDlg::generate_greyscale_image(IplImage * src)
{
	unsigned char * _grey = 0;
	IplImage * _img_grey = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	cvCvtColor(src, _img_grey, CV_RGB2GRAY);

	return _img_grey;
}

void CFMPhotoEditorDlg::OnBnClickedFacefind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (srcMat.data)
	{
		int rows = srcMat.rows;
		int cols = srcMat.cols;

		int flags = CV_HAAR_DO_CANNY_PRUNING | CV_HAAR_FIND_BIGGEST_OBJECT;

		CvHaarClassifierCascade * mFaceClassifier = 0;
		mFaceClassifier = (CvHaarClassifierCascade *)cvLoad((OPENCV_ROOT"/data/haarcascades/haarcascade_frontalface_alt.xml"), 0, 0, 0);
		CvMemStorage * storage = cvCreateMemStorage(0);

		if (mFaceClassifier)
		{
			IplImage _srcImageMat = srcMat;
			IplImage * _srcImage = &_srcImageMat;
			IplImage * img_grey = 0;

			CvScalar _scala = cvAvg(_srcImage);

			img_grey = generate_greyscale_image(_srcImage);
			CvSeq * faces = cvHaarDetectObjects(img_grey, mFaceClassifier, storage, 1.1f, 2, flags, cvSize(20, 20));

			if (faces->total != 0)
			{
				for (int i = 0; i < faces->total; i++)
				{
					CvRect * face_rect = (CvRect *)cvGetSeqElem(faces, i);

					Vec3b intensity = srcMat.at<Vec3b>(face_rect->y, face_rect->x);
					uchar blue = intensity.val[0];
					uchar green = intensity.val[1];
					uchar red = intensity.val[2];

					CvPoint pt1 = { face_rect->x, face_rect->y };
					CvPoint pt2 = { face_rect->x + face_rect->width, face_rect->y + face_rect->height };
					cvRectangle(_srcImage, pt1, pt2, CV_RGB(0, 255, 0), 3, 4, 0);
				}

				cvSaveImage("findface.jpg", _srcImage);
			}
		}
	}
}

Mat CFMPhotoEditorDlg::EqualizeIntensity(const Mat & inputImage)
{
	if (inputImage.channels() >= 3)
	{
		Mat _ycrcb;
		cvtColor(inputImage, _ycrcb, CV_BGR2YCrCb);

		imwrite("CVBGR2YCrCb.jpg", _ycrcb);
		
		return _ycrcb;
	}
}

void CFMPhotoEditorDlg::OnBnClickedHistequalization()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (srcMat.data)
	{
		Mat _ycrcb = EqualizeIntensity(srcMat);
		Mat resultMat;

		vector<Mat> channels;
		split(_ycrcb, channels);

		equalizeHist(channels[1], channels[1]);
		merge(channels, _ycrcb);

		cvtColor(_ycrcb, resultMat, CV_YCrCb2BGR);

		imwrite("resultYCRCB.jpg", resultMat);
	}
}


void CFMPhotoEditorDlg::DirForImageFileRead(CString pathDir)
{
	if (pathDir.GetLength() <= 0)
		return;

	CFileFind _file;
	CString _strFile = _T("\\*.*");
	BOOL bResult = _file.FindFile(pathDir + _strFile);

	CPhotoToHistogramConvert * pHistogramConvert = new CPhotoToHistogramConvert;

	while (bResult)
	{
		bResult = _file.FindNextFile();
		CT2CA strAtl(_file.GetFilePath());
		string strImagePath(strAtl);
		Mat _srcMat = imread(strImagePath);

		if (_srcMat.data)
		{
			pHistogramConvert->ImageToHistogram(_file.GetFileName(), _srcMat, false);
			PhotoImageToAutoWhiteBalance(_file.GetFileName(), _srcMat);
		}
	}

	delete pHistogramConvert;
}

void CFMPhotoEditorDlg::OnBnClickedFolderSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BROWSEINFO		BrInfo;
	TCHAR			szBuffer[DIRPATH_FILENAME_LEN];

	::ZeroMemory(&BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(szBuffer, DIRPATH_FILENAME_LEN);

	BrInfo.hwndOwner = GetSafeHwnd();

	BrInfo.lpszTitle = _T("Folder Select");
	BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNONLYFSDIRS;

	LPITEMIDLIST pItemIdList = ::SHBrowseForFolder(&BrInfo);

	::SHGetPathFromIDList(pItemIdList, szBuffer);

	DirForImageFileRead((LPCTSTR)szBuffer);

}


void CFMPhotoEditorDlg::OnBnClickedRgbdataConf()
{
	int		index = 0;


#if 1
	FILE * fpX, *fpY, *fpZ;
	char	line[200];

	fopen_s(&fpX, "Najak_X.txt", "r");
	fopen_s(&fpY, "Najak_Y.txt", "r");
	fopen_s(&fpZ, "Najak_Z.txt", "r");

	if(fpX != NULL)
	{
		while(!feof(fpX))
		{
			fgets(line, sizeof(line), fpX);
			double	_x = strtof(line, 0);
			temp_XArray.Add(_x);
		}
	}

	fclose(fpX);

	if (fpY != NULL)
	{
		while (!feof(fpY))
		{
			fgets(line, sizeof(line), fpY);
			double	_y = strtof(line, 0);
			temp_YArray.Add(_y);
		}
	}

	fclose(fpY);

	if(fpZ != NULL)
	{
		while(!feof(fpZ))
		{
			fgets(line, sizeof(line), fpZ);
			double	_z = strtof(line, 0);
			temp_ZArray.Add(_z);
		}
	}

	fclose(fpZ);
#else
	CString _selectQuery;
	_selectQuery.Format(_T("SELECT temp_X, temp_Y, temp_Z FROM PhotoRGB"));

	m_db.execQuery(_T("Begin;"));

	CppSQLite3Query _sqlResult = m_db.execQuery(_selectQuery);

	while (!_sqlResult.eof())
	{
		temp_XArray.Add(_sqlResult.getIntField(0));
		temp_YArray.Add(_sqlResult.getIntField(1));
		temp_ZArray.Add(_sqlResult.getIntField(2));

		index++;

		_sqlResult.nextRow();
	}

	m_db.execQuery(_T("Commit;"));
#endif
}



int CFMPhotoEditorDlg::ImageAdjust(CString fileName, IplImage* src, IplImage* dst,
	double low, double high,  
	double bottom, double top, 
	double gamma)
{
	if (low<0 && low>1 && high <0 && high>1 &&
		bottom<0 && bottom>1 && top<0 && top>1 && low>high)
		return -1;
	double low2 = low * 255;
	double high2 = high * 255;
	double bottom2 = bottom * 255;
	double top2 = top * 255;
	double err_in = high2 - low2;
	double err_out = top2 - bottom2;

	int x, y;
	double val;

	// intensity transform  
	for (y = 0; y < src->height; y++)
	{
		for (x = 0; x < src->width; x++)
		{
			val = ((uchar*)(src->imageData + src->widthStep*y))[x];
			val = pow((val - low2) / err_in, gamma) * err_out + bottom2;
			if (val>255) val = 255; if (val<0) val = 0; // Make sure src is in the range [low,high]  
			((uchar*)(dst->imageData + dst->widthStep*y))[x] = (uchar)val;
		}
	}

	CTime _time = CTime::GetCurrentTime();
	ImageFileSaved(fileName, _time.Format("%y%m%d_%H%M%S"), dst, _T("__Exposure"));

	return 0;
}