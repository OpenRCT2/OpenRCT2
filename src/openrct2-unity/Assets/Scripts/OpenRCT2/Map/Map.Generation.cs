using MeshBuilding;
using UnityEngine;

namespace OpenRCT2.Unity
{
    public partial class Map
    {
        const float TileToCoordsMultiplier = 2;
        const float HeightMultiplier = 0.5f;
        const int TileHeightStep = 2;

        MeshBuilder cachedBuilder;


        Mesh GenerateSurfaceMesh()
        {
            ResetSurfaceMaterials();

            if (cachedBuilder == null)
                cachedBuilder = new MeshBuilder();
            else
                cachedBuilder.Clear();

            int end = mapSize - 1;
            for (int x = 1; x < end; x++)
            {
                for (int y = 1; y < end; y++)
                {
                    Tile tile = tiles[x, y];
                    for (int e = 0; e < tile.Count; e++)
                    {
                        GenerateTileElement(cachedBuilder, ref tile.Elements[e], x, y);
                    }
                }
            }

            return cachedBuilder.Build();
        }


        void GenerateTileElement(MeshBuilder builder, ref TileElement tile, int x, int y)
        {
            switch (tile.Type)
            {
                case TileElementType.Surface:
                    GenerateSurface(builder, ref tile, x, y);
                    break;
            }
        }
    }
}
