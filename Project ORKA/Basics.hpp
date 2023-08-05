// ReSharper disable CppUnusedIncludeDirective

#pragma once

#pragma warning(disable : 4996) // disables unsecure warning

//#define _CSTD std::

#include <set>
#include <map>
#include <list>
#include "math.h"
//#include <cmath>
#include <deque>
#include <ctime>
#include <string>
#include <bitset>
#include <thread>
#include <memory>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <iostream>
#include <filesystem>
#include <functional>
#include <shared_mutex>

//#include <sstream>
#ifdef MEMORY_LEAK_DETECTION
	#include <crtdbg.h>
#endif // MEMORY_LEAK_DETECTION

// standard
using Bool = bool;

// signed integers
using Char = char;
using Short = short;
using Int = int;
using Long = long;
using LL = long long;

// unsigned integers
using Byte = unsigned char;
using UShort = unsigned short;
using Index = unsigned int;
using UInt = unsigned int;
using ULong = unsigned long;
using ULL = unsigned long long;
using SizeT = size_t;

// floating point
using Float = float;
using Double = double;
using LDouble = long double;

using U16 = std::uint16_t;
using U8 = std::uint8_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;
using I16 = std::int16_t;
using I8 = std::int8_t;
using I32 = std::int32_t;
using I64 = std::int64_t;

// pointers
template <typename T> using WeakPointer = std::weak_ptr<T>;
template <typename T> using UniquePointer = std::unique_ptr<T>;
template <typename T> using SharedPointer = std::shared_ptr<T>;

using String = std::string;

#include "Name.hpp"
#include "Vectors.hpp"

using Color = glm::vec4;

using Sphere = Vec4;

#include "glm/glm.hpp"
using Matrix = glm::mat4;

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/projection.hpp"

using Mutex = std::mutex;
using Rotation = glm::quat;
using Thread = std::thread;
using Quaternion = glm::quat;
using Path = std::filesystem::path;
using SharedMutex = std::shared_mutex;
template <typename T> using Function = std::function<T>;
template <typename T> using LockGuard = std::lock_guard<T>;

inline String threadId()
{
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	return oss.str();
}

template <typename T> using Set = std::set<T>;
template <typename T> using List = std::list<T>;
template <typename T> using Deque = std::deque<T>;
template <typename T> using Vector = std::vector<T>;
template <std::size_t T> using BitSet = std::bitset<T>;
template <typename T, typename K> using Map = std::map<T, K>;
template <typename T, size_t size> using Array = std::array<T, size>;

inline Float toFloat(const Byte v)
{
	return v;
}

inline Int toIntSafe(const ULL size)
{
	if(size < std::numeric_limits<Int>::max()) return static_cast<Int>(size);
	__debugbreak();
	return std::numeric_limits<Int>::max();
}

inline UInt toUIntSafe(const ULL size)
{
	// check if bigger than max value
	if(size < std::numeric_limits<UInt>::max()) return static_cast<UInt>(size);
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
	return "(" + BitSet<sizeof(ULL) * 8>(v).to_string() + ") = " + std::to_string(v);
}

inline String toString(const Bool v)
{
	if(v) return "true";
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
	return v.string();
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

// vectors

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

inline String toString(const ULLVec2 v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + ")";
}

inline String toString(const ULLVec3& v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + ")";
}

inline String toString(const ULLVec4& v)
{
	return "(" + toString(v.x) + "|" + toString(v.y) + "|" + toString(v.z) + "|" + toString(v.w) + ")";
}

inline String toString(const Matrix& v)
{
	return "\n" + toString(v[0]) + "\n" + toString(v[1]) + "\n" + toString(v[2]) + "\n" + toString(v[3]) + "\n";
}

template <typename T> String toString(T v)
{
	__debugbreak(); // implement conversion
	return "ERROR";
}

// threading

inline String thisThread()
{
	return "Thread (" + toString(std::hash<std::thread::id>{}(std::this_thread::get_id())) + ")";
}
