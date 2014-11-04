
#pragma once

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "CvvImage.h"

#include <opencv2\highgui\\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;

class CFMPhotoEditorDlg;

class CPhotoToHistogramConvert : public CObject
{
public:
	int		blueMaxValue;
	int		greenMaxValue;
	int		redMaxValue;

	int		blue_XValue;
	int		green_XValue;
	int		red_XValue;

	CPhotoToHistogramConvert();
	virtual ~CPhotoToHistogramConvert();

	CFMPhotoEditorDlg *pClass;

	void ImageToHistogram(CString fileName, Mat srcMat, bool b_show);
	afx_msg void HistogramForAnalysis(int x, int y, int color, CFMPhotoEditorDlg * pClass);
	afx_msg void RGBInitialization(CFMPhotoEditorDlg * pClass);

	HWND		m_hWnd;
};