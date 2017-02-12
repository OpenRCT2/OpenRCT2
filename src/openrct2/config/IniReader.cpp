#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "../common.h"
#include "../core/FileStream.hpp"
#include "../core/String.hpp"

class ConfigEnum
{

};

class IniReader
{
private:
    std::vector<uint8>                      _buffer;
    std::vector<std::tuple<size_t, size_t>> _lines;
    std::unordered_map<std::string, size_t> _sections;

public:
    IniReader(const std::string &path)
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        uint64 length = fs.GetLength();
        _buffer.resize(length);
        fs.Read(_buffer.data(), length);

        RemoveBOM();

        // Ensure there is a null terminator on the end, this is
        // mainly for ParseLines's sake
        if (_buffer[length - 1] != 0)
        {
            _buffer.push_back(0);
        }

        ParseLines();
        ParseSections();
    }

    bool ReadSection(const std::string &name)
    {
        auto it = _sections.find(name);
        if (it == _sections.end())
        {
            return false;
        }

        size_t startLine = it->second;
        UNUSED(startLine);
        return true;
    }

    bool GetBoolean(const std::string &name, bool defaultValue)
    {

    }

    sint32 GetSint32(const std::string &name, sint32 defaultValue, const ConfigEnum * configEnum = nullptr)
    {

    }

    std::string GetString(const std::string &name, const std::string &defaultValue)
    {

    }

private:
    void RemoveBOM()
    {
        utf8 * file = (utf8 *)_buffer.data();
        utf8 * content = String::SkipBOM(file);
        if (file != content)
        {
            size_t skipLength = content - file;
            _buffer.erase(_buffer.begin(), _buffer.begin() + skipLength);
        }
    }

    void ParseLines()
    {
        size_t lineBegin = 0;
        bool onNewLineCh = false;
        for (size_t i = 0; i < _buffer.size(); i++)
        {
            char b = (char)_buffer[i];
            if (b == 0 || b == '\n' || b == '\r')
            {
                if (!onNewLineCh)
                {
                    onNewLineCh = true;
                    size_t lineEnd = i;
                    _lines.emplace_back(lineBegin, lineEnd - lineBegin);
                }
            }
            else if (onNewLineCh)
            {
                onNewLineCh = false;
                lineBegin = i;
            }
        }
    }

    void ParseSections()
    {
        for (size_t i = 0; i < _lines.size(); i++)
        {
            std::string line = GetLine(i);
            line = String::Trim(line);
            if (line.size() > 3 && line[0] == '[')
            {
                size_t endIndex = line.find_first_of(']');
                std::string sectionName = line.substr(1, endIndex - 1);
                _sections[sectionName] = i;
            }
        }
    }

    std::string GetLine(size_t index)
    {
        utf8 * szBuffer = (utf8 *)_buffer.data();
        auto span = _lines[index];
        auto line = std::string(szBuffer + std::get<0>(span), std::get<1>(span));
        return line;
    }
};

extern "C"
{
    bool config_open(const utf8 * path)
    {
        auto iniReader = IniReader(path);
        bool b = iniReader.ReadSection("general");
        UNUSED(b);

        return false;
    }
}
