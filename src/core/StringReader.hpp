#pragma once

#include "../common.h"
#include "../localisation/localisation.h"
#include "../util/util.h"

interface IStringReader abstract {
	virtual bool TryPeek(int *outCodepoint) abstract;
	virtual bool TryRead(int *outCodepoint) abstract;
	virtual void Skip() abstract;
};

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
