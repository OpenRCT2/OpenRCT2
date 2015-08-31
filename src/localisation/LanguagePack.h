#pragma once

#include <vector>

extern "C" {
	#include "../common.h"
	#include "../util/util.h"
	#include "localisation.h"
}

struct IStringReader abstract {
	virtual bool TryPeek(int *outCodepoint) abstract;
	virtual bool TryRead(int *outCodepoint) abstract;
	virtual void Skip() abstract;
};

// TODO Move to separate file in Core
class UTF8StringReader final : public IStringReader {
public:
	UTF8StringReader(const utf8 *text)
	{
		// Skip UTF-8 byte order mark
		if (strlen(text) >= 3 && utf8_is_bom(text)) {
			text += 3;
		}

		_text = text;
		_current = text;
	}

	bool TryPeek(int *outCodepoint) override
	{
		if (_current == NULL) return false;

		int codepoint = utf8_get_next(_current, NULL);
		*outCodepoint = codepoint;
		return true;
	}

	bool TryRead(int *outCodepoint) override
	{
		if (_current == NULL) return false;

		int codepoint = utf8_get_next(_current, &_current);
		*outCodepoint = codepoint;
		if (codepoint == 0) {
			_current = NULL;
			return false;
		}
		return true;
	}

	void Skip() override
	{
		int codepoint;
		TryRead(&codepoint);
	}

private:
	const utf8 *_text;
	const utf8 *_current;
};

class LanguagePack final {
public:
	static LanguagePack *FromFile(int id, const utf8 *path);
	static LanguagePack *FromText(int id, const utf8 *text);

	~LanguagePack();

	int GetId() const { return _id; }
	int GetCount() const { return _strings.size(); }

private:
	struct ObjectOverride {
		char name[8];
		const utf8 *strings[4];
	};

	int _id;
	utf8 *_stringData;
	std::vector<utf8*> _strings;

	LanguagePack(int id, const utf8 *text);

	///////////////////////////////////////////////////////////////////////////
	// Parsing
	///////////////////////////////////////////////////////////////////////////
	utf8 *_currentGroup;

	void ParseLine(IStringReader *reader);
	void ParseGroup(IStringReader *reader);
	void ParseString(IStringReader *reader);
};
