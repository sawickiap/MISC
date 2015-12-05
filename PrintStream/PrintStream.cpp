// PrintStream.cpp
// Author:  Adam Sawicki, www.asawicki.info, adam__REMOVE__@asawicki.info
// Version: 2.0, 2015-12-05
// License: Public Domain

#include "PrintStream.hpp"
#include <cstdarg>
#include <cassert>

#ifdef UNICODE
	#define TSTRLEN wcslen
	#define TPRINTF wprintf
	#define TVPRINTF vwprintf
	#define TVSPRINTF_S vswprintf_s
	#define TVSCPRINTF _vscwprintf
	#define TFOPEN_S _wfopen_s
	#define TFPRINTF fwprintf
	#define TVFPRINTF vfwprintf
#else
	#define TSTRLEN strlen
	#define TPRINTF printf
	#define TVPRINTF vprintf
	#define TVSPRINTF_S vsprintf_s
	#define TVSCPRINTF _vscprintf
	#define TFOPEN_S fopen_s
	#define TFPRINTF fprintf
	#define TVFPRINTF vfprintf
#endif

static const size_t SMALL_BUF_SIZE = 256;

////////////////////////////////////////////////////////////////////////////////
// CPrintStream

void CPrintStream::print(const TCHAR* str)
{
	print(str, TSTRLEN(str));
}

void CPrintStream::print(const TCHAR* str, size_t strLen)
{
	if(strLen)
	{
		// String is null-terminated.
		if(str[strLen - 1] == 0)
			print(str);
		else
		{
			bool useSmallBuf = strLen < SMALL_BUF_SIZE;
			TCHAR smallBuf[SMALL_BUF_SIZE];
			std::vector<TCHAR> bigBuf(useSmallBuf ? 0 : strLen + 1);
			TCHAR* bufPtr = useSmallBuf ? smallBuf : &bigBuf[0];
			memcpy(bufPtr, str, strLen * sizeof(TCHAR));
			bufPtr[strLen] = 0;
			print(bufPtr);
		}
	}
}

void CPrintStream::print(const TSTRING& str)
{
	print(str.c_str(), str.length());
}

void CPrintStream::vprintf(const TCHAR* format, va_list argList)
{
	size_t dstLen = (size_t)::TVSCPRINTF(format, argList);
	if(dstLen)
	{
		bool useSmallBuf = dstLen < SMALL_BUF_SIZE;
		TCHAR smallBuf[SMALL_BUF_SIZE];
		std::vector<TCHAR> bigBuf(useSmallBuf ? 0 : dstLen + 1);
		TCHAR* bufPtr = useSmallBuf ? smallBuf : &bigBuf[0];
		::TVSPRINTF_S(bufPtr, dstLen + 1, format, argList);
		print(bufPtr, dstLen);
	}

}

void CPrintStream::printf(const TCHAR* format, ...)
{
	va_list argList;
	va_start(argList, format);
	vprintf(format, argList);
	va_end(argList);
}

////////////////////////////////////////////////////////////////////////////////
// CConsolePrintStream

void CConsolePrintStream::print(const TCHAR* str, size_t strLen)
{
	assert(strLen <= INT_MAX);
	::TPRINTF(_T("%.*s"), (int)strLen, str);
}

void CConsolePrintStream::print(const TCHAR* str)
{
	::TPRINTF(_T("%s"), str);
}

void CConsolePrintStream::vprintf(const TCHAR* format, va_list argList)
{
	::TVPRINTF(format, argList);
}

////////////////////////////////////////////////////////////////////////////////
// CFilePrintStream

CFilePrintStream::CFilePrintStream() :
	m_File(nullptr)
{
}

CFilePrintStream::CFilePrintStream(const TCHAR* filePath, const TCHAR* mode) :
	m_File(nullptr)
{
	Open(filePath, mode);
}

CFilePrintStream::~CFilePrintStream()
{
	Close();
}

bool CFilePrintStream::Open(const TCHAR* filePath, const TCHAR* mode)
{
	Close();
	bool success = TFOPEN_S(&m_File, filePath, mode) == 0;
	if(!success)
	{
		m_File = nullptr;
		// Handle error somehow.
		assert(0);
	}
	return success;
}

void CFilePrintStream::Close()
{
	if(m_File)
	{
		fclose(m_File);
		m_File = nullptr;
	}
}

void CFilePrintStream::print(const TCHAR* str, size_t strLen)
{
	if(IsOpened())
	{
		assert(strLen <= INT_MAX);
		::TFPRINTF(m_File, _T("%.*s"), (int)strLen, str);
	}
	else
		assert(0);
}

void CFilePrintStream::print(const TCHAR* str)
{
	if(IsOpened())
		::TFPRINTF(m_File, _T("%s"), str);
	else
		assert(0);
}

void CFilePrintStream::vprintf(const TCHAR* format, va_list argList)
{
	if(IsOpened())
		::TVFPRINTF(m_File, format, argList);
	else
		assert(0);
}

////////////////////////////////////////////////////////////////////////////////
// CMemoryPrintStream

void CMemoryPrintStream::print(const TCHAR* str, size_t strLen)
{
	m_BufPtr->insert(m_BufPtr->end(), str, str + strLen);
}

////////////////////////////////////////////////////////////////////////////////
// CDebugPrintStream

void CDebugPrintStream::print(const TCHAR* str)
{
	OutputDebugString(str);
}
