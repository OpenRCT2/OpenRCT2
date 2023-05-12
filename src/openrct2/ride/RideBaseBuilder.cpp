#include "RideBaseBuilder.h"

RideBaseBuilder::RideBaseBuilder(uint8_t sizeX, uint8_t sizeY)
    : _sizeX(sizeX)
    , _sizeY(sizeY)
{
    _blocks.resize(sizeX * sizeY + 1);
    _edges.resize(sizeX * sizeY);
    _sequences.resize(sizeX * sizeY);
    _mapping.resize(4);

    for (size_t i = 0; i < 4; i++)
        _mapping[i].resize(sizeX * sizeY);

    BuildBlocks();
    BuildEdges();
    BuildMapping();
    BuildSequences();
}

void RideBaseBuilder::SwapTiles(uint8_t index0, uint8_t index1)
{
}

uint8_t RideBaseBuilder::GetTileIndex(uint8_t x, uint8_t y) const
{
    std::vector<PreviewTrack>::const_iterator res = std::find_if(
        _blocks.begin(), _blocks.end(), [x, y](const auto& block) { return block.x / 32 == x && block.y / 32 == y; });
    if (res != _blocks.end())
        return res->index;
    else
        return 255;
}


std::vector<PreviewTrack> RideBaseBuilder::GetBlocks() const
{
    return _blocks;
}

std::vector<uint8_t> RideBaseBuilder::GetEdges() const
{
    return _edges;
}

std::vector<std::vector<uint8_t>> RideBaseBuilder::GetMapping() const
{
    return _mapping;
}

std::vector<uint8_t> RideBaseBuilder::GetSequences() const
{
    return _sequences;
}


void RideBaseBuilder::BuildBlocks()
{
    uint8_t index = 0;
    for (int16_t y = 0; y < _sizeY * 32; y += 32)
    {
        for (int16_t x = 0; x < _sizeX * 32; x += 32)
        {
            PreviewTrack track{ index, x, y, 0, 64, { 0b1111, 0 }, 0 };
            _blocks[index] = track;
            index++;
        }
    }
    _blocks[_sizeX * _sizeY] = TRACK_BLOCK_END;
}

void RideBaseBuilder::BuildEdges()
{
    std::vector<std::vector<uint8_t>> edges;
    edges.resize(_sizeX);
    for (auto& edge : edges)
        edge.resize(_sizeY);

    for (uint8_t y = 0; y < _sizeY; y++)
    {
        for (uint8_t x = 0; x < _sizeX; x++)
        {
            if (x == 0)
            {
                if (y == 0)
                    edges[x][y] = EDGE_NE | EDGE_NW;
                else if (y == (_sizeY-1))
                    edges[x][y] = EDGE_NE | EDGE_SE;
                else
                    edges[x][y] = EDGE_NE;
            }
            else if (x == (_sizeX-1))
            {
                if (y == 0)
                    edges[x][y] = EDGE_NW | EDGE_SW;
                else if (y == (_sizeY-1))
                    edges[x][y] = EDGE_SW | EDGE_SE;
                else
                    edges[x][y] = EDGE_SW;
            }
            else
            {
                if (y == 0)
                    edges[x][y] = EDGE_NW;
                else if (y == (_sizeY - 1))
                    edges[x][y] = EDGE_SE;
                else
                    edges[x][y] = 0;
            }
        }
    }

    for (auto block = _blocks.cbegin(); block < _blocks.cend() - 1; block++)
    {
        auto index = block->index;
        auto x = block->x / 32;
        auto y = block->y / 32;
        _edges[index] = edges[x][y];
    }
}

void RideBaseBuilder::BuildMapping()
{
    std::vector<std::vector<uint8_t>> regular;
    regular.resize(_sizeX);
    for (auto& vec : regular)
        vec.resize(_sizeY);

    for (auto block = _blocks.cbegin(); block < _blocks.cend() - 1; block++)
    {
        auto x = block->x / 32;
        auto y = block->y / 32;

        regular[x][y] = block->index;
    }

    std::vector<std::vector<uint8_t>> transposed;
    transposed.resize(_sizeX);
    for (auto& elem : transposed)
        elem.resize(_sizeY);

    for (int x = 0; x < _sizeX; x++)
    {
        for (int y = 0; y < _sizeY; y++)
        {
            transposed[x][y] = regular[(_sizeY-1) - y][x];
        }
    }

    uint32_t index = 0;
    for (int y = 0; y < _sizeY; y++)
    {
        for (int x = 0; x < _sizeX; x++)
        {
            index = x + y * _sizeX;
            _mapping[0][index] = regular[x][y];
            _mapping[3][index] = transposed[x][y];
            index++;
        }
    }

    _mapping[2] = _mapping[0];
    std::reverse(_mapping[2].begin(), _mapping[2].end());

    _mapping[1] = _mapping[3];
    std::reverse(_mapping[1].begin(), _mapping[1].end());
}

void RideBaseBuilder::BuildSequences()
{
    std::vector<std::vector<uint8_t>> edges;
    edges.resize(_sizeX);
    for (auto& edge : edges)
        edge.resize(_sizeY);

    for (uint8_t y = 0; y < _sizeY; y++)
    {
        for (uint8_t x = 0; x < _sizeX; x++)
        {
            if (x == 0)
            {
                if (y == 0)
                    edges[x][y] = EDGE_NE | EDGE_NW;
                else if (y == (_sizeY-1))
                    edges[x][y] = EDGE_NE | EDGE_SE;
                else
                    edges[x][y] = EDGE_NE;
            }
            else if (x == (_sizeX-1))
            {
                if (y == 0)
                    edges[x][y] = EDGE_NW | EDGE_SW;
                else if (y == (_sizeY-1))
                    edges[x][y] = EDGE_SW | EDGE_SE;
                else
                    edges[x][y] = EDGE_SW;
            }
            else
            {
                if (y == 0)
                    edges[x][y] = EDGE_NW;
                else if (y == (_sizeY-1))
                    edges[x][y] = EDGE_SE;
                else
                    edges[x][y] = 0;
            }
        }
    }

    for (auto block = _blocks.cbegin(); block < _blocks.cend() - 1; block++)
    {
        auto index = block->index;
        auto x = block->x / 32;
        auto y = block->y / 32;
        _sequences[index] = edges[x][y];
    }
}


