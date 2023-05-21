#pragma once

#pragma warning(disable : 4996) //disables unsecure warning

#include <stdint.h>
#include <memory>
#include <string>
#include <filesystem>
#include <shared_mutex>
#include <list>
#include <set>
#include <bitset>
#include <deque>
#include <map>
#include <thread>
//#include <iostream>
//#include <sstream>

//standard
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
using SizeT = std::size_t;

// floating point
using Float = float;
using Double = double;
using LDouble = long double;

using U16 = uint16_t;
using U8 = uint8_t;
using U32 = uint32_t;
using U64 = uint64_t;
using I16 = int16_t;
using I8 = int8_t;
using I32 = int32_t;
using I64 = int64_t;

//pointers
template<typename T>
using UniquePointer = std::unique_ptr<T>;
template<typename T>
using SharedPointer = std::shared_ptr<T>;
template<typename T>
using WeakPointer = std::weak_ptr<T>;

using String = std::string;

// ReSharper disable once CppUnusedIncludeDirective
#include "Name.hpp"

#include "Vectors.hpp"

using Color = glm::vec4;

using Sphere = Vec4;

#include "glm/glm.hpp"
using Matrix = glm::mat4;

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "glm/gtx/euler_angles.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "glm/gtx/projection.hpp"

using Quaternion = glm::quat;
using Rotation = glm::quat;

using Path = std::filesystem::path;

using Thread = std::thread;

using Mutex = std::shared_mutex;

template<typename T>
using List = std::list<T>;

template<typename T>
using Vector = std::vector<T>;

template<typename T, size_t size>
using Array = std::array<T,size>;

template<typename T>
using Set = std::set<T>;

template<typename T, typename K>
using Map = std::map<T, K>;

template<std::size_t T>
using BitSet = std::bitset<T>;

template<typename T>
using Deque = std::deque<T>;

inline Int toIntSafe(const ULL size)
{
	if (size < std::numeric_limits<Int>::max())	return static_cast<Int>(size);
	__debugbreak();
	return std::numeric_limits<Int>::max();
};

inline UInt toUIntSafe(const ULL size)
{
	//check if bigger than max value
	if(size < std::numeric_limits<UInt>::max()) return static_cast<UInt>(size);
	__debugbreak();
	return std::numeric_limits<UInt>::max();
};

inline Float toFloat(const Byte v)
{
	return v;
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

inline String toString(const UShort v)
{
	return std::to_string(v);
}

inline String toString(const UInt v)
{
	return std::to_string(v);
}

inline String toString(const ULong v)
{
	return std::to_string(v);
}

inline String toString(const ULL v)
{
	return std::to_string(v);
}

inline String toString(const Char v)
{
	return std::to_string(v);
}

inline String toString(const Short v)
{
	return std::to_string(v);
}

inline String toString(const Int v)
{
	return std::to_string(v);
}

inline String toString(const Long v)
{
	return std::to_string(v);
}

inline String toString(const LL v)
{
	return std::to_string(v);
}

inline String toString(const Float v)
{
	return std::to_string(v);
}

inline String toString(const Double v)
{
	return std::to_string(v);
}

inline String toString(const LDouble v)
{
	return std::to_string(v);
}

inline String toString(const IVec2 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append(")");
	return s;
}

inline String toString(const IVec3 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append(")");
	return s;
}

inline String toString(const IVec4 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append("|");
	s.append(toString(v.w));
	s.append(")");
	return s;
}

inline String toString(const Vec2 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append(")");
	return s;
}

inline String toString(const Vec3 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append(")");
	return s;
}

inline String toString(const Vec4 v)
{
	String s = "(";
	s.append(toString(v.x));
	s.append("|");
	s.append(toString(v.y));
	s.append("|");
	s.append(toString(v.z));
	s.append("|");
	s.append(toString(v.w));
	s.append(")");
	return s;
}