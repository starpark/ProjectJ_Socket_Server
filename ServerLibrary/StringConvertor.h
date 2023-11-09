#pragma once
template <typename Convertor>
class StringConvertor;
class UTF8ToWchar;
class WcharToUTF8;

using UTF8CHAR = unsigned char;
using WIDECHAR = wchar_t;
using ANSICHAR = char;

#define UTF8_TO_WCHAR(str) (WIDECHAR*)StringConvertor<UTF8ToWchar>((const UTF8CHAR*)str).Get()
#define WCHAR_TO_UTF8(str) (UTF8CHAR*)StringConvertor<WcharToUTF8>((const WIDECHAR*)str).Get()

int Strlen(const ANSICHAR* string);
int Strlen(const UTF8CHAR* string);
int Strlen(const WIDECHAR* string);

template <typename Convertor>
class StringConvertor
{
public:
	using FromType = typename Convertor::FromType;
	using ToType = typename Convertor::ToType;

	void Init(const FromType* source, int sourceLen)
	{
		stringLength_ = Convertor::ConvertedLength(source, sourceLen);
		int bufferSize = stringLength_ + 1;
		ptr_.resize(bufferSize, 0);
		Convertor::Convert(static_cast<ToType*>(ptr_.data()), stringLength_, source, sourceLen);
	}

	StringConvertor(const FromType* source)
	{
		if (source)
		{
			Init(source, Strlen(source));
		}
		else
		{
			stringLength_ = 0;
		}
	}

	StringConvertor(StringConvertor&&) = delete;
	StringConvertor(const StringConvertor&) = delete;
	StringConvertor& operator=(StringConvertor&&) = delete;
	StringConvertor& operator=(const StringConvertor&) = delete;

	const ToType* Get()
	{
		return static_cast<ToType*>(ptr_.data());
	}

private:
	vector<ToType> ptr_;
	int stringLength_;
};

class UTF8ToWchar
{
public:
	using FromType = UTF8CHAR;
	using ToType = WIDECHAR;

	static void Convert(ToType* dest, const int destLen, const FromType* source, const int sourceLen)
	{
		MultiByteToWideChar(CP_UTF8, 0, (LPCCH)source, sourceLen, dest, destLen);
	}

	static int ConvertedLength(const FromType* string, int stringLen)
	{
		return MultiByteToWideChar(CP_UTF8, 0, (LPCCH)string, stringLen, NULL, NULL);
	}
};

class WcharToUTF8
{
public:
	using FromType = WIDECHAR;
	using ToType = UTF8CHAR;

	static void Convert(ToType* dest, const int destLen, const FromType* source, const int sourceLen)
	{
		WideCharToMultiByte(CP_UTF8, 0, source, sourceLen, (LPSTR)dest, destLen, NULL, NULL);
	}

	static int ConvertedLength(const FromType* string, int stringLen)
	{
		return WideCharToMultiByte(CP_UTF8, 0, string, stringLen, NULL, 0, NULL, NULL);
	}
};
