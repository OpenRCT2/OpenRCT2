using System;
using UnityEngine;

namespace MeshBuilding
{
    /// <summary>
    /// Vertex value struct for storing its position, normal and uv information.
    /// </summary>
    public struct Vertex : IEquatable<Vertex>
	{
		public Vector3 position;
		public Vector3 normal;
		public Vector2 uv;


        #region Constructors

        /// <summary>
        /// Create a new vertex with a specified position, and its normal and uv set 
        /// to the default values.
        /// </summary>
        public Vertex(Vector3 pos)
		{
			position = pos;
			normal = Vector3.up;
			uv = new Vector2(pos.x, pos.z);
        }


        /// <summary>
        /// Create a new vertex with a specified position and normal vector, and the 
        /// uv set to the default values.
        /// </summary>
        public Vertex(Vector3 position, Vector3 normal)
		{
			this.position = position;
			this.normal = normal;
			uv = Vector2.zero;
		}


		/// <summary>
		/// Create a new vertex with a specified position, normal vector and uv value.
		/// </summary>
		public Vertex(Vector3 pos, Vector3 nor, Vector2 uvs)
		{
			position = pos;
			normal = nor;
			uv = uvs;
        }


        /// <summary>
        /// Create a new vertex with a specified position, and its normal and uv set 
        /// to the default values.
        /// </summary>
        public Vertex(float x, float y, float z)
            : this(new Vector3(x, y, z))
        {
        }


        /// <summary>
        /// Create a new vertex with a specified position and normal, and its uv set 
        /// to the default values.
        /// </summary>
        public Vertex(float x, float y, float z, Vector3 normal)
            : this(new Vector3(x, y, z), normal)
        {
        }


        /// <summary>
        /// Create a new vertex with a specified position, normal and uvs.
        /// </summary>
        public Vertex(float x, float y, float z, Vector3 normal, float u, float v)
            : this(new Vector3(x, y, z), normal, new Vector2(u, v))
        {
        }


        /// <summary>
        /// Create a new vertex with a specified position, normal and uvs.
        /// </summary>
        public Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
            : this(new Vector3(px, py, pz), new Vector3(nx, ny, nz), new Vector2(u, v))
        {
        }

        #endregion


        #region IEquatable implementation


        /// <summary>
        /// Determines whether two vertices are equal based on their position, normals
        /// and uv values.
        /// </summary>
        public static bool Equals(Vertex left, Vertex right)
            => (left.position == right.position
            && left.normal == right.normal
            && left.uv == right.uv);



        /// <summary>
        /// Determines whether the specified object is equal to this vertex.
        /// </summary>
        public override bool Equals(object obj)
            => (obj is Vertex vertex && Equals(this, vertex));


        /// <summary>
        /// Determines whether the specified vertex is equal to this vertex.
        /// </summary>
        public bool Equals(Vertex other)
            => (Equals(this, other));


        /// <summary>
        /// Determines the hash for this vertex.
        /// </summary>
        public override int GetHashCode()
            => (position.GetHashCode() ^ (normal.GetHashCode() << 2) ^ uv.GetHashCode() << 7);


        /// <summary>
        /// Determines whether two vertices are equal based on their position,
        /// normals and uv values.
        /// </summary>
        public static bool operator ==(Vertex left, Vertex right)
            => (Equals(left, right));


        /// <summary>
        /// Determines whether two vertices are not equal based on their position,
        /// normals and uv values.
        /// </summary>
        public static bool operator !=(Vertex left, Vertex right)
            => (!Equals(left, right));

        #endregion
    }
}
