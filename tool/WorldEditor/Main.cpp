#include "MainRoot.h"
#pragma comment(lib, "wininet.lib")

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	// Create application object
	try
	{
		CMainRoot::getInstance().Run();
	}
	catch( std::exception& e )
	{
		// #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		// 		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
		// #else
		// 		std::cerr << "An exception has occured: " << 
		// 			e.getFullDescription().c_str() << std::endl;
		// #endif
	}

	return 0;
	// ͨ��DXUT��������Ϣѭ����������Ⱦ���á����ڿ���ʱ��ʹ�������Ϣ��
	// ʱ����ʱ����ܽ�����OnFrameMove��OnFrameRender�ص�������
	//GetVideo()->Play(L"Data/yso_logo.avi");
	// 	if ( GetScript()->DoFile( GetConfig().GetScriptFilename() ) )
	// 	{
	// 		LOG("LoadScript Error!")
	// 	}
}


