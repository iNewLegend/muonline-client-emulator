//#pragma once
//#include "DlgBaseEditor.h"
//#include "TextRender.h"
//
//#if defined(_DEBUG)
//#pragma comment(lib, "cxcored.lib")
//#pragma comment(lib, "cvd.lib")
//#pragma comment(lib, "mld.lib")
//#pragma comment(lib, "highguid.lib")
//#else
//#pragma comment(lib, "cxcore.lib")
//#pragma comment(lib, "cv.lib")
//#pragma comment(lib, "ml.lib")
//#pragma comment(lib, "highgui.lib")
//#endif
//
//class CDlgFaceDetect: public CDlgBaseEditor
//{
//public:
//	CDlgFaceDetect();
//	~CDlgFaceDetect();
//	virtual void OnControlRegister();
//	virtual bool OnInitDialog();
//	virtual void OnFrameMove(double fTime, float fElapsedTime);
//	virtual void OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime);
//	void SetAR(bool bAR){m_bAR = bAR;}
//	bool GetAR(){return m_bAR;}
//protected:
//	CTexture*	m_pTexCapture;
//	Vec3D		m_vEye;
//	CUBB		m_ubb;
//	bool		m_bAR;
//	RECT		m_rcFace;
//private:
//};