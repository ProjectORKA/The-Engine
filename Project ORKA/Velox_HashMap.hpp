
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include <math.h>
#include "Velox_Common.hpp"
#include "Velox_Memory.hpp"
#include "Velox_Utils.hpp"
#include "Velox_UtilityConcepts.hpp"
#include "Velox_HashMapHelper.hpp"

TRE_NS_START

template<typename K, typename V>
struct HashMap : public hashMapHelper<K, V, std::hash<K>, FibonacciHashPolicy, std::equal_to<K>>
{
    using size_type = size_t;
    using pointer = value_type*;
    using reference = value_type&;
    using value_type = std::pair<K*, V*>;
    using difference_type = std::ptrdiff_t;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;
};

TRE_NS_END
