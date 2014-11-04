
// FMPhotoEditorDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include <opencv\cv.h>
#include <opencv\highgui.h>

#include <opencv2\highgui\\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include "CppSQLite3U.h"
#include "afxcmn.h"
#include "PhotoExposureSlider.h"

#define BLUECOLOR					0
#define GREENCOLOR					1
#define REDCOLOR					2

#define __SQLITE_NOT_USE__

using namespace cv;

// CFMPhotoEditorDlg 대화 상자
class CFMPhotoEditorDlg : public CDialogEx
{
// 생성입니다.
public:
	CFMPhotoEditorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	// 생성한 User Method
	bool ImageFileSaved(CString readImgFile, CString currentTimeID, IplImage * readImage, CString readFilePath);				// Read된 Image 파일을 파일명과 확장자를 분리하여 Database에 저장

// 대화 상자 데이터입니다.
	enum { IDD = IDD_FMPHOTOEDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

// 구현입니다.
protected:
	HICON m_hIcon;


	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	IplImage	* srcImage;
	IplImage	* dsrImage;
	IplImage	* compareImage;
	Mat			srcMat;
	Mat			dsrMat;
	Mat			compareMat;

	Mat			resultMat;
	CString		savedImgName;
	CString		str_FileID;

	CppSQLite3DB m_db;

	// 생성한 User Method 
	IplImage * generate_greyscale_image(IplImage * src);
	afx_msg void OnPhotoView(IplImage * viewIplImage);
	afx_msg Mat EqualizeIntensity(const Mat& inputImage);
	afx_msg float AutoWhiteBalanceRGB();
	afx_msg void convertTo(OutputArray _dst, int _type, double alpha, double beta);
	afx_msg void ImageResizeToSaveImage(CString fileName, IplImage * srcImage);

	afx_msg double PivotRgb(double n);
	afx_msg double CubicRoot(double n);
	afx_msg	double PivotXyz(double n);
	afx_msg int ToRGB(double n);

	afx_msg int CFMPhotoEditorDlg::ImageAdjust(CString fileName, IplImage* src, IplImage* dst,
		double low, double high,
		double bottom, double top,
		double gamma);

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedFolderSelect();
	CStatic picSrcPicture;
	CStatic picSrcHistorgamPicture;
	CStatic txtFileName;
	CString imgFileName;
	CButton btnHistogram;
	afx_msg void OnBnClickedBtnHistogram();
	CEdit c_BlueEditor;
	CEdit c_GeeenEditor;
	CEdit c_RedEditor;
	CEdit c_AvageEditor;
	CString m_strBlueEditor;
	CString m_strGreenEditor;
	CString m_strRedEditor;
	CString m_strAvageEditor;
	afx_msg void OnBnClickedBtnExposure();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedRgbtohsv();
	afx_msg void OnBnClickedFacefind();
	CString m_exposureValue;
	CPhotoExposureSlider m_exposureSliderCtrl;
	afx_msg void OnBnClickedHistequalization();
	afx_msg void OnBnClickedGrayword();
	afx_msg void PhotoImageToAutoWhiteBalance(CString fileName, Mat readMat);
	afx_msg void DirForImageFileRead(CString pathDir);

	afx_msg int RGBCalculateDifference(int blueValue, int greenValue, int redValue, int second_blueValue, int second_greenValue, int second_redValue);
	afx_msg void PhotoImageExposureEffect(CString fileName,IplImage * _srcImage, int RGBColor, float alphas);
	afx_msg void MatFileSaved(CString readImgFile, CString currentTimeID, Mat readMat, CString readFilePath);

	afx_msg void PhotoNormalize(InputArray _src, OutputArray _dst, double a, double b, int norm_type, int rtype, InputArray _mask);
	afx_msg IplImage * PhotoBlackPoint(IplImage * _srcImage, InputArray _blueSrc, InputArray _greenSrc, InputArray _redSrc, double _blueMin, double _greenMin, double _redMin);


	int		blueValue;
	int		redValue;
	int		greenValue;

	int		blueMax_X;
	int		redMax_X;
	int		greenMax_X;

	int		blueMax_Second_X;
	int		redMax_Second_X;
	int		greenMax_Second_X;

	int		MaxColorRGB;

	CUIntArray blueArray;
	CUIntArray greenArray;
	CUIntArray redArray;

	int		shadowMoveValue;
	int		whiteMoveValue;

	CUIntArray	temp_XArray;
	CUIntArray	temp_YArray;
	CUIntArray	temp_ZArray;

	CUIntArray	X_Position_High;
	CUIntArray	X_Position_Normal;
	CUIntArray	X_Position;

	afx_msg void OnBnClickedRgbdataConf();
};
