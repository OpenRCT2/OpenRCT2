#pragma once

#include "../common.h"
#include "Exception.hpp"
#include "IDisposable.hpp"

enum {
	STREAM_SEEK_BEGIN,
	STREAM_SEEK_CURRENT,
	STREAM_SEEK_END
};

/**
 * Represents a stream that can be read or written to. Implemented by types such as FileStream, NetworkStream or MemoryStream.
 */
interface IStream : public IDisposable {
	///////////////////////////////////////////////////////////////////////////
	// Interface methods
	///////////////////////////////////////////////////////////////////////////
	// virtual ~IStream()												abstract;

	virtual bool	CanRead()								const	abstract;
	virtual bool	CanWrite()								const	abstract;

	virtual sint64	GetLength()								const	abstract;
	virtual sint64	GetPosition()							const	abstract;
	virtual void	SetPosition(sint64 position)					abstract;
	virtual void	Seek(sint64 offset, int origin)					abstract;

	virtual void	Read(void *buffer, int length)					abstract;
	virtual void	Write(const void *buffer, int length)			abstract;

	///////////////////////////////////////////////////////////////////////////
	// Helper methods
	///////////////////////////////////////////////////////////////////////////

	/**
	 * Reads the size of the given type from the stream directly into the given address.
	 */
	template<typename T>
	void Read(T *value) {
		Read(value, sizeof(T));
	}

	/**
	 * Writes the size of the given type to the stream directly from the given address.
	 */
	template<typename T>
	void Write(const T *value) {
		Write(value, sizeof(T));
	}

	/**
	 * Reads the given type from the stream. Use this only for small types (e.g. sint8, sint64, double)
	 */
	template<typename T>
	T ReadValue() {
		T buffer;
		Read(&buffer);
		return buffer;
	}

	/**
	 * Writes the given type to the stream. Use this only for small types (e.g. sint8, sint64, double)
	 */
	template<typename T>
	void WriteValue(const T value) {
		Write(&value);
	}
};

class IOException : public Exception {
public:
	IOException(const char *message) : Exception(message) { }
};
