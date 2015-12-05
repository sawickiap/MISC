# PrintStream

A hierarchy of classes that represent abstract concept of a text-based stream that can be printed into, using methods:

	void print(const char* str, size_t strLen);
	void print(const char* str);
	void print(const std::string& str);
	void vprintf(const char* format, va_list argList);
	void printf(const char* format, ...);

Derived classes offer printing to:

- `CConsolePrintStream` - console (standard output), using functions like `printf`.
- `CFilePrintStream` - file, using functions like `fopen`, `fprintf`.
- `CMemoryPrintStream` - buffer in memory, of type `std::vector<char>`, with conversion to `std::string`.
- `CDebugPrintStream` - debug output, using function `OutputDebugString`.

The code is tested on Windows, using Visual Studio 2015 Update 1.

Unicode is supported. Just switch "Character Set" to "Use Unicode Character Set" in Visual Studio project properties and automatically defined macro `UNICODE` will make this code use `wchar_t` instead of `char`, `wprintf` instead of `printf` etc.

License: Public Domain.

External dependencies:

- WinAPI - `<Windows.h>`
- Some elements of standard C++ library (STL)
- Some elements of standard C library
