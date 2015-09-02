extern "C" {
	#include "../common.h"
	#include "../util/util.h"
	#include "localisation.h"
}

#include "../core/FileStream.hpp"
#include "../core/Memory.hpp"
#include "../core/StringBuilder.hpp"
#include "LanguagePack.h"
#include <SDL.h>

LanguagePack *LanguagePack::FromFile(int id, const utf8 *path)
{
	assert(path != NULL);

	uint32 fileLength;
	utf8 *fileData;

	// Load file directly into memory
	try {
		FileStream fs = FileStream(path, FILE_MODE_OPEN);
		
		fileLength = (uint32)fs.GetLength();
		fileData = Memory::Allocate<utf8>(fileLength);
		fs.Read(fileData, fileLength);

		fs.Dispose();
	} catch (Exception ex) {
		log_error("Unable to open %s: %s", path, ex.GetMessage());
		return NULL;
	}

	// Parse the memory as text
	LanguagePack *result = FromText(id, fileData);

	Memory::Free(fileData);
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
