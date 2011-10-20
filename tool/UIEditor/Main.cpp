#include "MainRoot.h"

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	// Create application object
	CMainRoot app;
	try
	{
		app.Run();
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
}