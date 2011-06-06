//#include "DlgFaceDetect.h"
//#include "cv.h"
//#include "highgui.h"
//
//static CvMemStorage* storage = 0;
//static CvHaarClassifierCascade* cascade = 0;
//CvCapture* capture = 0;
//IplImage *frame, *frame_copy,* gray = 0;
//
//CDlgFaceDetect::CDlgFaceDetect()
//{
//	m_pTexCapture = NULL;
//	m_bAR = false;
//	cascade = (CvHaarClassifierCascade*)cvLoad( "haarcascade_frontalface_alt2.xml", 0, 0, 0 );
//	if( !cascade )
//	{
//		fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
//		fprintf( stderr,
//			"Usage: facedetect --cascade=\"<cascade_path>\" [filename|camera_index]\n" );
//		//return -1;
//	}
//}
//CDlgFaceDetect::~CDlgFaceDetect()
//{
//	if( capture )
//	{
//		cvReleaseImage( &frame_copy );
//		cvReleaseImage( &gray );
//		cvReleaseCapture( &capture );
//		S_DEL(m_pTexCapture);
//	}
//
//	cvDestroyWindow("result");
//}
//void CDlgFaceDetect::OnControlRegister()
//{
//	CUIDialog::OnControlRegister();
//}
//
//const int FRAME_WIDTH = 176;
//const int FRAME_HEIGHT = 114;
//const int TEXTURE_WIDTH = 512;
//const int TEXTURE_HEIGHT = 512;
//bool CDlgFaceDetect::OnInitDialog()
//{
//	storage = cvCreateMemStorage(0);
//	capture = cvCaptureFromCAM(0);
//	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
//	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
//	return true;
//}
//void detect_and_draw( IplImage* img )
//{
//
//}
//
//void CDlgFaceDetect::OnFrameMove(double fTime, float fElapsedTime)
//{
//	if (!IsVisible())
//	{
//		return;
//	}
//	if( !cvGrabFrame( capture ))
//		return;
//	frame = cvRetrieveFrame( capture );
//	if( !frame )
//		return;
//	if( !frame_copy )
//	{
//		frame_copy = cvCreateImage( cvSize(frame->width,frame->height),
//		IPL_DEPTH_8U, frame->nChannels );
//		gray = cvCreateImage( cvSize(cvRound (frame->width),
//		cvRound (frame->height)), 8, 1 );
//		m_pTexCapture = GetRenderSystem().GetTextureMgr().CreateDynamicTexture(TEXTURE_WIDTH,TEXTURE_HEIGHT);
//	}
//
//	if( frame->origin == IPL_ORIGIN_TL )
//		cvCopy( frame, frame_copy, 0 );
//	else
//		cvFlip( frame, frame_copy, 0 );
//
//	static CvScalar colors={0,0,255};
//
//	//cvResize( frame_copy, small_img, CV_INTER_NN/*CV_INTER_LINEAR*/ );
//	cvCvtColor( frame_copy, gray, CV_BGR2GRAY );
//	cvEqualizeHist( gray, gray );// 直方图等化
//	cvClearMemStorage( storage );
//
//	if( cascade&&m_bAR)
//	{
//		double t = (double)cvGetTickCount();
//		CvSeq* faces = cvHaarDetectObjects( gray, cascade, storage,
//			1.1, 2, 0/*CV_HAAR_DO_CANNY_PRUNING*/,
//			cvSize(30, 30) );
//		t = (double)cvGetTickCount() - t;
//		printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
//		for(int i = 0; i < (faces ? min(faces->total,1) : 0); i++ )
//		{
//			CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
//
//			m_rcFace.left=r->x;
//			m_rcFace.top=r->y;
//			m_rcFace.right=r->x+r->width;
//			m_rcFace.bottom=r->y+r->height;
//
//			//static Vec2D spt1=Vec2D(0,0)*0.5f;
//			//static Vec2D spt2=spt1;
//			//static Vec2D spt3 = Vec2D(pt1.x*2.0f/FRAME_WIDTH-1.0f,pt1.y*2.0f/FRAME_HEIGHT-1.0f);
//			//static Vec2D spt4 = Vec2D(pt2.x*2.0f/FRAME_WIDTH-1.0f,pt2.y*2.0f/FRAME_HEIGHT-1.0f);
//			//spt1+=(spt3-spt1)*0.2f;
//			//spt2+=(spt4-spt2)*0.2f;
//			//m_vEye.z= -1.0f/abs(spt1.x-spt2.x);
//			//m_vEye.x= (spt1.x+spt2.x)*0.5f*m_vEye.z;
//			//m_vEye.y= (spt1.y+spt2.y)*0.5f*m_vEye.z;
//			//getModelDisplay().SetRealEye(m_vEye);
//			//cvRectangle(frame_copy, pt1, pt2, colors, 3, 8, 0 );
//		}
//	}
//	if (m_pTexCapture)
//	{
//		unsigned long* imageData = new unsigned long[TEXTURE_WIDTH*TEXTURE_HEIGHT];
//		for (int y=0; y<FRAME_HEIGHT; ++y)
//		{
//			unsigned long* pDest= (unsigned long*)imageData+TEXTURE_WIDTH*y;
//			char* pSrc= (frame_copy->imageData+frame_copy->widthStep*y);
//			for (int x=0; x<FRAME_WIDTH; ++x)
//			{
//				unsigned char r,g,b;
//				b=*pSrc;pSrc++;
//				g=*pSrc;pSrc++;
//				r=*pSrc;pSrc++;
//				*pDest = 0xFF000000+b+(g<<8)+(r<<16);
//				//*pDest=Color32(r,g,b).c;
//				//*pDest = 0xFF000000+(*++pSrc)+((*++pSrc)<<8)+((*++pSrc)<<16);
//				pDest++;
//				//pSrc+=3;
//			}
//		}
//		m_pTexCapture->FillLevel(imageData,TEXTURE_WIDTH*TEXTURE_HEIGHT*4,TEXTURE_WIDTH,TEXTURE_HEIGHT,0);
//		delete imageData;
//	}
//}
//
//void CDlgFaceDetect::OnFrameRender(const Matrix& mTransform, double fTime, float fElapsedTime)
//{
//	CDlgBaseEditor::OnFrameRender(fTime, fElapsedTime);
//	if (m_pTexCapture)
//	{
//		GetRenderSystem().SetTexture(0, m_pTexCapture);
//		RECT rcSrc= {0,0,FRAME_WIDTH,FRAME_HEIGHT};
//		GetGraphics().DrawQuad(rcSrc, GetBoundingBox(), TEXTURE_WIDTH, TEXTURE_HEIGHT, 0xFFFFFFFF);
//	}
//	Matrix mText;
//	mText.unit();
//	mText._11=(m_rcFace.right-m_rcFace.left)/256.0f;
//	mText._22=mText._11;
//	mText._41=(m_rcFace.right+m_rcFace.left)*0.5f;
//	mText._42=(m_rcFace.top+m_rcFace.bottom)*0.5f;
//
//	RECT rc = {0,0,2000,2000};
//	GetTextRender().BuildUBB(&m_ubb,L"[color=13,2,112][size=16]I thought whar i'd do was. I'd pretend I was one of those deaf-mutes[/size][/color]",rc);
//	size_t uSize = m_ubb.m_VB.size();
//	if (uSize>0)
//	{
//		float fLength = m_ubb.m_VB[uSize-1].p.x+10.0f;
//		for (size_t i=0; i<uSize; ++i)
//		{
//			float fRadian = -(m_ubb.m_VB[i].p.x)*PI*2.0f/fLength-(float)fTime;
//			float r = 104.0f-m_ubb.m_VB[i].p.y;
//			m_ubb.m_VB[i].p.x=sin(fRadian)*r;
//			m_ubb.m_VB[i].p.y=cos(fRadian)*r;
//			m_ubb.m_VB[i].p=mText*m_ubb.m_VB[i].p;
//			m_ubb.m_VB[i].c=0xFF0D0270;
//		}
//	}
//
//	unsigned long uTexLaughingMan1 = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/UI/Texture/LaughingMan1.tga");
//	unsigned long uTexLaughingMan2 = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/UI/Texture/LaughingMan2.tga");
//				GetRenderSystem().SetTexture(0, uTexLaughingMan1);
//
//	GetRenderSystem().SetDepthBufferFunc(false,false);
//	GetRenderSystem().SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
//	RECT rcDest={m_rcFace.left+m_x,m_rcFace.top+m_y,m_rcFace.right+m_x,m_rcFace.bottom+m_y};
//	GetGraphics().DrawTex(rcDest,uTexLaughingMan1,0xFFFFFFFF);
//	GetRenderSystem().setWorldMatrix(mText);
//	//GetTextRender().DrawUBB(&m_ubb);
//	GetRenderSystem().setWorldMatrix(Matrix::UNIT);
//	GetGraphics().DrawTex(rcDest,uTexLaughingMan2,0xFFFFFFFF);
//}

//{
//	size_t uIconSize = 200;
//	//Matrix mText;
//	//mText.unit();
//	//mText._11=1;
//	//mText._22=1;
//	//mText._14=m_rcBoundingBox.right-uIconSize/2;
//	//mText._24=m_rcBoundingBox.top+uIconSize/2;
//
//	//CUBB m_ubb;
//	//RECT rc = {0,0,2000,2000};
//	//GetTextRender().BuildUBB(&m_ubb,L"[color=13,2,112]I thought whar I'd do was. I'd pretend I was one of those deaf-mutes[/color]",rc);
//	//size_t uSize = m_ubb.m_VB.size();
//	//if (uSize>0)
//	//{
//	//	float fLength = m_ubb.m_VB[uSize-1].p.x+10.0f;
//	//	for (size_t i=0; i<uSize/4; ++i)
//	//	{
//	//		float w=m_ubb.m_VB[i*4+1].p.x-m_ubb.m_VB[i*4].p.x;
//	//		float h=m_ubb.m_VB[i*4+3].p.y-m_ubb.m_VB[i*4].p.y;
//	//		float fCenter = (m_ubb.m_VB[i*4].p.x+m_ubb.m_VB[i*4+1].p.x)*0.5f;
//	//		float fRadian = -fCenter*PI*2.0f/fLength-(float)fTime;
//	//		float r = 104.0f-m_ubb.m_VB[i*4+2].p.y;
//
//	//		Vec3D vPos(sin(fRadian)*r, cos(fRadian)*r,0);
//	//		Vec3D vNormal = vPos;
//	//		vPos = Vec3D(fCenter,0,0);
//	//		vNormal = Vec3D(1,0,0);
//
//	//		vNormal.normalize();
//	//		Vec3D vRight = vNormal.cross(Vec3D(0.0f,0.0f,1.0f));
//
//	//		vPos=mText*vPos;
//	//		vPos=Vec3D(int(vPos.x),int(vPos.y),int(vPos.z));
//
//	//		/*m_ubb.m_VB[i*4+3].p=Vec4D(vPos-vRight*(int(w/2)),1.0f);
//	//		m_ubb.m_VB[i*4+2].p=Vec4D(vPos+vRight*(w-int(w/2)),1.0f);
//	//		m_ubb.m_VB[i*4].p=m_ubb.m_VB[i*4+3].p-Vec4D(h*vNormal,0.0f);
//	//		m_ubb.m_VB[i*4+1].p=m_ubb.m_VB[i*4+2].p-Vec4D(h*vNormal,0.0f);*/
//
//	//		m_ubb.m_VB[i*4].c=0xFF0D0270;
//	//		m_ubb.m_VB[i*4+1].c=0xFF0D0270;
//	//		m_ubb.m_VB[i*4+2].c=0xFF0D0270;
//	//		m_ubb.m_VB[i*4+3].c=0xFF0D0270;
//	//	}
//	//	for (size_t i=0; i<uSize; ++i)
//	//	{
//	//		Vec4D vPos = m_ubb.m_VB[i].p;
//	//		m_ubb.m_VB[i].p.x = 400.0f-vPos.y;
//	//		m_ubb.m_VB[i].p.y = vPos.x;
//
//	//	}
//	//}
//
//	R.setWorldMatrix(Matrix::UNIT);
//	unsigned long uTexLaughingMan1 = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/LaughingMan1.tga");
//	unsigned long uTexLaughingMan2 = GetRenderSystem().GetTextureMgr().RegisterTexture("Data/Textures/LaughingMan2.tga");
//	//R.SetTexture(0, uTexLaughingMan1);
//
//	//R.SetDepthBufferFunc(false,false);
//	//R.SetSamplerFilter(0, TEXF_LINEAR, TEXF_LINEAR, TEXF_LINEAR);
//
//	RECT rcDest={m_rcBoundingBox.right-uIconSize,m_rcBoundingBox.top,m_rcBoundingBox.right,m_rcBoundingBox.top+uIconSize};
//	GetGraphics().DrawTex(rcDest,uTexLaughingMan1,0xFFFFFFFF);
//	//R.setWorldMatrix(mText);
//	//GetTextRender().DrawUBB(&m_ubb);
//	R.setWorldMatrix(Matrix::UNIT);
//	GetGraphics().DrawTex(rcDest,uTexLaughingMan2,0xFFFFFFFF);
//}