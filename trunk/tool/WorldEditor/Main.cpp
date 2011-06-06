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
	// 通过DXUT来处理消息循环并分派渲染调用。当在空闲时间和处理窗口消息的
	// 时间间隔时，框架将调用OnFrameMove和OnFrameRender回调函数。
	//GetVideo()->Play(L"Data/yso_logo.avi");
	// 	if ( GetScript()->DoFile( GetConfig().GetScriptFilename() ) )
	// 	{
	// 		LOG("LoadScript Error!")
	// 	}
}


