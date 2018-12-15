#include <cstdint>
#include <fstream>
#include <string_view>
#include <vector>

namespace OpenRCT2
{
    class ParkFile
    {
    public:
        void Save(const std::string_view& path);

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
            uint8_t Sha1{};
        };

        struct ChunkEntry
        {
            uint32_t Id{};
            uint64_t Offset{};
            uint64_t Length{};
        };
#pragma pack(pop)

        Header _header;
        std::vector<ChunkEntry> _chunks;
        ChunkEntry _currentChunk;

        void WriteHeader(std::ostream& stream, const Header& header, const std::vector<ChunkEntry>& chunks);
        void BeginChunk(std::ostream& stream);
        uint64_t EndChunk();
    };
} // namespace OpenRCT2
