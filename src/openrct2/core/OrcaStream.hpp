/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Compression.h"
#include "../world/Location.hpp"
#include "Crypt.h"
#include "FileStream.h"
#include "Identifier.hpp"
#include "MemoryStream.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <span>
#include <stack>
#include <type_traits>
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

        enum class CompressionType : uint32_t
        {
            none,
            gzip,
        };

    private:
#pragma pack(push, 1)
        struct Header
        {
            uint32_t Magic{};
            uint32_t TargetVersion{};
            uint32_t MinVersion{};
            uint32_t NumChunks{};
            uint64_t UncompressedSize{};
            CompressionType Compression{};
            uint64_t CompressedSize{};
            std::array<uint8_t, 8> FNV1a{};
            uint8_t padding[20];
        };
        static_assert(sizeof(Header) == 64, "Header should be 64 bytes");

        struct ChunkEntry
        {
            uint32_t Id{};
            uint64_t Offset{};
            uint64_t Length{};
        };
#pragma pack(pop)

        IStream* _stream;
        Mode _mode;
        Header _header;
        std::vector<ChunkEntry> _chunks;
        MemoryStream _buffer;
        ChunkEntry _currentChunk;

    public:
        OrcaStream(IStream& stream, const Mode mode)
        {
            _stream = &stream;
            _mode = mode;
            if (mode == Mode::READING)
            {
                _header = _stream->ReadValue<Header>();

                _chunks.clear();
                for (uint32_t i = 0; i < _header.NumChunks; i++)
                {
                    auto entry = _stream->ReadValue<ChunkEntry>();
                    _chunks.push_back(entry);
                }

                // Read compressed data into buffer (read in blocks)
                _buffer = MemoryStream{};
                uint8_t temp[2048];
                uint64_t bytesLeft = _header.CompressedSize;
                do
                {
                    auto readLen = std::min(size_t(bytesLeft), sizeof(temp));
                    _stream->Read(temp, readLen);
                    _buffer.Write(temp, readLen);
                    bytesLeft -= readLen;
                } while (bytesLeft > 0);

                // Uncompress
                if (_header.Compression == CompressionType::gzip)
                {
                    auto uncompressedData = Compression::ungzip(_buffer.GetData(), _buffer.GetLength());
                    if (_header.UncompressedSize != uncompressedData.size())
                    {
                        // Warning?
                    }
                    _buffer.Clear();
                    _buffer.Write(uncompressedData.data(), uncompressedData.size());
                }
            }
            else
            {
                _header = {};
                _header.Compression = CompressionType::gzip;

                _buffer = MemoryStream{};
            }
        }

        OrcaStream(const OrcaStream&) = delete;

        ~OrcaStream()
        {
            if (_mode == Mode::WRITING)
            {
                const void* uncompressedData = _buffer.GetData();
                const uint64_t uncompressedSize = _buffer.GetLength();

                _header.NumChunks = static_cast<uint32_t>(_chunks.size());
                _header.UncompressedSize = uncompressedSize;
                _header.CompressedSize = uncompressedSize;
                _header.FNV1a = Crypt::FNV1a(uncompressedData, uncompressedSize);

                // Compress data
                std::optional<std::vector<uint8_t>> compressedBytes;
                if (_header.Compression == CompressionType::gzip)
                {
                    compressedBytes = Compression::gzip(uncompressedData, uncompressedSize);
                    if (compressedBytes)
                    {
                        _header.CompressedSize = compressedBytes->size();
                    }
                    else
                    {
                        // Compression failed
                        _header.Compression = CompressionType::none;
                    }
                }

                // Write header and chunk table
                _stream->WriteValue(_header);
                for (const auto& chunk : _chunks)
                {
                    _stream->WriteValue(chunk);
                }

                // Write chunk data
                if (compressedBytes)
                {
                    _stream->Write(compressedBytes->data(), compressedBytes->size());
                }
                else
                {
                    _stream->Write(uncompressedData, uncompressedSize);
                }
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

        template<typename TFunc>
        bool ReadWriteChunk(const uint32_t chunkId, TFunc f)
        {
            if (_mode == Mode::READING)
            {
                if (SeekChunk(chunkId))
                {
                    ChunkStream stream(_buffer, _mode);
                    f(stream);
                    return true;
                }

                return false;
            }

            _currentChunk.Id = chunkId;
            _currentChunk.Offset = _buffer.GetPosition();
            _currentChunk.Length = 0;
            ChunkStream stream(_buffer, _mode);
            f(stream);
            _currentChunk.Length = static_cast<uint64_t>(_buffer.GetPosition()) - _currentChunk.Offset;
            _chunks.push_back(_currentChunk);
            return true;
        }

    private:
        bool SeekChunk(const uint32_t id)
        {
            const auto result = std::find_if(_chunks.begin(), _chunks.end(), [id](const ChunkEntry& e) { return e.Id == id; });
            if (result != _chunks.end())
            {
                const auto offset = result->Offset;
                _buffer.SetPosition(offset);
                return true;
            }
            return false;
        }

    public:
        class ChunkStream
        {
        private:
            struct ArrayState
            {
                std::streampos StartPos{};
                std::streampos LastPos{};
                size_t Count{};
                size_t ElementSize{};
            };

            MemoryStream& _buffer;
            Mode _mode;
            std::stack<ArrayState> _arrayStack;

        public:
            ChunkStream(MemoryStream& buffer, const Mode mode)
                : _buffer(buffer)
                , _mode(mode)
            {
            }

            Mode GetMode() const
            {
                return _mode;
            }

            MemoryStream& GetStream()
            {
                return _buffer;
            }

            void ReadWrite(void* addr, const size_t len)
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

            void Read(void* addr, const size_t len)
            {
                if (_mode == Mode::READING)
                {
                    ReadBuffer(addr, len);
                }
                else
                {
                    throw std::runtime_error("Incorrect mode");
                }
            }

            void Write(const void* addr, const size_t len)
            {
                if (_mode == Mode::READING)
                {
                    throw std::runtime_error("Incorrect mode");
                }
                else
                {
                    WriteBuffer(addr, len);
                }
            }

            template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
            void ReadWrite(T& v)
            {
                if (_mode == Mode::READING)
                {
                    v = ReadInteger<T>();
                }
                else
                {
                    WriteInteger(v);
                }
            }

            template<typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
            void ReadWrite(T& v)
            {
                using underlying = typename std::underlying_type<T>::type;
                if (_mode == Mode::READING)
                {
                    v = static_cast<T>(ReadInteger<underlying>());
                }
                else
                {
                    WriteInteger(static_cast<underlying>(v));
                }
            }

            template<typename T, T TNullValue, typename TTag>
            void ReadWrite(TIdentifier<T, TNullValue, TTag>& value)
            {
                if (_mode == Mode::READING)
                {
                    T temp{};
                    ReadWrite(temp);
                    value = TIdentifier<T, TNullValue, TTag>::FromUnderlying(temp);
                }
                else
                {
                    auto temp = value.ToUnderlying();
                    ReadWrite(temp);
                }
            }

            void ReadWrite(bool& value)
            {
                uint8_t value8 = value ? 1 : 0;
                ReadWrite(&value8, sizeof(value8));
                value = value8 != 0;
            }

            void ReadWrite(CoordsXY& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
            }

            void ReadWrite(CoordsXYZ& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
                ReadWrite(coords.z);
            }

            void ReadWrite(CoordsXYZD& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
                ReadWrite(coords.z);
                ReadWrite(coords.direction);
            }

            void ReadWrite(TileCoordsXY& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
            }

            void ReadWrite(TileCoordsXYZ& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
                ReadWrite(coords.z);
            }

            void ReadWrite(TileCoordsXYZD& coords)
            {
                ReadWrite(coords.x);
                ReadWrite(coords.y);
                ReadWrite(coords.z);
                ReadWrite(coords.direction);
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            T Read()
            {
                T v{};
                ReadWrite(v);
                return v;
            }

            void ReadWrite(std::string& v)
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

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            void Write(T v)
            {
                if (_mode == Mode::READING)
                {
                    T temp{};
                    ReadWrite(temp);
                }
                else
                {
                    ReadWrite(v);
                }
            }

            void Write(const char* v)
            {
                std::string_view sv;
                if (v != nullptr)
                    sv = v;
                Write(sv);
            }

            void Write(const std::string_view v)
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

            void Write(const std::string& v)
            {
                Write(std::string_view(v));
            }

            template<typename TVec, typename TFunc>
            void ReadWriteVector(TVec& vec, TFunc f)
            {
                if (_mode == Mode::READING)
                {
                    const auto count = BeginArray();
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

            template<typename TArr, typename TFunc>
            void ReadWriteArray(std::span<TArr> arr, TFunc f)
            {
                if (_mode == Mode::READING)
                {
                    const auto count = BeginArray();
                    for (auto& el : arr)
                    {
                        el = {};
                    }
                    for (size_t i = 0; i < count; i++)
                    {
                        if (i < arr.size())
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

            template<typename TArr, size_t TArrSize, typename TFunc>
            void ReadWriteArray(TArr (&arr)[TArrSize], TFunc f)
            {
                ReadWriteArray(std::span<TArr>{ arr, TArrSize }, f);
            }

            template<typename TArr, size_t TArrSize, typename TFunc>
            void ReadWriteArray(std::array<TArr, TArrSize>& arr, TFunc f)
            {
                ReadWriteArray(std::span<TArr>{ arr.begin(), arr.end() }, f);
            }

            template<typename T>
            void Ignore()
            {
                T value{};
                ReadWrite(value);
            }

        private:
            void ReadBuffer(void* dst, const size_t len)
            {
                _buffer.Read(dst, len);
            }

            void WriteBuffer(const void* buffer, const size_t len)
            {
                _buffer.Write(buffer, len);
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            T ReadInteger()
            {
                if constexpr (sizeof(T) > 4)
                {
                    if constexpr (std::is_signed<T>())
                    {
                        int64_t raw{};
                        Read(&raw, sizeof(raw));
                        return static_cast<T>(raw);
                    }
                    else
                    {
                        uint64_t raw{};
                        Read(&raw, sizeof(raw));
                        return static_cast<T>(raw);
                    }
                }
                else
                {
                    if constexpr (std::is_signed<T>())
                    {
                        int32_t raw{};
                        Read(&raw, sizeof(raw));
                        if (raw < std::numeric_limits<T>::min() || raw > std::numeric_limits<T>::max())
                        {
                            throw std::runtime_error("Value is incompatible with internal type.");
                        }
                        return static_cast<T>(raw);
                    }
                    else
                    {
                        uint32_t raw{};
                        Read(&raw, sizeof(raw));
                        if (raw > std::numeric_limits<T>::max())
                        {
                            throw std::runtime_error("Value is incompatible with internal type.");
                        }
                        return static_cast<T>(raw);
                    }
                }
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            void WriteInteger(const T value)
            {
                if constexpr (sizeof(T) > 4)
                {
                    if constexpr (std::is_signed<T>())
                    {
                        auto raw = static_cast<int64_t>(value);
                        Write(&raw, sizeof(raw));
                    }
                    else
                    {
                        auto raw = static_cast<uint64_t>(value);
                        Write(&raw, sizeof(raw));
                    }
                }
                else
                {
                    if constexpr (std::is_signed<T>())
                    {
                        auto raw = static_cast<int32_t>(value);
                        Write(&raw, sizeof(raw));
                    }
                    else
                    {
                        auto raw = static_cast<uint32_t>(value);
                        Write(&raw, sizeof(raw));
                    }
                }
            }

            std::string ReadString()
            {
                std::string buffer;
                while (true)
                {
                    char c{};
                    ReadBuffer(&c, sizeof(c));
                    if (c == '\0')
                    {
                        break;
                    }
                    buffer.push_back(c);
                }
                return buffer;
            }

            void WriteString(const std::string_view s)
            {
                const char nullt = '\0';
                auto len = s.find('\0');
                if (len == std::string_view::npos)
                {
                    len = s.size();
                }
                _buffer.Write(s.data(), len);
                _buffer.Write(&nullt, sizeof(nullt));
            }

            size_t BeginArray()
            {
                auto& arrayState = _arrayStack.emplace();
                if (_mode == Mode::READING)
                {
                    arrayState.Count = Read<uint32_t>();
                    arrayState.ElementSize = Read<uint32_t>();
                    arrayState.LastPos = _buffer.GetPosition();
                    return arrayState.Count;
                }

                arrayState.Count = 0;
                arrayState.ElementSize = 0;
                arrayState.StartPos = _buffer.GetPosition();
                Write<uint32_t>(0);
                Write<uint32_t>(0);
                arrayState.LastPos = _buffer.GetPosition();
                return 0;
            }

            bool NextArrayElement()
            {
                auto& arrayState = _arrayStack.top();
                if (_mode == Mode::READING)
                {
                    if (arrayState.Count == 0)
                    {
                        return false;
                    }
                    if (arrayState.ElementSize != 0)
                    {
                        arrayState.LastPos += arrayState.ElementSize;
                        _buffer.SetPosition(arrayState.LastPos);
                    }
                    arrayState.Count--;
                    return arrayState.Count == 0;
                }

                const auto lastElSize = static_cast<size_t>(_buffer.GetPosition()) - arrayState.LastPos;
                if (arrayState.Count == 0)
                {
                    // Set array element size based on first element size
                    arrayState.ElementSize = lastElSize;
                }
                else if (arrayState.ElementSize != lastElSize)
                {
                    // Array element size was different from first element so reset it
                    // to dynamic
                    arrayState.ElementSize = 0;
                }
                arrayState.Count++;
                arrayState.LastPos = _buffer.GetPosition();
                return true;
            }

            void EndArray()
            {
                auto& arrayState = _arrayStack.top();
                if (_mode == Mode::WRITING)
                {
                    const size_t backupPos = _buffer.GetPosition();
                    if (backupPos != static_cast<size_t>(arrayState.StartPos) + 8 && arrayState.Count == 0)
                    {
                        throw std::runtime_error("Array data was written but no elements were added.");
                    }
                    _buffer.SetPosition(arrayState.StartPos);
                    Write(static_cast<uint32_t>(arrayState.Count));
                    Write(static_cast<uint32_t>(arrayState.ElementSize));
                    _buffer.SetPosition(backupPos);
                }
                _arrayStack.pop();
            }
        };
    };
} // namespace OpenRCT2
