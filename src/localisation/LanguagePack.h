#pragma once

#include <vector>

extern "C" {
	#include "../common.h"
	#include "../util/util.h"
	#include "localisation.h"
}

#include "../core/StringReader.hpp"

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
