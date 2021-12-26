#pragma once


//standard
using Bool = bool;
using Boolean = bool;

using Int = int;
using Integer = int;
using LL = long long;
using Index = unsigned int;
using UInt = unsigned int;
using ULong = unsigned long;
using ULL = unsigned long long;
using ULLInt = unsigned long long;
using Float = float;

using Double = double;
using LDouble = long double;

using Short = short;
using UShort = unsigned short;

using Char = char;
using Byte = unsigned char;

using SizeT = unsigned __int64;//std::size_t;

//pointers
#include <memory>
template <typename T>
using UniquePointer = std::unique_ptr<T>;

template<typename T>
using SharedPointer = std::shared_ptr<T>;

template<typename T>
using WeakPointer = std::weak_ptr<T>;

#include "String.hpp"

#define NAME_SIZE 64

struct Name {
	char data[NAME_SIZE] = {};

	Name();
	Name(const char* name);
	Name& operator=(const char* other);
	Name& operator=(std::string other);
	bool operator==(const Name& rhs);
};

bool operator<(const Name& l, const Name& r);

//glm
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "glm/gtc/matrix_transform.hpp"

using IVec2 = glm::ivec2;
using IVec3 = glm::ivec3;
using Vec2 = glm::vec2;
using DVec2 = glm::dvec2;
using DVec3 = glm::dvec3;

using Vec3 = glm::vec3;
using Location = glm::vec3;

using Vec4 = glm::vec4;
using Color = glm::vec4;

using ULLVec2 = glm::u64vec2;
using ULLVec3 = glm::u64vec3;

using Matrix = glm::mat4;
using Quat = glm::quat;
using Quaternion = glm::quat;

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

using Rotation = glm::quat;

//input output
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
using Path = std::filesystem::path;

#include <shared_mutex>
using Mutex = std::shared_mutex;

//containers
#include <list>
template<typename T>
using List = std::list<T>;

#include <vector>
template<typename T>
using Vector = std::vector<T>;

#include <set>
template<typename T>
using Set = std::set<T>;

#include <map>
template<typename T, typename K>
using Map = std::map<T, K>;

#include <bitset>
template <std::size_t T>
using BitSet = std::bitset<T>;

//#include <queue>
//template<typename T>
//using Queue = std::queue<T>;

#include <deque>
template<typename T>
using Deque = std::deque<T>;