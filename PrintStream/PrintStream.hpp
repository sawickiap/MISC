// PrintStream.hpp
// Author:  Adam Sawicki, www.asawicki.info, adam__REMOVE__@asawicki.info
// Version: 2.0, 2015-12-05
// License: Public Domain

#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdint>

#ifdef UNICODE
	#define TSTRING std::wstring
#else
	#define TSTRING std::string
#endif

// Abstract base class.
// Derived class must implement at least first or second version of print method
// and share inherited ones with:
// using CPrintStream::print;
class CPrintStream
{
public:
	virtual ~CPrintStream() { }
	// Default implementation copies to a temporary null-terminated string and rediects it to print(str).
	virtual void print(const TCHAR* str, size_t strLen);
	// Default implementation calculates length and redirects to print(str, strLen).
	virtual void print(const TCHAR* str);
	// Default implementation redirects to print(str, strLen).
	virtual void print(const TSTRING& str);
	// Default implementation formats string in memory and redirects it to print(str, strLen).
	virtual void vprintf(const TCHAR* format, va_list argList);
	// Redirects to print(format, argList).
	void printf(const TCHAR* format, ...);
};

// Prints to standard output.
class CConsolePrintStream : public CPrintStream
{
public:
	using CPrintStream::print;
	virtual void print(const TCHAR* str, size_t strLen);
	virtual void print(const TCHAR* str);
	virtual void vprintf(const TCHAR* format, va_list argList);
};

// Prints to file.
class CFilePrintStream : public CPrintStream
{
public:
	// Initializes object with empty state.
	CFilePrintStream();
	// Opens file during initialization.
	CFilePrintStream(const TCHAR* filePath, const TCHAR* mode);
	// Automatically closes file.
	~CFilePrintStream();

	// mode: Like in fopen, e.g. "wb", "a".
	bool Open(const TCHAR* filePath, const TCHAR* mode);
	void Close();
	bool IsOpened() const { return m_File != nullptr; }

	using CPrintStream::print;
	virtual void print(const TCHAR* str, size_t strLen);
	virtual void print(const TCHAR* str);
	virtual void vprintf(const TCHAR* format, va_list argList);

private:
	FILE* m_File;
};

// Appends to internal or external memory buffer.
class CMemoryPrintStream : public CPrintStream
{
public:
	typedef std::vector<TCHAR> Buf_t;

	CMemoryPrintStream(Buf_t* externalBuf = nullptr) :
		m_BufPtr(externalBuf ? externalBuf : &m_InternalBuf)
	{
	}

	const Buf_t* GetBuf() const { return m_BufPtr; }
	Buf_t* GetBuf() { return m_BufPtr; }
	void GetAsString(TSTRING& out) const { out.assign(m_BufPtr->begin(), m_BufPtr->end()); }

	using CPrintStream::print;
	virtual void print(const TCHAR* str, size_t strLen);

private:
	Buf_t m_InternalBuf;
	// Pointer to either m_InternalBuf (if using internal buffer) or external buffer passed to constructor.
	// Not null-terminated.
	Buf_t* m_BufPtr;
};

// Prints to OutputDebugString.
class CDebugPrintStream : public CPrintStream
{
public:
	using CPrintStream::print;
	virtual void print(const TCHAR* str);
};
