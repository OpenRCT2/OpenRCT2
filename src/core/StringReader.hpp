#pragma once

#include "../common.h"
#include "../localisation/localisation.h"
#include "../util/util.h"
#include "String.hpp"

interface IStringReader
{
    virtual bool TryPeek(codepoint_t * outCodepoint)       abstract;
    virtual bool TryRead(codepoint_t * outCodepoint)       abstract;
    virtual void Skip()                                    abstract;
    virtual bool CanRead()                           const abstract;
};

class UTF8StringReader final : public IStringReader
{
public:
    UTF8StringReader(const utf8 * text)
    {
        text = String::SkipBOM(text);

        _text = text;
        _current = text;
    }

    bool TryPeek(codepoint_t * outCodepoint) override
    {
        if (_current == nullptr) return false;

        codepoint_t codepoint = String::GetNextCodepoint(_current);
        *outCodepoint = codepoint;
        return true;
    }

    bool TryRead(codepoint_t * outCodepoint) override
    {
        if (_current == nullptr) return false;

        codepoint_t codepoint = String::GetNextCodepoint(_current, &_current);
        *outCodepoint = codepoint;
        if (codepoint == 0)
        {
            _current = nullptr;
            return false;
        }
        return true;
    }

    void Skip() override
    {
        codepoint_t codepoint;
        TryRead(&codepoint);
    }

    bool CanRead() const override
    {
        return _current != nullptr;
    }

private:
    const utf8 *_text;
    const utf8 *_current;
};
