#pragma once

#include "OrkaStdIncludes.hpp"

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

// max values
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

template <typename T>
using List = std::list<T>;
template <typename T>
using Deque = std::deque<T>;
template <typename T>
using Vector = std::vector<T>;
template <SizeT T>
using BitSet = std::bitset<T>;
template <typename T, typename K>
using Map = std::map<T, K>;
template <typename T, SizeT S>
using Array = std::array<T, S>;

struct Vec2Vector;
struct Vec3Vector;

struct Vec2Vector : Vector<Vec2>
{
	explicit Vec2Vector(const Vec3Vector& other);
	explicit Vec2Vector(const SizeT size) : Vector<Vec2>(size) {}
	explicit Vec2Vector(const Vector<Vec2>& other) : Vector<Vec2>(other) {}

	Vec2Vector() = default;
	Vec2Vector(const SizeT size, const Vec2& value) : Vector<Vec2>(size, value) {}
	Vec2Vector(const std::initializer_list<Vec2> initList) : Vector<Vec2>(initList) {}

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
	if (size < std::numeric_limits<UInt>::max()) return static_cast<UInt>(size);
	__debugbreak();
	return std::numeric_limits<UInt>::max();
}
