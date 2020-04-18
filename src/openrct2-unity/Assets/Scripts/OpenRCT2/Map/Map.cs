using System;
using UnityEngine;

namespace OpenRCT2.Unity
{
    /// <summary>
    /// The map of the park.
    /// </summary>
    public partial class Map : MonoBehaviour
    {
        MeshFilter meshFilter;


        const int MaxElementsPerTile = 128;

		int mapSize;
		Tile[,] tiles;


		void Awake()
		{
			meshFilter = gameObject.GetComponent<MeshFilter>();
		}


		void Start()
		{
			LoadMap();
			GenerateSurfaceMesh();

            Debug.Log($"Map load complete!");
        }


        /// <summary>
        /// Loads the map of the currently loaded park into Unity.
        /// </summary>
		void LoadMap()
		{
            // Remove all children
            foreach (Transform child in transform)
                Destroy(child.gameObject);

            // Load the map
            mapSize = OpenRCT2.GetMapSize();
			tiles = new Tile[mapSize, mapSize];

            Debug.Log($"Map size: {mapSize}");

			TileElement[] buffer = new TileElement[MaxElementsPerTile];

			for (int x = 0; x < mapSize; x++)
			{
				for (int y = 0; y < mapSize; y++)
				{
					int amount = OpenRCT2.GetMapElementsAt(x, y, buffer);

					tiles[x, y] = new Tile(buffer, amount);
				}
			}

            // Generate the surface mesh
			Mesh mesh = GenerateSurfaceMesh();
            mesh.name = "Map";
			meshFilter.sharedMesh = mesh;

            Debug.Log($"Map loaded!");
        }


        /// <summary>
        /// A tile struct containing multiple elements.
        /// </summary>
		readonly struct Tile
		{
            /// <summary>
            /// All the elements on this tile.
            /// </summary>
			public TileElement[] Elements { get; }


            // The index at which the surface tile element is located.
            readonly int surfaceIndex;


            /// <summary>
            /// Creates a new tile from the specified buffer.
            /// </summary>
			public Tile(TileElement[] buffer, int size)
			{
				Elements = new TileElement[size];
				Array.Copy(buffer, Elements, size);

                surfaceIndex = Array.FindIndex(Elements, t => t.Type == TileElementType.Surface);
            }


            /// <summary>
            /// Returns the amount of tile elements on this tile.
            /// </summary>
            public int Count
                => Elements.Length;


            /// <summary>
            /// Returns the surface element on this tile.
            /// </summary>
            public SurfaceElement Surface
                => ((surfaceIndex != -1) ? Elements[surfaceIndex].AsSurface() : default);
		}


        /// <summary>
        /// Transforms a OpenRCT2 TileCoords to the Unity coordination system.
        /// </summary>
        public static Vector3 TileCoordsToUnity(float x, float y, float z)
        {
            float halftile = TileCoordsToVector3Multiplier / 2f;

            return new Vector3(
                (x * TileCoordsToVector3Multiplier) + halftile,
                y * TileHeightMultiplier,
                (z * TileCoordsToVector3Multiplier) + halftile
            );
        }


        /// <summary>
        /// Transforms a OpenRCT2 Coords to the Unity coordination system.
        /// </summary>
        public static Vector3 CoordsToVector3(float x, float y, float z)
        {
            return new Vector3(
                (x * CoordsToVector3Multiplier),
                y * CoordsToVector3Multiplier,
                (z * CoordsToVector3Multiplier)
            );
        }


        /// <summary>
        /// Transforms a OpenRCT2 Coords to the Unity coordination system.
        /// </summary>
        public static Vector3 CoordsToVector3(Vector3 position)
            => CoordsToVector3(position.x, position.y, position.z);
    }
}
