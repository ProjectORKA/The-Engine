#pragma once

template <typename T>
struct Override {
	// override temorarily overrides a value for the duration of its existence
	// multiple overrides can work on the same value as long as they are destroyed in order of creation
	T* sourcePtr = nullptr;
	T stored;

	Override(T& source, T value) {
		sourcePtr = &source;
		stored = source;
		*sourcePtr = value;
	};
	~Override() { *source = stored; }
};
