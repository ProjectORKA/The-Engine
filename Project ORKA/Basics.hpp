// ReSharper disable CppUnusedIncludeDirective
#pragma once
#pragma warning(disable : 4996)

#include "OrkaTypes.hpp"

String toString(ULL v);

inline String toString(const LL v)
{
	return std::to_string(v);
}

inline String toString(const Int v)
{
	return std::to_string(v);
}

inline String toString(const Bool v)
{
	if (v) return "true";
	return "false";
}

inline String toString(const Byte v)
{
	return std::to_string(v);
}

inline String toString(const UInt v)
{
	return std::to_string(v);
}

inline String toString(const Char v)
{
	return std::to_string(v);
}

inline String toString(const Long v)
{
	return std::to_string(v);
}

inline String toString(const Char* v)
{
	return v;
}

inline String toString(const Short v)
{
	return std::to_string(v);
}

inline String toString(const ULong v)
{
	return std::to_string(v);
}

inline String toString(const Float v)
{
	return std::to_string(v);
}

inline String toString(const Path& v)
{
	const WString                                    wideString = v.wstring();
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wideString);
}

inline String toString(const Double v)
{
	return std::to_string(v);
}

inline String toString(const UShort v)
{
	return std::to_string(v);
}

inline String toString(const String& v)
{
	return v;
}

inline String toString(const Vec2 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + ")";
}

inline String toString(const Vec3 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + ")";
}

inline String toString(const Vec4 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + "|" + toString(v.w) + ")";
}

inline String toString(const IVec2 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + ")";
}

inline String toString(const IVec3 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + ")";
}

inline String toString(const IVec4 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + "|" + toString(v.w) + ")";
}

inline String toString(const DVec3& v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + ")";
}

inline String toString(const LDouble v)
{
	return std::to_string(v);
}

inline String toString(const UllVec2 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + ")";
}

inline String toString(const UllVec3& v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + ")";
}

inline String toString(const UllVec4& v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + "|" + toString(v.w) + ")";
}

inline String toString(const Matrix& v)
{
	return "\n" + toString(v[0]) + "\n" + toString(v[1]) + "\n" + toString(v[2]) + "\n" + toString(v[3]) + "\n";
}

inline String toString(Byte* v)
{
	return {reinterpret_cast<const char*>(v)};
}

inline String toString(const Byte* v)
{
	return {reinterpret_cast<const char*>(v)};
}

inline String toString(Char* v)
{
	return v;
}

template <typename T>
String toString(T v)
{
	__debugbreak();
	return "(ERROR: String conversion for Type: " + toString(typeid(T).name()) + "not implemented)";
}

inline String toLowerCase(const String& str)
{
	String result;
	for (const Char c : str)
	{
		result += tolower(c);
	}
	return result;
}

inline Double toDouble(const Float a)
{
	return a;
}

inline Double toDouble(const Int a)
{
	return a;
}

inline Double toDouble(const ULL a)
{
	return static_cast<Double>(a);
}

inline Double toDouble(const Double a)
{
	return a;
}

template <typename T>
Double toDouble(T v)
{
	__debugbreak();
	return 0.0;
}

inline String thisThread()
{
	return "Thread (" + toString(std::hash<std::thread::id>{}(std::this_thread::get_id())) + ")";
}
