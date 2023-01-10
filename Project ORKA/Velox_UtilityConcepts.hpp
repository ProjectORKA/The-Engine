
#pragma once

//written by Omarito
//modified by
//  Blendurian

#include "Velox_Common.hpp"
#include <type_traits>

TRE_NS_START

template<typename T>
concept SwappableConcept = requires(T& t1, T& t2) {
    { swap(t1, t2) };
};

template<typename T>
concept MoveableConcept = std::is_move_assignable_v<T> && std::is_move_constructible_v<T>;

template<typename T>
concept CopyableConcept = std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T>;

template<typename C>
concept ContainerConcept = requires(C& c)
{
    c.size() -> usize; // -> usize;
    c.data();
};

template<typename T>
concept POD = (std::is_standard_layout_v<T> && std::is_trivial_v<T>) || (std::is_trivially_copyable_v<T> && std::is_trivially_destructible_v<T>);

TRE_NS_END
