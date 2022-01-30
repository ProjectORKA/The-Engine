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
using UChar = unsigned char;

using SizeT = unsigned __int64;//std::size_t;

//pointers
#include <memory>
template <typename T>
using UniquePointer = std::unique_ptr<T>;

template<typename T>
using SharedPointer = std::shared_ptr<T>;

template<typename T>
using WeakPointer = std::weak_ptr<T>;

#include "String.hpp";

#include "Name.hpp"

#include "Vectors.hpp"

#include "Color.hpp"

using Sphere = Vec4;

using Matrix = glm::mat4;
using Quat = glm::quat;
using Quaternion = glm::quat;

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

using Rotation = glm::quat;

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

#include "Map.hpp"

#include <bitset>
template <std::size_t T>
using BitSet = std::bitset<T>;

//#include <queue>
//template<typename T>
//using Queue = std::queue<T>;

#include <deque>
template<typename T>
using Deque = std::deque<T>;