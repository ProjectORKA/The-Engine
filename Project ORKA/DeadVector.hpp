#pragma once


template<typename T>
struct DeadVector{
	Vector<Bool> exists;
	Vector<T> data;
}
