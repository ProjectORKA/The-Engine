// ReSharper disable CppUnusedIncludeDirective
#pragma once
#pragma warning(disable : 4996) // disables unsecure warning

#define GLM_ENABLE_EXPERIMENTAL

#include "Settings.hpp"

#include <set>
#include <map>
#include <list>
#include <array>
#include <cmath>
#include <ctime>
#include <deque>
#include <bitset>
#include <limits>
#include <math.h>
#include <chrono>
#include <future>
#include <memory>
#include <ranges>
#include <string>
#include <thread>
#include <vector>
#include <codecvt>
#include <float.h>
#include <fstream>
#include <iomanip>
#include <utility>
#include <iostream>
#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <exception>
#include <algorithm>
#include <functional>
#include <filesystem>
#include <shared_mutex>
#include <unordered_set>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/rotate_vector.hpp"

// standard
using Bool = bool;

// signed integers
using Char  = char;
using Short = short;
using Int   = int;
using Long  = long;
using LL    = long long;

// unsigned integers
using Byte   = unsigned char;
using UShort = unsigned short;
using Index  = unsigned int;
using UInt   = unsigned int;
using ULong  = unsigned long;
using ULL    = unsigned long long;
using SizeT  = size_t;

// floating point
using Float   = float;
using Double  = double;
using LDouble = long double;

using U16 = uint16_t;
using U8  = uint8_t;
using U32 = uint32_t;
using U64 = uint64_t;
using I16 = int16_t;
using I8  = int8_t;
using I32 = int32_t;
using I64 = int64_t;

//max values
constexpr UInt maxUInt = 4294967295U;
constexpr ULL  maxULL  = 18446744073709551615Ui64;

// pointers
template <typename T>
using WeakPointer = std::weak_ptr<T>;
template <typename T>
using UniquePointer = std::unique_ptr<T>;
template <typename T>
using SharedPointer = std::shared_ptr<T>;

using String  = std::string;
using WString = std::wstring;

using Exception = std::exception;
using ErrorCode = std::error_code;

constexpr unsigned short nameSize = 64;

struct Name
{
	Name();
	Name(const char* name);
	Name(const String& name);
	Name&                operator=(const char* other);
	Name&                operator=(const String& other);
	[[nodiscard]] String toString() const;
	[[nodiscard]] Bool   operator<(const Name& other) const;
	[[nodiscard]] Bool   operator==(const Name& other) const;
	[[nodiscard]] Bool   operator!=(const Name& other) const;

private:
	char data[nameSize] = {};
};

inline String toString(const Name& name)
{
	return name.toString();
}

using IVec2 = glm::ivec2;
using IVec3 = glm::ivec3;
using IVec4 = glm::ivec4;

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;

using DVec2 = glm::dvec2;
using DVec3 = glm::dvec3;

using UllVec2 = glm::u64vec2;
using UllVec3 = glm::u64vec3;
using UllVec4 = glm::u64vec4;

using Color = glm::vec4;

constexpr UInt colorToRGBHex(const Color color)
{
	const auto red   = static_cast<Byte>(color.r * 255);
	const auto green = static_cast<Byte>(color.g * 255);
	const auto blue  = static_cast<Byte>(color.b * 255);

	return red << 16 | green << 8 | blue;
}

constexpr UInt colorToRGBAHex(const Color color)
{
	const auto red   = static_cast<Byte>(color.r * 255);
	const auto green = static_cast<Byte>(color.g * 255);
	const auto blue  = static_cast<Byte>(color.b * 255);
	const auto alpha = static_cast<Byte>(color.a * 255);

	return red << 24 | green << 16 | blue << 8 | alpha;
}

using Sphere = Vec4;

using Matrix = glm::mat4;

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/projection.hpp>

using Mutex       = std::mutex;
using Rotation    = glm::quat;
using Thread      = std::thread;
using Quaternion  = glm::quat;
using Path        = std::filesystem::path;
using SharedMutex = std::shared_mutex;
template <typename T>
using Atomic = std::atomic<T>;
template <typename T>
using Function = std::function<T>;
template <typename T>
using LockGuard = std::lock_guard<T>;
template <typename T>
using SharedLockGuard = std::shared_lock<T>;

inline String threadId()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	return oss.str();
}

//template <typename T> using Set = std::set<T>;
template <typename T>
using List = std::list<T>;
template <typename T>
using Deque = std::deque<T>;
template <typename T>
using Vector = std::vector<T>;
template <std::size_t T>
using BitSet = std::bitset<T>;
template <typename T, typename K>
using Map = std::map<T, K>;
template <typename T, size_t size>
using Array = std::array<T, size>;

struct Vec2Vector;
struct Vec3Vector;

struct Vec2Vector : std::vector<Vec2>
{
	explicit Vec2Vector(const Vec3Vector& other);
	explicit Vec2Vector(const std::size_t size) : std::vector<Vec2>(size) {}
	explicit Vec2Vector(const std::vector<Vec2>& other) : std::vector<Vec2>(other) {}

	Vec2Vector() = default;
	Vec2Vector(const std::size_t size, const Vec2& value) : std::vector<Vec2>(size, value) {}
	Vec2Vector(const std::initializer_list<Vec2> initList) : std::vector<Vec2>(initList) {}

	[[nodiscard]] Vec3Vector toVec3() const;
};

struct Vec3Vector : Vector<Vec3>
{
	Vec3Vector() = default;
	[[nodiscard]] Vec2Vector toVec2() const;
	explicit                 Vec3Vector(const Vec2Vector& other);
	explicit                 Vec3Vector(const SizeT size) : Vector<Vec3>(size) {}
	Vec3Vector(const SizeT size, const Vec3& value) : Vector<Vec3>(size, value) {}
	Vec3Vector(const std::initializer_list<Vec3> initList) : Vector<Vec3>(initList) {}
};

inline Float toFloat(const Byte v)
{
	return v;
}

inline Int toIntSafe(const ULL size)
{
	if (size < std::numeric_limits<Int>::max()) return static_cast<Int>(size);
	__debugbreak();
	return std::numeric_limits<Int>::max();
}

inline UInt toUIntSafe(const ULL size)
{
	// check if bigger than max value
	if (size < std::numeric_limits<UInt>::max()) return static_cast<UInt>(size);
	__debugbreak();
	return std::numeric_limits<UInt>::max();
}

// to String functions

inline String toString(const LL v)
{
	return std::to_string(v);
}

inline String toString(const Int v)
{
	return std::to_string(v);
}

inline String toString(const ULL v)
{
	if (printULLWithBits)
	{
		return "(" + BitSet<sizeof(ULL) * 8>(v).to_string() + ") = " + std::to_string(v);
	}
	else
	{
		return std::to_string(v);
	}
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
	// there seems to be an issue with, for example japanese characters, that make this conversion necessary
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

inline String toString(const String& v) // lol
{
	return v;
}

// vectors to string

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
	__debugbreak(); // implement conversion
	return "(ERROR: String conversion not implemented)";
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

// to Double functions

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
	__debugbreak(); // implement conversion
	return 0.0;
}

// threading

inline String thisThread()
{
	return "Thread (" + toString(std::hash<std::thread::id>{}(std::this_thread::get_id())) + ")";
}
