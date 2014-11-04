#include "stdafx.h"
#include "FMPhotoEditor.h"
#include "FMPhotoEditorDlg.h"
#include "afxdialogex.h"
#include "PhotoToHistogramConvert.h"

#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "CvvImage.h"

#include <opencv2\highgui\\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

#define HISTOGRAM_WIDTH		512
#define HISTOGRAM_HEIGHT	400

#define COLOR_BLUE			0
#define COLOR_GREEN			1
#define COLOR_RED			2

CPhotoToHistogramConvert::CPhotoToHistogramConvert()
{
	CWnd * pWnd = AfxGetMainWnd();
	m_hWnd = pWnd->m_hWnd;
	
	blue_XValue = 0;
	red_XValue = 0;
	green_XValue = 0;

	blueMaxValue = 0;
	redMaxValue = 0;
	greenMaxValue = 0;

	pClass = (CFMPhotoEditorDlg *)CWnd::FromHandle(m_hWnd);

}

CPhotoToHistogramConvert::~CPhotoToHistogramConvert()
{

}

void CPhotoToHistogramConvert::HistogramForAnalysis(int x, int y, int color, CFMPhotoEditorDlg * pClass)			// 0=Blue, 1=Green, 2=Red
{
	int		_realX = 0;
	int		_insertIndex = 0;
	int		_currentValue = 0;

	if (x != 0)
	{
		_realX = x / 2;
		_insertIndex = _realX / X_POSITION_RANGE;

#if 0
		int		_y = HISTOGRAM_HEIGHT - y;
		if (_y > 200)
		{
			_currentValue = pClass->X_Position_High[_insertIndex];
			pClass->X_Position_High[_insertIndex] = _currentValue + 1;
		}
		else if (_y > 100)
		{
			_currentValue = pClass->X_Position_Normal[_insertIndex];
			pClass->X_Position_Normal[_insertIndex] = _currentValue + 1;
		}

		if (_y > 50)
		{
			_currentValue = pClass->X_Position[_insertIndex];
			pClass->X_Position[_insertIndex] = _currentValue + 1;
		}
#else

		if (color == COLOR_RED)
		{
			if (y > 200)
			{
				_currentValue = pClass->X_Position_High[_insertIndex];
				pClass->X_Position_High[_insertIndex] = _currentValue + 1;
			}
			if (y > X_REFERENCE_POINT)
			{
				_currentValue = pClass->X_Position_Normal[_insertIndex];
				pClass->X_Position_Normal[_insertIndex] = _currentValue + 1;
			}
		}
		//if (y > X_REFERENCE_POINT)
		{
			_currentValue = pClass->X_Position[_insertIndex];
			pClass->X_Position[_insertIndex] = _currentValue + 1;
		}
#endif
	}


	switch (color)
	{
		case COLOR_BLUE:
			pClass->blueArray[x] = y;

			if (blueMaxValue <= y)
			{
				blueMaxValue = y;
				pClass->blueMax_Second_X = blue_XValue;
				blue_XValue = _realX;
			}
			break;
		case COLOR_GREEN:
			pClass->greenArray[x] = y;

			if (greenMaxValue <= y)
			{
				greenMaxValue = y;
				pClass->greenMax_Second_X = green_XValue;
				green_XValue = _realX;
			}
			break;
		case COLOR_RED:
			pClass->redArray[x] = y;

			if (redMaxValue <= y)
			{
				redMaxValue = y;
				pClass->redMax_Second_X = red_XValue;
				red_XValue = _realX;
			}
			break;
	}
}

void CPhotoToHistogramConvert::RGBInitialization(CFMPhotoEditorDlg * pClass)
{
	blue_XValue = 0;
	red_XValue = 0;
	green_XValue = 0;

	blueMaxValue = 0;
	redMaxValue = 0;
	greenMaxValue = 0;

	pClass->blueArray.SetSize(513);
	pClass->greenArray.SetSize(513);
	pClass->redArray.SetSize(513);
	
	pClass->X_Position_High.SetSize(X_ARRAY_VALUE);
	pClass->X_Position_Normal.SetSize(X_ARRAY_VALUE);
	pClass->X_Position.SetSize(X_ARRAY_VALUE);

	pClass->blueMax_X = 0;
	pClass->greenMax_X = 0;
	pClass->redMax_X = 0;

	pClass->blueMax_Second_X = 0;
	pClass->greenMax_Second_X = 0;
	pClass->redMax_Second_X = 0;

	pClass->blueValue = 0;
	pClass->greenValue = 0;
	pClass->redValue = 0;

	pClass->shadowMoveValue = 0;
	pClass->whiteMoveValue = 0;
}

void CPhotoToHistogramConvert::ImageToHistogram(CString fileName, Mat srcMat, bool b_show)
{
	for (int i = 0; i < pClass->X_Position_High.GetSize(); i++)
	{
		pClass->X_Position_High[i] = 0;
		pClass->X_Position_Normal[i] = 0;
	}
	for (int i = 0; i < pClass->blueArray.GetSize(); i++)
	{
		pClass->blueArray[i] = 0;
		pClass->greenArray[i] = 0;
		pClass->redArray[i] = 0;
	}

	vector<Mat> bgr_planes;
	split(srcMat, bgr_planes);

	int histSize = 256;

	float range[] = { 0, 256 };
	const float * histRange = { range };

	bool uniform = true;
	bool accumulate = false;

	MatND b_hist, g_hist, r_hist;

	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);		// blue
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);		// green
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);		// red

	int hist_w = HISTOGRAM_WIDTH;
	int hist_h = HISTOGRAM_HEIGHT;

	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());			// blue
	normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());			// green
	normalize(r_hist, r_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());			// red

	RGBInitialization(pClass);

	for (int i = 1; i < histSize; i++)
	{
		if (b_show)
		{
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),				/* Blue */
				Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
				Scalar(255, 0, 0), 2, 8, 0);

			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),				/* Green */
				Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),				/* Red */
				Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
				Scalar(0, 0, 255), 2, 8, 0);
		}
		HistogramForAnalysis(bin_w*(i), cvRound(b_hist.at<float>(i)), 0, pClass);
		HistogramForAnalysis(bin_w*(i), cvRound(g_hist.at<float>(i)), 1, pClass);
		HistogramForAnalysis(bin_w*(i), cvRound(r_hist.at<float>(i)), 2, pClass);
	}



	if (b_show)
	{
		IplImage dstImg = (IplImage)histImage;
		IplImage *_histImage = &dstImg;

		CRect rect = CRect(0, 0, hist_w, hist_h);

		CDC * pDC = pClass->picSrcHistorgamPicture.GetDC();
		pClass->picSrcHistorgamPicture.ValidateRect(&rect);

		CvvImage cvv_Img;
		cvv_Img.CopyOf(_histImage);
		cvv_Img.DrawToHDC(pDC->GetSafeHdc(), &rect);

		CTime _time = CTime::GetCurrentTime();
		CFMPhotoEditorDlg * pFMPhotoEditorDlg = new CFMPhotoEditorDlg;
		pFMPhotoEditorDlg->ImageFileSaved(fileName, _time.Format("%y%m%d_%H%M%S"), _histImage, _T("__Histogram"));
		delete pFMPhotoEditorDlg;
	}
	pClass->blueValue = blueMaxValue;
	pClass->redValue = redMaxValue;
	pClass->greenValue = greenMaxValue;

	pClass->blueMax_X = blue_XValue;
	pClass->redMax_X = red_XValue;
	pClass->greenMax_X = green_XValue;

}