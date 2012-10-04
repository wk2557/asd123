#ifndef __LPR_UTILITY_H__
#define __LPR_UTILITY_H__

#include "LPRResult.h"
#include "LPRStructure.h"
#include "stdarg.h"
#include <string>
#include <vector>

#if defined(WIN32)
	#ifdef LPRUTILITY_EXPORTS
		#define LPRUTILITY_API __declspec(dllexport)
	#else
		#define LPRUTILITY_API __declspec(dllimport)
	#endif
#elif defined(LPR_LINUX)
	#ifdef LPRUTILITY_EXPORTS
		#define LPRUTILITY_API __attribute__ ((visibility("default")))
	#else
		#define LPRUTILITY_API
	#endif
#else
	#error "No platform specified!"
#endif

#ifdef __cplusplus
extern "C" {
#endif

LPRUTILITY_API void*	  __stdcall LPRSetting_Create();
LPRUTILITY_API void		  __stdcall LPRSetting_Destroy( void* pObject );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_LoadFromFile( void* pObject, const char* pFileName, const char* pReserved );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_SaveToFile( const void* pObject, const char* pFileName, const char* pReserved );
LPRUTILITY_API bool		  __stdcall LPRSetting_HasKey( const void* pObject, const char* pKeyName );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_GetIntValue( const void* pObject, const char* pKeyName, int* pValue );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_GetFloatValue( const void* pObject, const char* pKeyName, float* pValue );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_GetBoolValue( const void* pObject, const char* pKeyName, bool* pValue );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_GetStringValue( const void* pObject, const char* pKeyName, char* pBuf, int* pLen );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_SetIntValue( void* pObject, const char* pKeyName, int value );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_SetFloatValue( void* pObject, const char* pKeyName, float value );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_SetBoolValue( void* pObject, const char* pKeyName, bool value );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_SetStringValue( void* pObject, const char* pKeyName, const char* pStr );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_AddIntKey( void* pObject, const char* pKeyName, int value, bool bOverwrite );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_AddFloatKey( void* pObject, const char* pKeyName, float value, bool bOverwrite );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_AddBoolKey( void* pObject, const char* pKeyName, bool value, bool bOverwrite );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_AddStringKey( void* pObject, const char* pKeyName, const char* pStr, bool bOverwrite );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_DeleteKey( void* pObject, const char* pKeyName );
LPRUTILITY_API LPRRESULT  __stdcall LPRSetting_GetAllKeys( const void* pObject, char* pBuffer, int* pBufLen );

class LPRSetting
{
public:
	LPRSetting() { m_pObject = LPRSetting_Create(); }
	~LPRSetting() { LPRSetting_Destroy( m_pObject ); }

	// 描述:   从文件读入配置
	// 参数:   pFileName - 文件名, 可以是绝对路径，也可以是相对路径。
	//         如果是相对路径的话，以应用程序所在目录作为基准路径。
	//         pReserved - 保留，必须设为NULL。   
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT LoadFromFile( const char* pFileName, const char* pReserved )
	{
		return LPRSetting_LoadFromFile( m_pObject, pFileName, pReserved );
	}

	// 描述:   保存配置到文件
	// 参数:   pFileName - 文件名, 可以是绝对路径，也可以是相对路径。
	//         如果是相对路径的话，以应用程序所在目录作为基准路径。
	//         pReserved - 保留，必须设为NULL。   
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT SaveToFile( const char* pFileName, const char* pReserved ) const
	{
		return LPRSetting_SaveToFile( m_pObject, pFileName, pReserved );
	}

	// 描述:   查询是否有特定的配置参数
	// 参数:   pKeyName - 参数名
	// 返回值: true表示有此参数，false表示无此参数
	bool HasKey( const char* pKeyName )
	{
		return LPRSetting_HasKey( m_pObject, pKeyName );
	}
	
	// 描述:   获得所有参数的名字
	// 参数:   pBuffer - 用于存放参数名的缓冲区
	//		   pBufLen - 缓冲区长度
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	// 说明:   如果函数执行成功，pBuffer内将是顺序存放的各个参数的名字字符串
	//			（包含结尾的'\0'），所以字符串以空字符串（'\0'） 结尾。pBufLen
	//			内存放的是数据的实际长度。如果函数因为缓冲区不够大而失败，那么
	//			pBufLen内存放的是需要的最小缓冲区长度。
	LPRRESULT GetAllKeys( char* pBuffer, int* pBufLen ) const
	{
		return LPRSetting_GetAllKeys( m_pObject, pBuffer, pBufLen );
	}

	// 描述:   辅助函数，功能同GetAllKeys( char* pBuffer, int* pBufLen )，只是数据的
	//		   数据以字符串数组形式存放
	std::vector<std::string> GetAllKeys() const
	{
		std::vector<std::string>	result;

		int len = 0;
		LPRRESULT lr = LPRSetting_GetAllKeys( m_pObject, NULL, &len );
		if( lr != LPR_BUFFER_TOO_SMALL )
			return result;
		
		char* pBuf = new char[len];
		if( !pBuf )
			return result;

		lr = LPRSetting_GetAllKeys( m_pObject, pBuf, &len );
		if( lr < 0 )
		{
			delete[] pBuf;
			return result;
		}

		char* pCur = pBuf;
		std::string str = pCur;
		pCur += str.length()+1;
		while( !str.empty() )
		{
			result.push_back( str );
			str = pCur;
			pCur += str.length()+1;
		}

		delete[] pBuf;
		return result;
	}
	// 描述:   读取整数类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pValue - 用于存放读取的整型值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT GetIntValue( const char* pKeyName, int* pValue ) const
	{
		return LPRSetting_GetIntValue( m_pObject, pKeyName, pValue );
	}

	// 描述:   读取浮点类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pValue - 用于存放读取的浮点值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT GetFloatValue( const char* pKeyName, float* pValue ) const
	{
		return LPRSetting_GetFloatValue( m_pObject, pKeyName, pValue );
	}

	// 描述:   读取布尔类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pValue - 用于存放读取的布尔值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT GetBoolValue( const char* pKeyName, bool* pValue ) const
	{
		return LPRSetting_GetBoolValue( m_pObject, pKeyName, pValue );
	}
	
	// 描述:   读取字符串类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pBuf - 接收字符串数据的数据块。
	//         pLen - pBuf数据块的长度，函数如果执行成功，*pLen存放字符串的长度
	//         （不包含尾部的'\0'），如果函数失败并返回LPR_BUFFER_TOO_SMALL，表示
	//         数据块太小，此时*pLen中存放着最小数据块长度的负值（比如 -128）。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT GetStringValue( const char* pKeyName, char* pBuf, int* pLen ) const
	{
		return LPRSetting_GetStringValue( m_pObject, pKeyName, pBuf, pLen );
	}

	// 描述:   修改整数类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 新的参数值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT SetIntValue( const char* pKeyName, int value )
	{
		return LPRSetting_SetIntValue( m_pObject, pKeyName, value );
	}

	// 描述:   修改浮点类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 新的参数值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT SetFloatValue( const char* pKeyName, float value )
	{
		return LPRSetting_SetFloatValue( m_pObject, pKeyName, value );
	}

	// 描述:   修改布尔类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 新的参数值。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT SetBoolValue( const char* pKeyName, bool value )
	{
		return LPRSetting_SetBoolValue( m_pObject, pKeyName, value );
	}
	
	// 描述:   修改字符串类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pStr - 新的字符串值，字符串以NULL结尾。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT SetStringValue( const char* pKeyName, const char* pStr )
	{
		return LPRSetting_SetStringValue( m_pObject, pKeyName, pStr );
	}

	// 描述:   增加整数类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 参数值。
	//         bOverWrite - 如果原先存在pKeyName命名的参数的话,是否覆盖原来的参数。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT AddIntKey( const char* pKeyName, int value, bool bOverWrite )
	{
		return LPRSetting_AddIntKey( m_pObject, pKeyName, value, bOverWrite );
	}

	// 描述:   增加浮点类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 参数值。
	//         bOverWrite - 如果原先存在pKeyName命名的参数的话,是否覆盖原来的参数。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT AddFloatKey( const char* pKeyName, float value, bool bOverWrite )
	{
		return LPRSetting_AddFloatKey( m_pObject, pKeyName, value, bOverWrite );
	}

	// 描述:   增加布尔类型的配置参数
	// 参数:   pKeyName - 参数名
	//         value - 参数值。
	//         bOverWrite - 如果原先存在pKeyName命名的参数的话,是否覆盖原来的参数。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT AddBoolKey( const char* pKeyName, bool value, bool bOverWrite )
	{
		return LPRSetting_AddBoolKey( m_pObject, pKeyName, value, bOverWrite );
	}
	
	// 描述:   增加字符串类型的配置参数
	// 参数:   pKeyName - 参数名
	//         pStr - 字符串值，字符串以NULL结尾。
	//         bOverWrite - 如果原先存在pKeyName命名的参数的话,是否覆盖原来的参数。
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT AddStringKey( const char* pKeyName, const char* pStr, bool bOverWrite )
	{
		return LPRSetting_AddStringKey( m_pObject, pKeyName, pStr, bOverWrite );
	}

	// 描述:   删除配置参数
	// 参数:   pKeyName - 参数名
	// 返回值: 大于等于0表示执行成功，小于0表示失败，失败值参见LPRResult.h
	LPRRESULT DeleteKey( const char* pKeyName )
	{
		return LPRSetting_DeleteKey( m_pObject, pKeyName );
	}
private:
	void*	m_pObject;
};

LPRUTILITY_API void*	  __stdcall LPRLog_Create();
LPRUTILITY_API void		  __stdcall LPRLog_Destroy( void* pObject );
LPRUTILITY_API LPRRESULT  __stdcall LPRLog_Init( void* pObject, const char* pFileName );
LPRUTILITY_API LPRRESULT  __stdcall LPRLog_Fini( const void* pObject );
LPRUTILITY_API LPRRESULT  __stdcall LPRLog_Log( const void* pObject, const char* str, va_list args );

class LPRLog
{
public:
	LPRLog() { m_pObject = LPRLog_Create(); }
	~LPRLog() { LPRLog_Destroy( m_pObject ); }
	LPRRESULT Init( const char* pFileName ) { return LPRLog_Init( m_pObject, pFileName ); }
	LPRRESULT Fini() { return LPRLog_Fini( m_pObject ); }
	LPRRESULT Log( const char* str, ... ) 
	{ 
		va_list args;
		va_start( args, str );
		LPRRESULT lr = LPRLog_Log( m_pObject, str, args ); 
		va_end( args );
		return lr;
	}
private:
	void*	m_pObject;
};

// 一些辅助函数
LPRUTILITY_API LPRRESULT __stdcall LPRRatioPointToPoint( LPRRatioPoint* pRPt, int width, int height, POINT* pPt );
LPRUTILITY_API LPRRESULT __stdcall PointToLPRRatioPoint( POINT* pPt, int width, int height, LPRRatioPoint* pRPt );
LPRUTILITY_API LPRRESULT __stdcall LPRGetExeFilePath( char* pPathBuf, int* pLen, bool needEndSlash );
LPRUTILITY_API LPRRESULT __stdcall LPRGetModuleFilePath( const char* pModuleName, char* pPathBuf, int* pLen, bool needEndSlash );


LPRUTILITY_API const char*		__stdcall LPRGetPlateTypeStr( int type );
LPRUTILITY_API const wchar_t*	__stdcall LPRGetPlateTypeStrW( int type );
LPRUTILITY_API const char*		__stdcall LPRGetPlateColorStr( int color );
LPRUTILITY_API const wchar_t*	__stdcall LPRGetPlateColorStrW( int color );

LPRUTILITY_API const char*		__stdcall LPRGetErrorStr( LPRRESULT err );
LPRUTILITY_API const wchar_t*	__stdcall LPRGetErrorStrW( LPRRESULT err );



#ifdef __cplusplus
}
#endif

#endif //__LPR_UTILITY_H__

