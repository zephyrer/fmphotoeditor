// PhotoExposureSlider.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "FMPhotoEditor.h"
#include "PhotoExposureSlider.h"
#include "FMPhotoEditorDlg.h"

// CPhotoExposureSlider

IMPLEMENT_DYNAMIC(CPhotoExposureSlider, CSliderCtrl)

CPhotoExposureSlider::CPhotoExposureSlider()
{

}

CPhotoExposureSlider::~CPhotoExposureSlider()
{
}


BEGIN_MESSAGE_MAP(CPhotoExposureSlider, CSliderCtrl)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CPhotoExposureSlider::OnLButtonDown(UINT nFlags, CPoint point)
{
	CRect rect;
	GetThumbRect(&rect);

	if (rect.PtInRect(point))
		m_bSelected = true;
	else
		m_bSelected = false;

	CSliderCtrl::OnMButtonDown(nFlags, point);
}

void CPhotoExposureSlider::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bSelected = false;

	int nPos = GetPos();
	m_exposureSliderCtrl->SetPos(nPos);

	CSliderCtrl::OnLButtonUp(nFlags, point);
}

void CPhotoExposureSlider::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bSelected && m_exposureSliderCtrl)
	{
		int nPos = GetPos();
		m_exposureSliderCtrl->SetPos(nPos);

		CString str;
		str.Format(_T("%d"), nPos);
		CFMPhotoEditorDlg * pMain = (CFMPhotoEditorDlg *)AfxGetMainWnd();
		pMain->m_exposureValue.SetString(str);
		pMain->UpdateData(false);
	}

	CSliderCtrl::OnMouseMove(nFlags, point);
}



// CPhotoExposureSlider 메시지 처리기입니다.




void CPhotoExposureSlider::setExposureSlider(CSliderCtrl* m_pSliderCtrl)
{
	m_exposureSliderCtrl = m_pSliderCtrl;
}
