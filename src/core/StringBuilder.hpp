#pragma once

#include "../common.h"
#include "../localisation/localisation.h"
#include "Math.hpp"
#include "Memory.hpp"

/**
 * Class for constructing strings efficiently. A buffer is automatically allocated and reallocated when characters or strings
 * are appended. Use GetString to copy the current state of the string builder to a new fire-and-forget string.
 */
class StringBuilder final {
public:
	StringBuilder() {
		_buffer = NULL;
		_capacity = 0;
		_length = 0;
	}

	StringBuilder(int capacity) : StringBuilder() {
		EnsureCapacity(capacity);
	}

	~StringBuilder() {
		if (_buffer != NULL) Memory::Free(_buffer);
	}

	/**
	 * Appends the given character to the current string.
	 */
	void Append(int codepoint) {
		int codepointLength = utf8_get_codepoint_length(codepoint);
		EnsureCapacity(_length + codepointLength + 1);
		utf8_write_codepoint(_buffer + _length, codepoint);
		_length += codepointLength;
		_buffer[_length] = 0;
	}

	/**
	 * Appends the given string to the current string.
	 */
	void Append(const utf8 *text) {
		int textLength = strlen(text);

		EnsureCapacity(_length + textLength + 1);
		Memory::Copy(_buffer + _length, text, textLength);
		_length += textLength;
		_buffer[_length] = 0;
	}

	/**
	 * Clears the current string, but preserves the allocated memory for another string.
	 */
	void Clear() {
		_length = 0;
		if (_capacity >= 1) {
			_buffer[_length] = 0;
		}
	}

	/**
	 * Like Clear, only will guarantee freeing of the underlying buffer.
	 */
	void Reset() {
		_length = 0;
		_capacity = 0;
		if (_buffer != NULL) {
			Memory::Free(_buffer);
		}
	}

	/**
	 * Returns the current string buffer as a new fire-and-forget string.
	 */
	utf8 *GetString() const {
		utf8 *result = Memory::AllocateArray<utf8>(_length + 1);
		Memory::CopyArray(result, _buffer, _length);
		result[_length] = 0;
		return result;
	}

	/**
	 * Gets the current state of the StringBuilder. Warning: this represents the StringBuilder's current working buffer and will
	 * be deallocated when the StringBuilder is destructed.
	 */
	const utf8 *GetBuffer() const {
		return _buffer;
	}

	/**
	 * Gets the amount of allocated memory for the string buffer.
	 */
	size_t GetCapacity() const { return _capacity; }

	/**
	 * Gets the length of the current string.
	 */
	size_t GetLength() const { return _length; }

private:
	utf8 *_buffer;
	size_t _capacity;
	size_t _length;

	void EnsureCapacity(size_t capacity)
	{
		if (_capacity > capacity) return;

		_capacity = Math::Max(8U, _capacity);
		while (_capacity < capacity) {
			_capacity *= 2;
		}

		_buffer = Memory::ReallocateArray<utf8>(_buffer, _capacity);
	}
};
