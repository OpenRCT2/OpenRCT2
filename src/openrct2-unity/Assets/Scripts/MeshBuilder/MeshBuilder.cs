using System.Collections.Generic;
using UnityEngine;

namespace MeshBuilding
{
    /// <summary>
    /// Small class to easily create meshes through C#.
    /// </summary>
    public class MeshBuilder
	{
		readonly Dictionary<Vertex, int> vertices = new Dictionary<Vertex, int>(64);
		readonly List<List<int>> triangles = new List<List<int>>(1); // per submesh
        Mesh mesh = null;


        /// <summary>
        /// Add a vertex to the mesh. Returns the index of this vertex.
        /// </summary>
        int AddVertex(Vertex a)
		{
			if (vertices.ContainsKey(a))
			{
				return vertices[a];
			}
			else
			{
				int idx = vertices.Count;
				vertices[a] = idx;
				return idx;
			}
		}


		/// <summary>
		/// Add a triangle with 3 vertices. Order: clock-wise.
		/// </summary>
		public void AddTriangle(Vertex a, Vertex b, Vertex c, int submesh = 0)
		{
			while (submesh >= triangles.Count)
				triangles.Add(new List<int>(16));

			triangles[submesh].AddRange(new int[]{
				AddVertex(a),
				AddVertex(b),
				AddVertex(c)
			});
		}


		/// <summary>
		/// Add two triangles in the shape of a quad. Order: top left -> top 
		/// right -> bottom right -> bottom left (clock-wise).
		/// </summary>
		public void AddQuad(Vertex a, Vertex b, Vertex c, Vertex d, int submesh = 0)
		{
			AddTriangle(a, b, c, submesh);
			AddTriangle(c, d, a, submesh);
		}


		/// <summary>
		/// Clear all vertice and triangle data in the MeshBuilder.
		/// </summary>
		public void Clear()
		{
			vertices.Clear();
			triangles.Clear();

            if (mesh != null)
			    mesh.Clear();
		}


		/// <summary>
		/// Build the vertices and triangles into a Mesh.
		/// </summary>
		public Mesh Build()
		{
			if (mesh == null)
				mesh = new Mesh();
			else
				mesh.Clear();

			int count = vertices.Count;
			if (count == 0)
				return null;

			Vector3[] verts = new Vector3[count];
			Vector3[] norms = new Vector3[count];
			Vector2[] uvs = new Vector2[count];
			int idx;
			foreach (KeyValuePair<Vertex, int> vert in vertices)
			{
				idx = vert.Value;

				verts[idx] = vert.Key.position;
				norms[idx] = vert.Key.normal;
				uvs[idx] = vert.Key.uv;
			}

			mesh.vertices = verts;
			mesh.normals = norms;
			mesh.uv = uvs;

			// Add all submeshes
			int subMeshCount = triangles.Count;
			mesh.subMeshCount = subMeshCount;

			for (int i = 0; i < subMeshCount; i++)
				mesh.SetTriangles(triangles[i], i);

			return mesh;
		}
	}
}

