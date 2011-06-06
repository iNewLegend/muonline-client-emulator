#pragma once

#include <windows.h>
#include <time.h>

// define
typedef char int8;
typedef __int16 int16;
typedef __int32 int32;
typedef unsigned __int64 uint64;

// obj define
const int _MAX_NAMESIZE	=16;

// STL
#include <string>
#include <vector>
#include <map>

//----
// # Helper macros to build member functions that access member variables
//----
#define SET_VARIABLE(x,y,z)				void set##y(x t)	{ z = t; };
#define GET_VARIABLE(x,y,z)				x get##y()		{ return z; };
#define GET_SET_VARIABLE(x,y,z)			SET_VARIABLE(x,z,y##z) GET_VARIABLE(x,z,y##z)
//----
#define CONST_SET_VARIABLE(x,y,z)		void set##y(const x t)	{ z = t; };
#define CONST_GET_VARIABLE(x,y,z)		const x get##y()const	{ return z; };
#define CONST_GET_SET_VARIABLE(x,y,z)	CONST_SET_VARIABLE(x,z,y##z) CONST_GET_VARIABLE(x,z,y##z)
//----
#define GET_SET_VARIABLE_STRING(y)		CONST_SET_VARIABLE(char*,y,m_str##y) CONST_GET_VARIABLE(char*,y,m_str##y.c_str())
//----
#define FOR_IN(t,i,c)					for (t::iterator i = c.begin();i != c.end(); ++i)
#define CONST_FOR_IN(t,i,c)				for (t::const_iterator i = c.begin();i != c.end(); ++i)
//----

#ifndef S_REL
#define S_REL(p) {if(p){ try{p->Release();}catch(...){/*LOGERROR("CATCH: *** S_REL() crash! ***\n")*/;} p=NULL;}}
#endif
#ifndef S_DEL
#define S_DEL(p) {if(p){ try{delete p;}catch(...){/*LOGERROR("CATCH: *** S_DEL() crash! ***\n")*/;} p=NULL;}}
#endif
#ifndef S_DELS
#define S_DELS(p) {if(p){ try{delete[] p;}catch(...){/*LOGERROR("CATCH: *** S_DELS() crash! ***\n")*/;} p=NULL;}}
#endif
#ifndef LERP
#define LERP(x, y, s)	(x + s*(y - x))
#endif

#ifndef ARGB
#define ARGB(a,r,g,b)          ((COLORREF) ((BYTE)(r)| (((WORD)(BYTE)(g))<<8)| (((unsigned long)(BYTE)(b))<<16)| (((unsigned long)(BYTE)(a))<<24)))
#endif

// function
bool SafeCopy(char * pTarget, const char * pSource, int nBufLen = 0);

// ÆäËü
#include <assert.h>

//inline void	log_assert(const char* type, const char* str, const char* file, int line) { LOGERROR("¡ï%s(%s)¡ï in %s, %d", type, str, file, line); }

#ifdef	LOCAL_DEBUG
#include <assert.h>
#undef	ASSERT
#define		ASSERT(x)	(void)( (x) || (assert(!"(#x)"),log_assert("ASSERT", #x, __FILE__, __LINE__), 0) )
#define		CHECK(x)	do{ if(!(x)) { assert(!(#x)),log_assert("CHECK", #x, __FILE__, __LINE__); return; } }while(0)
#define		CHECKF(x)	do{ if(!(x)) { assert(!(#x)),log_assert("CHECKF", #x, __FILE__, __LINE__); return 0; } }while(0)
#define		IF_NOT(x)	if( !(x) ? ( assert(!(#x)),log_assert("IF_NOT", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_NOT_(x)	if( !(x) ? ( assert(!"(#x)"),log_assert("IF_NOT_", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_OK(x)	if( (x) ? 1 : ( assert(!(#x)),log_assert("IF_OK", #x, __FILE__, __LINE__),0 ) )
#define		IF_OK_(x)	if( (x) ? 1 : ( assert(!"(#x)"),log_assert("IF_OK_", #x, __FILE__, __LINE__),0 ) )
#define		PURE_VIRTUAL_FUNCTION_0		= 0;
#define		PURE_VIRTUAL_FUNCTION		= 0;
#define		PURE_VIRTUAL_FUNCTION_(x)	= 0;
#else // LOCAL_DEBUG
#undef	ASSERT
#define		ASSERT(x)	(void)( (x) || (log_assert("ASSERT", #x, __FILE__, __LINE__), 0) )
#define		CHECK(x)	do{ if(!(x)) { log_assert("CHECK", #x, __FILE__, __LINE__); return; } }while(0)
#define		CHECKF(x)	do{ if(!(x)) { log_assert("CHECKF", #x, __FILE__, __LINE__); return 0; } }while(0)
#define		IF_NOT(x)	if( !(x) ? ( log_assert("IF_NOT", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_NOT_(x)	if( !(x) ? ( log_assert("IF_NOT_", #x, __FILE__, __LINE__),1 ) : 0 )
#define		IF_OK(x)	if( (x) ? 1 : ( log_assert("IF_OK", #x, __FILE__, __LINE__),0 ) )
#define		IF_OK_(x)	if( (x) ? 1 : ( log_assert("IF_OK_", #x, __FILE__, __LINE__),0 ) )
#define		PURE_VIRTUAL_FUNCTION_0		{ ASSERT(!"PURE_VIRTUAL_FUNCTION_0"); return 0; }
#define		PURE_VIRTUAL_FUNCTION		{ ASSERT(!"PURE_VIRTUAL_FUNCTION"); }
#define		PURE_VIRTUAL_FUNCTION_(x)	{ ASSERT(!"PURE_VIRTUAL_FUNCTION"); return x(); }
#endif // LOCAL_DEBUG

void	LogSave		(const char* fmt, ...);
#define		DEBUG_TRY		try{
#define		DEBUG_CATCH(x)	}catch(...){ ::LogSave("CATCH: *** " x " crash()! ***\n"); }
#define		DEBUG_CATCH2(x,y)	}catch(...){ ::LogSave((x),(y)); ::LogSave("CATCH: *** " x " crash()! ***\n"); }
#define		DEBUG_CATCH3(x,y,z)	}catch(...){ ::LogSave((x),(y),(z)); ::LogSave("CATCH: *** " x " crash()! ***\n"); }

// int xCount = 0;
// while(...) { ...; DEAD_LOOP_BREAK(xCount, 10000) }
#define		DEAD_LOOP_BREAK(x,n)	{ if(++(x) > (n)){ ASSERT(!"DEAD_LOCK_BREAK"); break; } }

#define		TYPENAME	typename

template<TYPENAME T>
	inline void		safe_release(T*& pT)			{ pT->Release(); pT=NULL; }
#define		SafeRelease	safe_release

std::string ws2s(const std::wstring& ws);
std::wstring s2ws(const std::string& s);
std::wstring i2ws(int i, int nRadix=10);
std::wstring f2ws(float f,const int slen,const int alen);
std::string Format(const char* pszFormat, ...);
std::wstring FormatW(const wchar_t* pwszFormat, ...);
void Tokenizer(const std::string& in, std::vector<std::string>& out, const std::string& delimiters=",");
void TokenizerW(const std::wstring& in, std::vector<std::wstring>& out, const std::wstring& delimiters=L",");
void hexStdStr2Date(const std::string& str, void* pDate, size_t size);
std::string date2HexStdStr(void* pDate, size_t size);

	template <typename T>
	void hexStdStr2Date(const std::string& str, T& date)
	{
		hexStdStr2Date(str,&date,sizeof(T));
	}

	template <typename T>
	std::string date2HexStdStr(const T& date)
	{
		return date2HexStdStr(&date, sizeof(T));
	}