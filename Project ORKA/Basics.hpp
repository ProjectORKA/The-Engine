#pragma once

//standard
using Bool = bool;
using Boolean = bool;

using Int = int;
using Integer = int;
using LL = long long;
using Index = unsigned int;
using UInt = unsigned int;
using ULL = unsigned long long;
using ULLInt = unsigned long long;

using Float = float;

using Double = double;

using Short = short;
using UShort = unsigned short;

using Char = char;
using Byte = unsigned char;

#include <string>
using String = std::string;

//glm
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
using Vec2 = glm::vec2;
using DVec2 = glm::dvec2;

using Vec3 = glm::vec3;
using Location = glm::vec3;

using Vec4 = glm::vec4;
using Color = glm::vec4;

using ULLVec3 = glm::u64vec3;

using Matrix = glm::mat4;

//input output
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
using Path = std::filesystem::path;

//tread
#include <thread>
using Thread = std::thread;

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