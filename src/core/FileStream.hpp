#pragma once

#include <SDL.h>
#include "../common.h"
#include "IStream.hpp"

enum {
	FILE_MODE_OPEN,
	FILE_MODE_WRITE
};

/**
 * A stream for reading and writing to files. Wraps an SDL_RWops, SDL2's cross platform file stream.
 */
class FileStream : public IStream {
private:
	SDL_RWops *_file;
	bool _canRead;
	bool _canWrite;
	bool _disposed;
		
public:
	FileStream(const utf8 *path, int fileMode) {
		const char *mode;
		switch (fileMode) {
		case FILE_MODE_OPEN:
			mode = "rb";
			_canRead = true;
			_canWrite = false;
			break;
		case FILE_MODE_WRITE:
			mode = "wb";
			_canRead = false;
			_canWrite = true;
			break;
		default:
			throw;
		}

		_file = SDL_RWFromFile(path, mode);
		if (_file == NULL) {
			throw IOException(SDL_GetError());
		}

		_disposed = false;
	}

	~FileStream() {
		Dispose();
	}

	void Dispose() override {
		if (!_disposed) {
			_disposed = true;
			SDL_RWclose(_file);
		}
	}

	bool CanRead() const override { return _canRead; }
	bool CanWrite() const override { return _canWrite; }

	sint64 GetLength() const override { return SDL_RWsize(_file); }
	sint64 GetPosition() const override { return SDL_RWtell(_file); }

	void SetPosition(sint64 position) override {
		Seek(position, STREAM_SEEK_BEGIN);
	}

	void Seek(sint64 offset, int origin) override {
		switch (origin) {
		case STREAM_SEEK_BEGIN:
			SDL_RWseek(_file, offset, RW_SEEK_SET);
			break;
		case STREAM_SEEK_CURRENT:
			SDL_RWseek(_file, offset, RW_SEEK_CUR);
			break;
		case STREAM_SEEK_END:
			SDL_RWseek(_file, offset, RW_SEEK_END);
			break;
		}
	}

	void Read(void *buffer, int length) override {
		if (SDL_RWread(_file, buffer, length, 1) != 1) {
			throw IOException("Attempted to read past end of file.");
		}
	}

	void Write(const void *buffer, int length) override {
		if (SDL_RWwrite(_file, buffer, length, 1) != 1) {
			throw IOException("Unable to write to file.");
		}
	}
};
