#pragma once


// CPhotoExposureSlider

class CPhotoExposureSlider : public CSliderCtrl
{
	DECLARE_DYNAMIC(CPhotoExposureSlider)

public:
	CPhotoExposureSlider();
	virtual ~CPhotoExposureSlider();

protected:
	DECLARE_MESSAGE_MAP()

public:
	bool m_bSelected;
	CSliderCtrl * m_exposureSliderCtrl;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	void setExposureSlider(CSliderCtrl* m_pSliderCtrl);
};


