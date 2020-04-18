using MeshBuilding;
using UnityEngine;

namespace OpenRCT2.Unity
{
    public partial class Map
    {
        [SerializeField] GameObject pathPrefab;
        [SerializeField] GameObject trackPrefab;
        [SerializeField] GameObject smallSceneryPrefab;


        const int TileCoordsToCoords = 32;
        const float TileCoordsToVector3Multiplier = 2;
        const float CoordsToVector3Multiplier = 2 / TileCoordsToCoords;
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

                case TileElementType.Path:
                    InstantiateElement(pathPrefab, x, tile.baseHeight, y);
                    break;

                case TileElementType.Track:
                    InstantiateElement(trackPrefab, x, tile.baseHeight, y);
                    break;

                case TileElementType.SmallScenery:
                    GameObject scenery = InstantiateElement(smallSceneryPrefab, x, tile.baseHeight, y);

                    Vector3 scale = scenery.transform.localScale;
                    scale.y = Mathf.Max((tile.clearanceHeight - tile.baseHeight) * HeightMultiplier, 1);
                    scenery.transform.localScale = scale;
                    break;
            }
        }


        GameObject InstantiateElement(GameObject obj, float x, float y, float z)
        {
            Vector3 position = TileCoordsToVector3(x, y, z);
            return Instantiate(obj, position, Quaternion.identity, transform);
        }
    }
}
