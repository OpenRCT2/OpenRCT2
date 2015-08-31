extern "C" {
	#include "../common.h"
	#include "../util/util.h"
	#include "localisation.h"
}

#include "LanguagePack.h"

#include <SDL.h>

// TODO Move to separate file
class StringBuilder final {
public:
	StringBuilder()
	{
		_buffer = NULL;
		_capacity = 0;
		_length = 0;
	}

	StringBuilder(int capacity) : StringBuilder()
	{
		EnsureCapacity(capacity);
	}

	~StringBuilder()
	{
		if (_buffer != NULL) free(_buffer);
	}

	utf8 *GetString() const
	{
		utf8 *result = (utf8*)malloc(_length + 1);
		memcpy(result, _buffer, _length);
		result[_length] = 0;
		return result;
	}

	void Append(int codepoint)
	{
		int codepointLength = utf8_get_codepoint_length(codepoint);
		EnsureCapacity(_length + codepointLength + 1);
		utf8_write_codepoint(_buffer + _length, codepoint);
		_length += codepointLength;
	}

	void Append(utf8 *text)
	{
		int textLength = strlen(text);

		EnsureCapacity(_length + textLength + 1);
		memcpy(_buffer + _length, text, textLength);
		_length += textLength;
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

		if (_buffer == NULL) {
			_buffer = (utf8*)malloc(_capacity);
		} else {
			_buffer = (utf8*)realloc(_buffer, _capacity);
		}
	}
};

LanguagePack *LanguagePack::FromFile(int id, const utf8 *path)
{
	assert(path != NULL);

	int fileLength;
	utf8 *fileData;

	// Load file directly into memory
	SDL_RWops *file = SDL_RWFromFile(path, "rb");
	if (file == NULL) {
		log_error("Unable to open %s", path);
		return NULL;
	}

	fileLength = (int)SDL_RWsize(file);
	fileData = (utf8*)malloc(fileLength);
	SDL_RWread(file, fileData, fileLength, 1);
	SDL_RWclose(file);

	// Parse the memory as text
	LanguagePack *result = FromText(id, fileData);
	free(fileData);

	return result;
}

LanguagePack *LanguagePack::FromText(int id, const utf8 *text)
{	
	return new LanguagePack(id, text);
}

LanguagePack::LanguagePack(int id, const utf8 *text)
{
	assert(text != NULL);

	_id = id;
	_stringData = NULL;
	_currentGroup = NULL;

	auto reader = UTF8StringReader(text);

}

LanguagePack::~LanguagePack()
{
	SafeFree(_stringData);
	SafeFree(_currentGroup);
}

static void SkipWhitespace(IStringReader *reader)
{
	int codepoint;
	while (reader->TryPeek(&codepoint)) {
		if (codepoint == '\t' || codepoint == ' ' || codepoint == '\r' || codepoint == '\n') {
			reader->Skip();
		} else {
			break;
		}
	}
}

static void SkipToEndOfLine(IStringReader *reader)
{
	int codepoint;
	while (reader->TryPeek(&codepoint)) {
		if (codepoint != '\r' && codepoint != '\n') {
			reader->Skip();
		} else {
			break;
		}
	}
}

void LanguagePack::ParseLine(IStringReader *reader)
{
	SkipWhitespace(reader);

	int codepoint;
	if (!reader->TryPeek(&codepoint))
		return;

	switch (codepoint) {
	case '#':
		SkipToEndOfLine(reader);
		break;
	case '[':
		ParseGroup(reader);
		break;
	default:
		ParseString(reader);
		break;
	}
}

void LanguagePack::ParseGroup(IStringReader *reader)
{
	auto sb = StringBuilder();
	int codepoint;

	// Should have already deduced that the next codepoint is a [
	reader->Skip();

	// Read string up to ] or line end
	bool closedCorrectly = false;
	while (reader->TryRead(&codepoint)) {
		if (codepoint == '\n' || codepoint == '\r') break;
		if (codepoint == ']') {
			closedCorrectly = true;
			break;
		}
		sb.Append(codepoint);
	}

	if (closedCorrectly) {
		SafeFree(_currentGroup);
		_currentGroup = sb.GetString();
	}
}

void LanguagePack::ParseString(IStringReader *reader)
{

}
