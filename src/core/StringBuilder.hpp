#pragma once

#include "../common.h"
#include "../localisation/localisation.h"
#include "Memory.hpp"

/**
 * Class for constructing strings efficiently. A buffer is automatically allocated and reallocated when characters or strings
 * are appended. Use GetString to copy the current state of the string builder to a new fire and forget string.
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

	utf8 *GetString() const {
		utf8 *result = Memory::AllocateArray<utf8>(_length + 1);
		Memory::CopyArray(result, _buffer, _length);
		result[_length] = 0;
		return result;
	}

	void Append(int codepoint) {
		int codepointLength = utf8_get_codepoint_length(codepoint);
		EnsureCapacity(_length + codepointLength + 1);
		utf8_write_codepoint(_buffer + _length, codepoint);
		_length += codepointLength;
		_buffer[_length] = 0;
	}

	void Append(utf8 *text) {
		int textLength = strlen(text);

		EnsureCapacity(_length + textLength + 1);
		Memory::Copy(_buffer + _length, text, textLength);
		_length += textLength;
		_buffer[_length] = 0;
	}

	void Clear() {
		_length = 0;
		if (_capacity >= 1) {
			_buffer[_length] = 0;
		}
	}

	/**
	 * Gets the current state of the StringBuilder. Warning: this represents the StringBuilder's current working buffer and will
	 * be deallocated when the StringBuilder is destructed.
	 */
	const utf8 *GetBuffer() {
		return _buffer;
	}

private:
	utf8 *_buffer;
	size_t _capacity;
	size_t _length;

	void EnsureCapacity(size_t capacity)
	{
		if (_capacity > capacity) return;

		_capacity = max(8, _capacity);
		while (_capacity < capacity) {
			_capacity *= 2;
		}

		_buffer = Memory::ReallocateArray<utf8>(_buffer, _capacity);
	}
};
