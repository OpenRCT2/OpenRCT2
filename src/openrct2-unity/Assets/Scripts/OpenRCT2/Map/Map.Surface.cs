using MeshBuilding;
using UnityEngine;

namespace OpenRCT2.Unity
{
    public partial class Map
    {
        void GenerateSurface(MeshBuilder builder, ref TileElement tile, int x, int y)
        {
            /* Surface coords to Unity:
             * 
             *  ^   W N
             *  |   S E
             *  y 
             *    x -->
             */
            SurfaceElement surface = tile.AsSurface();
            SurfaceSlope slope = surface.Slope;
            int baseHeight = tile.baseHeight;

            Vertex north = GetSurfaceCorner(x + 1, y + 1, baseHeight, slope, SurfaceSlope.NorthUp);
            Vertex east = GetSurfaceCorner(x + 1, y, baseHeight, slope, SurfaceSlope.EastUp);
            Vertex south = GetSurfaceCorner(x, y, baseHeight, slope, SurfaceSlope.SouthUp);
            Vertex west = GetSurfaceCorner(x, y + 1, baseHeight, slope, SurfaceSlope.WestUp);

            int submesh = GetMaterialIndex(MaterialType.Surface /*surface.SurfaceStyle*/);

            if ((slope & SurfaceSlope.WestUp) == 0 && (slope & SurfaceSlope.EastUp) == 0)
            {
                // In these cases the quad has to be rotated to show the correct kind of slope
                builder.AddTriangle(west, north, east, submesh);
                builder.AddTriangle(east, south, west, submesh);
            }
            else
            {
                builder.AddQuad(north, east, south, west, submesh);
            }

            TryAddSurfaceEdge(builder, north, west, x, y, 0, 1, SurfaceSlope.EastUp, SurfaceSlope.SouthUp, submesh); // Edge northwest
            TryAddSurfaceEdge(builder, east, north, x, y, 1, 0, SurfaceSlope.SouthUp, SurfaceSlope.WestUp, submesh); // Edge northeast
            TryAddSurfaceEdge(builder, south, east, x, y, 0, -1, SurfaceSlope.WestUp, SurfaceSlope.NorthUp, submesh); // Edge southeast
            TryAddSurfaceEdge(builder, west, south, x, y, -1, 0, SurfaceSlope.NorthUp, SurfaceSlope.EastDown, submesh); // Edge southwest
        }


        /// <summary>
        /// Tries to add an surface edge to the specified offset.
        /// </summary>
        void TryAddSurfaceEdge(MeshBuilder builder, Vertex left, Vertex right, int x, int y, int offsetX, int offsetY, SurfaceSlope leftOtherCorner, SurfaceSlope rightOtherCorner, int submesh)
        {
            SurfaceElement other = tiles[x + offsetX, y + offsetY].Surface;
            int baseHeight = other.BaseHeight;
            SurfaceSlope otherSlope = other.Slope;

            float leftHeight = GetSurfaceCornerHeight(baseHeight, otherSlope, leftOtherCorner) * HeightMultiplier;
            float rightHeight = GetSurfaceCornerHeight(baseHeight, otherSlope, rightOtherCorner) * HeightMultiplier;

            if (left.position.y > leftHeight || right.position.y > rightHeight)
            {
                Vector3 normal = new Vector3(offsetX, 0, offsetY);
                left.normal = normal;
                left.uv = new Vector2(0, left.position.y);
                right.normal = normal;
                right.uv = new Vector2(1, right.position.y);

                Vertex leftBottom = new Vertex(left.position.x, leftHeight, left.position.z, normal, 0, leftHeight);
                Vertex rightBottom = new Vertex(right.position.x, rightHeight, right.position.z, normal, 1, rightHeight);

                builder.AddQuad(left, right, rightBottom, leftBottom, submesh);
            }
        }


        Vertex GetSurfaceCorner(int x, int y, int startHeight, SurfaceSlope surfaceSlope, SurfaceSlope surfaceCorner)
        {
            int height = GetSurfaceCornerHeight(startHeight, surfaceSlope, surfaceCorner);

            Vector3 position = new Vector3(
                x * TileToCoordsMultiplier,
                height * HeightMultiplier,
                y * TileToCoordsMultiplier
            );

            return new Vertex(position);
        }


        int GetSurfaceCornerHeight(int height, SurfaceSlope surfaceSlope, SurfaceSlope surfaceCorner)
        {
            short slope = (short)surfaceSlope;
            short corner = (short)surfaceCorner;

            // Lift corner vertex up
            if ((slope & corner) != 0)
                height += TileHeightStep;

            if ((slope & (int)SurfaceSlope.DoubleHeight) != 0)
            {
                const int allcornersup = (int)SurfaceSlope.AllCornersUp;

                int corners = (slope & allcornersup); // get just the corners
                int opposite = (corner << 2) % allcornersup;

                // Check if all corners except opposite are raised.
                if (corners == (allcornersup & ~opposite))
                    height += TileHeightStep;
            }
            return height;
        }
    }
}
