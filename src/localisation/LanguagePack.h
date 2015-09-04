#pragma once

#include <vector>

extern "C" {
	#include "../common.h"
	#include "../util/util.h"
	#include "localisation.h"
}

#include "../core/StringBuilder.hpp"
#include "../core/StringReader.hpp"

class LanguagePack final {
public:
	static LanguagePack *FromFile(int id, const utf8 *path);
	static LanguagePack *FromText(int id, const utf8 *text);

	~LanguagePack();

	int GetId() const { return _id; }
	int GetCount() const { return _strings.size(); }

	const utf8 *GetString(int stringId) const;

	void SetString(int stringId, const utf8 *str) {
		if (_strings.size() >= (size_t)stringId) {
			_strings[stringId] = str;
		}
	}

	rct_string_id GetObjectOverrideStringId(const char *objectIdentifier, int index);

private:
	struct ObjectOverride {
		char name[8];
		const utf8 *strings[4];
	};

	int _id;
	utf8 *_stringData;
	std::vector<const utf8*> _strings;
	std::vector<ObjectOverride> _objectOverrides;

	LanguagePack(int id, const utf8 *text);
	ObjectOverride *GetObjectOverride(const char *objectIdentifier);

	///////////////////////////////////////////////////////////////////////////
	// Parsing
	///////////////////////////////////////////////////////////////////////////
	StringBuilder _stringDataSB;
	utf8 *_currentGroup;
	ObjectOverride *_currentObjectOverride;

	void ParseLine(IStringReader *reader);
	void ParseGroup(IStringReader *reader);
	void ParseString(IStringReader *reader);

	bool ParseToken(IStringReader *reader, uint32 *token);
};
