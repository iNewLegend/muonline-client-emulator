#ifdef WZAG_EXPORTS
#define WZAG_API extern "C" __declspec(dllexport)
#else
#define WZAG_API extern "C" __declspec(dllimport)
#endif

WZAG_API bool	AGInit() ;
WZAG_API int	AGConnectSetFS() ;
WZAG_API char * AGGetDataBuffer()  ;
WZAG_API int	AGGetDataBufferSize() ;
WZAG_API void	AGGetKey(HANDLE handle,char* key,int startindex,int len)   ;
WZAG_API int	AGReleaseHandle() ;
WZAG_API int	AGRequestData(int NoneUse,int GetType)  ;
WZAG_API int	AGSetInfo(int Language,char * Partition,int un0,char * SrvVer,char* SrvName,int SrvType,char * Addr7)    ;
WZAG_API int	AGSetSubInfo()  ;