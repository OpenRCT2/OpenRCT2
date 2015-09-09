#pragma once

/**
 * Utility methods for memory management. Typically helpers and wrappers around the C standard library.
 */
namespace Memory {
	template<typename T>
	T *Allocate() {
		return (T*)malloc(sizeof(T));
	}

	template<typename T>
	T *Allocate(size_t size) {
		return (T*)malloc(size);
	}

	template<typename T>
	T *AllocateArray(size_t count) {
		return (T*)malloc(count * sizeof(T));
	}

	template<typename T>
	T *Reallocate(T *ptr, size_t size) {
		if (ptr == NULL)
			return (T*)malloc(size);
		else
			return (T*)realloc((void*)ptr, size);
	}

	template<typename T>
	T *ReallocateArray(T *ptr, size_t count) {
		if (ptr == NULL)
			return (T*)malloc(count * sizeof(T));
		else
			return (T*)realloc((void*)ptr, count * sizeof(T));
	}

	template<typename T>
	void Free(T *ptr) {
		free((void*)ptr);
	}

	template<typename T>
	T *Copy(T *dst, const T *src, size_t size) {
		return (T*)memcpy((void*)dst, (const void*)src, size);
	}

	template<typename T>
	T *CopyArray(T *dst, const T *src, size_t count) {
		return (T*)memcpy((void*)dst, (const void*)src, count * sizeof(T));
	}

	template<typename T>
	T *Duplicate(const T *src, size_t size) {
		T *result = Allocate<T>(size);
		return Copy(result, src, size);
	}

	template<typename T>
	T *DuplicateArray(const T *src, size_t count) {
		T *result = AllocateArray<T>(count);
		return CopyArray(result, src, count);
	}
}
