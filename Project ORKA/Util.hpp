
#include <type_traits>

//stackoverflow.com/questions/11421432/how-can-i-output-the-value-of-an-enum-class-in-c11/26809130
template <typename T>
auto enumClassAsInt(T const value)-> typename std::underlying_type<T>::type
{
    return static_cast<typename std::underlying_type<T>::type>(value);
}