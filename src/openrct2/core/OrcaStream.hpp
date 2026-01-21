/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <sfl/small_vector.hpp>
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
            reading,
            writing,
        };

        enum class CompressionType : uint32_t
        {
            none,
            gzip,
            zstd,
        };

    private:
#pragma pack(push, 1)
        struct Header
        {
            uint32_t magic{};
            uint32_t targetVersion{};
            uint32_t minVersion{};
            uint32_t numChunks{};
            uint64_t uncompressedSize{};
            CompressionType compression{};
            uint64_t compressedSize{};
            std::array<uint8_t, 8> fnv1a{};
            uint8_t padding[20]{};
        };
        static_assert(sizeof(Header) == 64, "Header should be 64 bytes");

        struct ChunkEntry
        {
            uint32_t id{};
            uint64_t offset{};
            uint64_t length{};
        };
#pragma pack(pop)

        IStream* _stream;
        Mode _mode;
        Header _header;
        sfl::small_vector<ChunkEntry, 32> _chunks;
        MemoryStream _buffer;
        ChunkEntry _currentChunk;
        int16_t _compressionLevel;

    public:
        OrcaStream(IStream& stream, const Mode mode, int16_t compressionLevel = Compression::kNoCompressionLevel)
        {
            _stream = &stream;
            _mode = mode;
            _compressionLevel = compressionLevel;
            if (mode == Mode::reading)
            {
                _header = _stream->ReadValue<Header>();

                _chunks.clear();
                for (uint32_t i = 0; i < _header.numChunks; i++)
                {
                    auto entry = _stream->ReadValue<ChunkEntry>();
                    _chunks.push_back(entry);
                }

                // Uncompress
                if (_header.compression != CompressionType::none)
                {
                    bool decompressStatus = false;

                    switch (_header.compression)
                    {
                        case CompressionType::gzip:
                            decompressStatus = Compression::zlibDecompress(
                                *_stream, _header.compressedSize, _buffer, _header.uncompressedSize,
                                Compression::ZlibHeaderType::gzip);
                            break;
                        case CompressionType::zstd:
                            decompressStatus = Compression::zstdDecompress(
                                *_stream, _header.compressedSize, _buffer, _header.uncompressedSize);
                            break;
                        default:
                            throw IOException("Unknown park compression type");
                    }

                    if (!decompressStatus)
                        throw IOException("Decompression error!");
                }
                else
                {
                    if (_header.uncompressedSize != _header.compressedSize)
                        throw IOException("Compressed and uncompressed sizes don't match!");
                    _buffer.CopyFromStream(*_stream, _header.uncompressedSize);
                }

                // early in-dev versions used SHA1 instead of FNV1a, so just assume any file
                // with a verison number of 0 may be one of these, and don't check their hashes.
                if (_header.targetVersion > 0)
                {
                    auto checksum = Crypt::FNV1a(_buffer.GetData(), _buffer.GetLength());
                    if (checksum != _header.fnv1a)
                        throw IOException("Checksum is not valid!");
                }
            }
            else
            {
                _header = {};
                _header.compression = _compressionLevel == Compression::kNoCompressionLevel ? CompressionType::none
                                                                                            : CompressionType::zstd;
                _buffer = MemoryStream{};
            }
        }

        OrcaStream(const OrcaStream&) = delete;

        ~OrcaStream()
        {
            if (_mode == Mode::writing)
            {
                _header.numChunks = static_cast<uint32_t>(_chunks.size());
                _header.uncompressedSize = _buffer.GetLength();
                _header.compressedSize = _buffer.GetLength();
                _header.fnv1a = Crypt::FNV1a(_buffer.GetData(), _buffer.GetLength());

                if (_compressionLevel == Compression::kNoCompressionLevel)
                    _header.compression = CompressionType::none;

                // Compress data
                if (_header.compression != CompressionType::none)
                {
                    MemoryStream compressed;
                    bool compressStatus = false;

                    _buffer.SetPosition(0);
                    switch (_header.compression)
                    {
                        case CompressionType::gzip:
                            compressStatus = Compression::zlibCompress(
                                _buffer, _buffer.GetLength(), compressed, Compression::ZlibHeaderType::gzip, _compressionLevel);
                            break;
                        case CompressionType::zstd:
                            // PARK header already has length and checksum, so exclude them in the compression frame
                            compressStatus = Compression::zstdCompress(
                                _buffer, _buffer.GetLength(), compressed, Compression::ZstdMetadata::none, _compressionLevel);
                            break;
                        default:
                            break;
                    }

                    if (compressStatus && compressed.GetLength() < _buffer.GetLength())
                    {
                        _buffer = std::move(compressed);
                        _header.compressedSize = _buffer.GetLength();
                    }
                    else
                    {
                        // Compression increases filesize, so just store uncompressed data
                        _header.compression = CompressionType::none;
                    }
                }

                // Write header and chunk table
                _stream->WriteValue(_header);
                for (const auto& chunk : _chunks)
                    _stream->WriteValue(chunk);
                // Write chunk data
                _stream->Write(_buffer.GetData(), _buffer.GetLength());
            }
        }

        Mode getMode() const
        {
            return _mode;
        }

        Header& getHeader()
        {
            return _header;
        }

        const Header& getHeader() const
        {
            return _header;
        }

        int16_t getCompressionLevel() const
        {
            return _compressionLevel;
        }

        void setCompressionLevel(int16_t compressionLevel)
        {
            _compressionLevel = compressionLevel;
        }

        template<typename TFunc>
        bool readWriteChunk(const uint32_t chunkId, TFunc f)
        {
            if (_mode == Mode::reading)
            {
                if (seekChunk(chunkId))
                {
                    ChunkStream stream(_buffer, _mode);
                    f(stream);
                    return true;
                }

                return false;
            }

            _currentChunk.id = chunkId;
            _currentChunk.offset = _buffer.GetPosition();
            _currentChunk.length = 0;
            ChunkStream stream(_buffer, _mode);
            f(stream);
            _currentChunk.length = static_cast<uint64_t>(_buffer.GetPosition()) - _currentChunk.offset;
            _chunks.push_back(_currentChunk);
            return true;
        }

    private:
        bool seekChunk(const uint32_t id)
        {
            const auto result = std::find_if(_chunks.begin(), _chunks.end(), [id](const ChunkEntry& e) { return e.id == id; });
            if (result != _chunks.end())
            {
                const auto offset = result->offset;
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
                std::streampos startPos{};
                std::streampos lastPos{};
                size_t count{};
                size_t elementSize{};
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

            Mode getMode() const
            {
                return _mode;
            }

            MemoryStream& getStream()
            {
                return _buffer;
            }

            void readWrite(void* addr, const size_t len)
            {
                if (_mode == Mode::reading)
                {
                    readBuffer(addr, len);
                }
                else
                {
                    writeBuffer(addr, len);
                }
            }

            void read(void* addr, const size_t len)
            {
                if (_mode == Mode::reading)
                {
                    readBuffer(addr, len);
                }
                else
                {
                    throw std::runtime_error("Incorrect mode");
                }
            }

            void write(const void* addr, const size_t len)
            {
                if (_mode == Mode::reading)
                {
                    throw std::runtime_error("Incorrect mode");
                }
                else
                {
                    writeBuffer(addr, len);
                }
            }

            template<typename T, std::enable_if_t<std::is_integral<T>::value, bool> = true>
            void readWrite(T& v)
            {
                if (_mode == Mode::reading)
                {
                    v = readInteger<T>();
                }
                else
                {
                    writeInteger(v);
                }
            }

            template<typename T, std::enable_if_t<std::is_enum<T>::value, bool> = true>
            void readWrite(T& v)
            {
                using underlying = std::underlying_type<T>::type;
                if (_mode == Mode::reading)
                {
                    v = static_cast<T>(readInteger<underlying>());
                }
                else
                {
                    writeInteger(static_cast<underlying>(v));
                }
            }

            template<typename T, T TNullValue, typename TTag>
            void readWrite(TIdentifier<T, TNullValue, TTag>& value)
            {
                if (_mode == Mode::reading)
                {
                    T temp{};
                    readWrite(temp);
                    value = TIdentifier<T, TNullValue, TTag>::FromUnderlying(temp);
                }
                else
                {
                    auto temp = value.ToUnderlying();
                    readWrite(temp);
                }
            }

            void readWrite(bool& value)
            {
                uint8_t value8 = value ? 1 : 0;
                readWrite(&value8, sizeof(value8));
                value = value8 != 0;
            }

            void readWrite(CoordsXY& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
            }

            void readWrite(CoordsXYZ& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
                readWrite(coords.z);
            }

            void readWrite(CoordsXYZD& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
                readWrite(coords.z);
                readWrite(coords.direction);
            }

            void readWrite(TileCoordsXY& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
            }

            void readWrite(TileCoordsXYZ& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
                readWrite(coords.z);
            }

            void readWrite(TileCoordsXYZD& coords)
            {
                readWrite(coords.x);
                readWrite(coords.y);
                readWrite(coords.z);
                readWrite(coords.direction);
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            T read()
            {
                T v{};
                readWrite(v);
                return v;
            }

            void readWrite(std::string& v)
            {
                if (_mode == Mode::reading)
                {
                    v = readString();
                }
                else
                {
                    writeString(v);
                }
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            void write(T v)
            {
                if (_mode == Mode::reading)
                {
                    T temp{};
                    readWrite(temp);
                }
                else
                {
                    readWrite(v);
                }
            }

            void write(const char* v)
            {
                std::string_view sv;
                if (v != nullptr)
                    sv = v;
                write(sv);
            }

            void write(const std::string_view v)
            {
                if (_mode == Mode::reading)
                {
                    std::string temp;
                    readWrite(temp);
                }
                else
                {
                    writeString(v);
                }
            }

            void write(const std::string& v)
            {
                write(std::string_view(v));
            }

            template<typename TVec, typename TFunc>
            void readWriteVector(TVec& vec, TFunc f)
            {
                if (_mode == Mode::reading)
                {
                    const auto count = beginArray();
                    vec.clear();
                    for (size_t i = 0; i < count; i++)
                    {
                        auto& el = vec.emplace_back();
                        f(el);
                        nextArrayElement();
                    }
                    endArray();
                }
                else
                {
                    beginArray();
                    for (auto& el : vec)
                    {
                        f(el);
                        nextArrayElement();
                    }
                    endArray();
                }
            }

            template<typename TArr, typename TFunc>
            void readWriteArray(std::span<TArr> arr, TFunc f)
            {
                if (_mode == Mode::reading)
                {
                    const auto count = beginArray();
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
                        nextArrayElement();
                    }
                    endArray();
                }
                else
                {
                    beginArray();
                    for (auto& el : arr)
                    {
                        if (f(el))
                        {
                            nextArrayElement();
                        }
                    }
                    endArray();
                }
            }

            template<typename TArr, size_t TArrSize, typename TFunc>
            void readWriteArray(TArr (&arr)[TArrSize], TFunc f)
            {
                readWriteArray(std::span<TArr>{ arr, TArrSize }, f);
            }

            template<typename TArr, size_t TArrSize, typename TFunc>
            void readWriteArray(std::array<TArr, TArrSize>& arr, TFunc f)
            {
                readWriteArray(std::span<TArr>{ arr.begin(), arr.end() }, f);
            }

            template<typename T>
            void ignore()
            {
                T value{};
                readWrite(value);
            }

        private:
            void readBuffer(void* dst, const size_t len)
            {
                _buffer.Read(dst, len);
            }

            void writeBuffer(const void* buffer, const size_t len)
            {
                _buffer.Write(buffer, len);
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            T readInteger()
            {
                if constexpr (sizeof(T) > 4)
                {
                    if constexpr (std::is_signed<T>())
                    {
                        int64_t raw{};
                        read(&raw, sizeof(raw));
                        return static_cast<T>(raw);
                    }
                    else
                    {
                        uint64_t raw{};
                        read(&raw, sizeof(raw));
                        return static_cast<T>(raw);
                    }
                }
                else
                {
                    if constexpr (std::is_signed<T>())
                    {
                        int32_t raw{};
                        read(&raw, sizeof(raw));
                        if (raw < std::numeric_limits<T>::min() || raw > std::numeric_limits<T>::max())
                        {
                            throw std::runtime_error("Value is incompatible with internal type.");
                        }
                        return static_cast<T>(raw);
                    }
                    else
                    {
                        uint32_t raw{};
                        read(&raw, sizeof(raw));
                        if (raw > std::numeric_limits<T>::max())
                        {
                            throw std::runtime_error("Value is incompatible with internal type.");
                        }
                        return static_cast<T>(raw);
                    }
                }
            }

            template<typename T, typename = std::enable_if<std::is_integral<T>::value>>
            void writeInteger(const T value)
            {
                if constexpr (sizeof(T) > 4)
                {
                    if constexpr (std::is_signed<T>())
                    {
                        auto raw = static_cast<int64_t>(value);
                        write(&raw, sizeof(raw));
                    }
                    else
                    {
                        auto raw = static_cast<uint64_t>(value);
                        write(&raw, sizeof(raw));
                    }
                }
                else
                {
                    if constexpr (std::is_signed<T>())
                    {
                        auto raw = static_cast<int32_t>(value);
                        write(&raw, sizeof(raw));
                    }
                    else
                    {
                        auto raw = static_cast<uint32_t>(value);
                        write(&raw, sizeof(raw));
                    }
                }
            }

            std::string readString()
            {
                std::string buffer;
                while (true)
                {
                    char c{};
                    readBuffer(&c, sizeof(c));
                    if (c == '\0')
                    {
                        break;
                    }
                    buffer.push_back(c);
                }
                return buffer;
            }

            void writeString(const std::string_view s)
            {
                const char nullt = '\0';
                auto len = s.find('\0');
                if (len == std::string_view::npos)
                {
                    len = s.size();
                }
                if (len > 0)
                {
                    _buffer.Write(s.data(), len);
                }
                _buffer.Write(&nullt, sizeof(nullt));
            }

            size_t beginArray()
            {
                auto& arrayState = _arrayStack.emplace();
                if (_mode == Mode::reading)
                {
                    arrayState.count = read<uint32_t>();
                    arrayState.elementSize = read<uint32_t>();
                    arrayState.lastPos = _buffer.GetPosition();
                    return arrayState.count;
                }

                arrayState.count = 0;
                arrayState.elementSize = 0;
                arrayState.startPos = _buffer.GetPosition();
                write<uint32_t>(0);
                write<uint32_t>(0);
                arrayState.lastPos = _buffer.GetPosition();
                return 0;
            }

            bool nextArrayElement()
            {
                auto& arrayState = _arrayStack.top();
                if (_mode == Mode::reading)
                {
                    if (arrayState.count == 0)
                    {
                        return false;
                    }
                    if (arrayState.elementSize != 0)
                    {
                        arrayState.lastPos += arrayState.elementSize;
                        _buffer.SetPosition(arrayState.lastPos);
                    }
                    arrayState.count--;
                    return arrayState.count == 0;
                }

                const auto lastElSize = static_cast<size_t>(_buffer.GetPosition()) - arrayState.lastPos;
                if (arrayState.count == 0)
                {
                    // Set array element size based on first element size
                    arrayState.elementSize = lastElSize;
                }
                else if (arrayState.elementSize != lastElSize)
                {
                    // Array element size was different from first element so reset it
                    // to dynamic
                    arrayState.elementSize = 0;
                }
                arrayState.count++;
                arrayState.lastPos = _buffer.GetPosition();
                return true;
            }

            void endArray()
            {
                auto& arrayState = _arrayStack.top();
                if (_mode == Mode::writing)
                {
                    const size_t backupPos = _buffer.GetPosition();
                    if (backupPos != static_cast<size_t>(arrayState.startPos) + 8 && arrayState.count == 0)
                    {
                        throw std::runtime_error("Array data was written but no elements were added.");
                    }
                    _buffer.SetPosition(arrayState.startPos);
                    write(static_cast<uint32_t>(arrayState.count));
                    write(static_cast<uint32_t>(arrayState.elementSize));
                    _buffer.SetPosition(backupPos);
                }
                _arrayStack.pop();
            }
        };
    };
} // namespace OpenRCT2
