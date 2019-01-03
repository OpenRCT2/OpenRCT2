/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Crypt.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

namespace OpenRCT2
{
    class OrcaStream
    {
    public:
        enum class Mode
        {
            READING,
            WRITING,
        };

        static constexpr uint32_t COMPRESSION_NONE = 0;
        static constexpr uint32_t COMPRESSION_GZIP = 1;

    private:
#pragma pack(push, 1)
        struct Header
        {
            uint32_t Magic{};
            uint32_t TargetVersion{};
            uint32_t MinVersion{};
            uint32_t NumChunks{};
            uint64_t UncompressedSize{};
            uint32_t Compression{};
            std::array<uint8_t, 20> Sha1{};
        };

        struct ChunkEntry
        {
            uint32_t Id{};
            uint64_t Offset{};
            uint64_t Length{};
        };
#pragma pack(pop)

        std::string _path;
        Mode _mode;
        Header _header;
        std::vector<ChunkEntry> _chunks;
        std::stringstream _buffer;
        ChunkEntry _currentChunk;

    public:
        OrcaStream(const std::string_view& path, Mode mode)
        {
            _path = path;
            _mode = mode;
            if (mode == Mode::READING)
            {
                std::ifstream fs(std::string(path).c_str(), std::ios::binary);
                fs.read((char*)&_header, sizeof(_header));

                _chunks.clear();
                for (uint32_t i = 0; i < _header.NumChunks; i++)
                {
                    ChunkEntry entry;
                    fs.read((char*)&entry, sizeof(entry));
                    _chunks.push_back(entry);
                }

                _buffer = std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
                _buffer.clear();

                char temp[2048];
                size_t read = 0;
                do
                {
                    fs.read(temp, sizeof(temp));
                    read = fs.gcount();
                    _buffer.write(temp, read);
                } while (read != 0);
            }
            else
            {
                _header = {};
                _header.Compression = COMPRESSION_NONE;

                _buffer = std::stringstream(std::ios::out | std::ios::binary);
            }
        }

        OrcaStream(const OrcaStream&) = delete;

        ~OrcaStream()
        {
            if (_mode == Mode::READING)
            {
            }
            else
            {
                // TODO avoid copying the buffer
                auto uncompressedData = _buffer.str();

                _header.NumChunks = (uint32_t)_chunks.size();
                _header.UncompressedSize = _buffer.tellp();
                _header.Sha1 = Crypt::SHA1(uncompressedData.data(), uncompressedData.size());

                std::ofstream fs(_path.c_str(), std::ios::binary);

                // Write header
                fs.seekp(0);
                fs.write((const char*)&_header, sizeof(_header));
                for (const auto& chunk : _chunks)
                {
                    fs.write((const char*)&chunk, sizeof(chunk));
                }

                // Write chunk data
                fs.write(uncompressedData.data(), uncompressedData.size());
            }
        }

        Mode GetMode() const
        {
            return _mode;
        }

        Header& GetHeader()
        {
            return _header;
        }

        const Header& GetHeader() const
        {
            return _header;
        }

        template<typename TFunc> bool ReadWriteChunk(uint32_t chunkId, TFunc f)
        {
            if (_mode == Mode::READING)
            {
                if (SeekChunk(chunkId))
                {
                    ChunkStream stream(_buffer, _mode);
                    f(stream);
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                _currentChunk.Id = chunkId;
                _currentChunk.Offset = _buffer.tellp();
                _currentChunk.Length = 0;
                ChunkStream stream(_buffer, _mode);
                f(stream);
                _currentChunk.Length = (uint64_t)_buffer.tellp() - _currentChunk.Offset;
                _chunks.push_back(_currentChunk);
                return true;
            }
        }

    private:
        bool SeekChunk(uint32_t id)
        {
            auto result = std::find_if(_chunks.begin(), _chunks.end(), [id](const ChunkEntry& e) { return e.Id == id; });
            if (result != _chunks.end())
            {
                auto offset = result->Offset;
                _buffer.seekg(offset);
                return true;
            }
            return false;
        }

    public:
        class ChunkStream
        {
        private:
            std::stringstream& _buffer;
            Mode _mode;
            std::streampos _currentArrayStartPos;
            std::streampos _currentArrayLastPos;
            size_t _currentArrayCount;
            size_t _currentArrayElementSize;

        public:
            ChunkStream(std::stringstream& buffer, Mode mode)
                : _buffer(buffer)
                , _mode(mode)
            {
            }

            Mode GetMode() const
            {
                return _mode;
            }

            void ReadWrite(void* addr, size_t len)
            {
                if (_mode == Mode::READING)
                {
                    ReadBuffer(addr, len);
                }
                else
                {
                    WriteBuffer(addr, len);
                }
            }

            template<typename T> void ReadWrite(T& v)
            {
                ReadWrite((void*)&v, sizeof(T));
            }

            template<typename TMem, typename TSave> void ReadWriteAs(TMem& v)
            {
                TSave sv;
                if (_mode != Mode::READING)
                {
                    sv = v;
                }
                ReadWrite((void*)&sv, sizeof(TSave));
                if (_mode == Mode::READING)
                {
                    v = static_cast<TMem>(sv);
                }
            }

            template<typename T> T Read()
            {
                T v{};
                ReadWrite(v);
                return v;
            }

            template<> void ReadWrite(std::string_view& v) = delete;

            template<> void ReadWrite(std::string& v)
            {
                if (_mode == Mode::READING)
                {
                    v = ReadString();
                }
                else
                {
                    WriteString(v);
                }
            }

            template<typename T> void Write(const T& v)
            {
                if (_mode == Mode::READING)
                {
                    T temp;
                    ReadWrite(temp);
                }
                else
                {
                    ReadWrite(v);
                }
            }

            template<> void Write(const std::string_view& v)
            {
                if (_mode == Mode::READING)
                {
                    std::string temp;
                    ReadWrite(temp);
                }
                else
                {
                    WriteString(v);
                }
            }

            template<typename TVec, typename TFunc> void ReadWriteVector(TVec& vec, TFunc f)
            {
                if (_mode == Mode::READING)
                {
                    auto count = BeginArray();
                    vec.clear();
                    for (size_t i = 0; i < count; i++)
                    {
                        auto& el = vec.emplace_back();
                        f(el);
                        NextArrayElement();
                    }
                    EndArray();
                }
                else
                {
                    BeginArray();
                    for (auto& el : vec)
                    {
                        f(el);
                        NextArrayElement();
                    }
                    EndArray();
                }
            }

            template<typename TArr, size_t TArrSize, typename TFunc> void ReadWriteArray(TArr (&arr)[TArrSize], TFunc f)
            {
                if (_mode == Mode::READING)
                {
                    auto count = BeginArray();
                    for (auto& el : arr)
                    {
                        el = {};
                    }
                    for (size_t i = 0; i < count; i++)
                    {
                        if (i < TArrSize)
                        {
                            f(arr[i]);
                        }
                        NextArrayElement();
                    }
                    EndArray();
                }
                else
                {
                    BeginArray();
                    for (auto& el : arr)
                    {
                        if (f(el))
                        {
                            NextArrayElement();
                        }
                    }
                    EndArray();
                }
            }

        private:
            void ReadBuffer(void* dst, size_t len)
            {
                _buffer.read((char*)dst, len);
            }

            void WriteBuffer(const void* buffer, size_t len)
            {
                _buffer.write((char*)buffer, len);
            }

            std::string ReadString()
            {
                std::string buffer;
                buffer.reserve(64);
                while (true)
                {
                    char c;
                    ReadBuffer(&c, sizeof(c));
                    if (c == 0)
                    {
                        break;
                    }
                    buffer.push_back(c);
                }
                buffer.shrink_to_fit();
                return buffer;
            }

            void WriteString(const std::string_view& s)
            {
                char nullt = '\0';
                auto len = s.find('\0');
                if (len == std::string_view::npos)
                {
                    len = s.size();
                }
                _buffer.write(s.data(), len);
                _buffer.write(&nullt, sizeof(nullt));
            }

            size_t BeginArray()
            {
                if (_mode == Mode::READING)
                {
                    _currentArrayCount = Read<uint32_t>();
                    _currentArrayElementSize = Read<uint32_t>();
                    _currentArrayLastPos = _buffer.tellg();
                    return _currentArrayCount;
                }
                else
                {
                    _currentArrayCount = 0;
                    _currentArrayElementSize = 0;
                    _currentArrayStartPos = _buffer.tellp();
                    Write<uint32_t>(0);
                    Write<uint32_t>(0);
                    _currentArrayLastPos = _buffer.tellp();
                    return 0;
                }
            }

            bool NextArrayElement()
            {
                if (_mode == Mode::READING)
                {
                    if (_currentArrayCount == 0)
                    {
                        return false;
                    }
                    if (_currentArrayElementSize != 0)
                    {
                        _currentArrayLastPos += _currentArrayElementSize;
                        _buffer.seekg(_currentArrayLastPos);
                    }
                    _currentArrayCount--;
                    return _currentArrayCount == 0;
                }
                else
                {
                    auto lastElSize = (size_t)_buffer.tellp() - _currentArrayLastPos;
                    if (_currentArrayCount == 0)
                    {
                        // Set array element size based on first element size
                        _currentArrayElementSize = lastElSize;
                    }
                    else if (_currentArrayElementSize != lastElSize)
                    {
                        // Array element size was different from first element so reset it
                        // to dynamic
                        _currentArrayElementSize = 0;
                    }
                    _currentArrayCount++;
                    _currentArrayLastPos = _buffer.tellp();
                    return true;
                }
            }

            void EndArray()
            {
                if (_mode == Mode::READING)
                {
                }
                else
                {
                    auto backupPos = _buffer.tellp();
                    if ((size_t)backupPos != (size_t)_currentArrayStartPos + 8 && _currentArrayCount == 0)
                    {
                        throw std::runtime_error("Array data was written but no elements were added.");
                    }
                    _buffer.seekp(_currentArrayStartPos);
                    Write((uint32_t)_currentArrayCount);
                    Write((uint32_t)_currentArrayElementSize);
                    _buffer.seekp(backupPos);
                }
            }
        };
    };
} // namespace OpenRCT2
