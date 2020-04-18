using System;
using MeshBuilding;
using UnityEngine;

namespace OpenRCT2.Unity
{
	public partial class Map : MonoBehaviour
	{
		MeshFilter meshFilter;


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


		void LoadMap()
		{
			mapSize = OpenRCT2.GetMapSize();
			tiles = new Tile[mapSize, mapSize];

            Debug.Log($"Map size: {mapSize}");

			TileElement[] buffer = new TileElement[16];

			for (int x = 0; x < mapSize; x++)
			{
				for (int y = 0; y < mapSize; y++)
				{
					int amount = OpenRCT2.GetMapElementsAt(x, y, buffer, buffer.Length);

					tiles[x, y] = new Tile(buffer, amount);
				}
			}

			Mesh mesh = GenerateSurfaceMesh();
            mesh.name = "Map";
			meshFilter.sharedMesh = mesh;

            Debug.Log($"Map loaded!");
        }


		


		readonly struct Tile
		{
			public TileElement[] Elements { get; }


            readonly int surfaceIndex;


			public Tile(TileElement[] buffer, int size)
			{
				Elements = new TileElement[size];
				Array.Copy(buffer, Elements, size);

                surfaceIndex = Array.FindIndex(Elements, t => t.Type == TileElementType.Surface);
            }


            public int Count
                => Elements.Length;


            public SurfaceElement Surface
                => ((surfaceIndex != -1) ? Elements[surfaceIndex].AsSurface() : default);
		}
	}
}
