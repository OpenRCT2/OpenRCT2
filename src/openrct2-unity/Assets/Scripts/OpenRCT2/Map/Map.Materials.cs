using System.Collections.Generic;

namespace OpenRCT2.Unity
{
    public partial class Map
    {
        enum MaterialType : byte
        {
            Surface,
            Edge,
            Water
        }


        List<RequestedMaterial> requestedSurfaceMaterials;


        void ResetSurfaceMaterials()
        {
            requestedSurfaceMaterials?.Clear();
        }


        /// <summary>
        /// Gets the material / submesh index for the given material type.
        /// </summary>
        int GetMaterialIndex(MaterialType type, byte value)
        {
            if (requestedSurfaceMaterials == null)
                requestedSurfaceMaterials = new List<RequestedMaterial>();

            RequestedMaterial material = new RequestedMaterial(type, value);
            int index = requestedSurfaceMaterials.IndexOf(material);

            if (index == -1)
            {
                // Add if it doesn't exist yet.
                index = requestedSurfaceMaterials.Count;
                requestedSurfaceMaterials.Add(material);
            }
            return index;
        }


        /// <summary>
        /// Gets the default material / submesh index for the given material type.
        /// </summary>
        int GetMaterialIndex(MaterialType style)
            => GetMaterialIndex(style, 0);


        /// <summary>
        /// Gets the material / submesh index for the given surface style.
        /// </summary>
        int GetMaterialIndex(TerrainSurfaceStyle style)
            => GetMaterialIndex(MaterialType.Surface, (byte)style);


        /// <summary>
        /// Gets the material / submesh index for the given terrain edge style.
        /// </summary>
        int GetMaterialIndex(TerrainEdgeStyle style)
            => GetMaterialIndex(MaterialType.Edge, (byte)style);


        readonly struct RequestedMaterial
        {
            public readonly MaterialType Type;
            public readonly byte Value;


            public RequestedMaterial(MaterialType type, byte value)
            {
                Type = type;
                Value = value;
            }


            public override bool Equals(object obj)
                => obj is RequestedMaterial material
                    && Type == material.Type
                    && Value == material.Value;


            public override int GetHashCode()
                => ((Value.GetHashCode() << 3) + Type.GetHashCode());
        }
    }
}
