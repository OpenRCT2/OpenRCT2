using UnityEngine;

namespace MeshBuilding
{
    /// <summary>
    /// Extents the MeshBuilder base class with some helpful extra methods.
    /// </summary>
    public static class MeshBuilderExtensions
	{
		/// <summary>
		/// Add a simple subdivided triangle to the builder. Vertex order is clock-wise.
		/// </summary>
		public static void AddSubdividedTriangle(this MeshBuilder builder, Vertex a, Vertex b, Vertex c, int levels, int submesh = 0)
		{
			Vertex midAB = new Vertex(Maths.GetCenter(a.position, b.position), Maths.GetCenter(a.normal, b.normal), Maths.GetCenter(a.uv, b.uv));
			Vertex midBC = new Vertex(Maths.GetCenter(b.position, c.position), Maths.GetCenter(b.normal, c.normal), Maths.GetCenter(b.uv, c.uv));
			Vertex midCA = new Vertex(Maths.GetCenter(c.position, a.position), Maths.GetCenter(c.normal, a.normal), Maths.GetCenter(c.uv, a.uv));

			if (levels <= 1)
			{
				builder.AddTriangle(a, midAB, midCA, submesh);
				builder.AddTriangle(b, midBC, midAB, submesh);
				builder.AddTriangle(c, midCA, midBC, submesh);
				builder.AddTriangle(midAB, midBC, midCA, submesh);
			}
			else
			{
				levels--;
				builder.AddSubdividedTriangle(a, midAB, midCA, levels, submesh);
				builder.AddSubdividedTriangle(b, midBC, midAB, levels, submesh);
				builder.AddSubdividedTriangle(c, midCA, midBC, levels, submesh);
				builder.AddSubdividedTriangle(midAB, midBC, midCA, levels, submesh);
			}
		}


		/// <summary>
		/// Add a simple subdivided quad to the builder. Vertex order is clock-wise.
		/// </summary>
		public static void AddSimpleSubdividedQuad(this MeshBuilder builder, Vertex a, Vertex b, Vertex c, Vertex d, int submesh = 0)
		{
			float[] matrix = new float[] { 1 / 16f, 3 / 16f, 9 / 16f, 3 / 16f };
			Vertex[] innerVerts = new Vertex[4];

			for (int p = 0; p < 4; p++)
			{
				//innerVerts[p] = new Vertex(matrix[p] * a.position + matrix[(p+1)%4] * b.position + matrix[(p+2)%4] * c.position + matrix[(p+3)%4] * d.position);
				innerVerts[p] = new Vertex(
					CalculateMatrix(matrix, p, a.position, b.position, c.position, d.position),
					Vector3.up,
					CalculateMatrix(matrix, p, a.uv, b.uv, c.uv, d.uv));
			}

			builder.AddQuad(innerVerts[3], innerVerts[2], innerVerts[1], innerVerts[0], submesh);
			builder.AddQuad(d, a, innerVerts[2], innerVerts[3], submesh);
			builder.AddQuad(a, b, innerVerts[1], innerVerts[2], submesh);
			builder.AddQuad(b, c, innerVerts[0], innerVerts[1], submesh);
			builder.AddQuad(c, d, innerVerts[3], innerVerts[0], submesh);
		}


        /// <summary>
        /// Calculates the matrix for the specified <see cref="Vector3"/>.
        /// </summary>
		static Vector3 CalculateMatrix(float[] matrix, int offset, params Vector3[] vectors)
		{
			Vector3 result = Vector3.zero;
			int max = vectors.Length;

			for(int i = 0; i < max; i++)
			{
				result += matrix[(i + offset) % 4] * vectors[i];
			}

			return result;
		}


        /// <summary>
        /// Calculates the matrix for the specified <see cref="Vector2"/>.
        /// </summary>
        static Vector2 CalculateMatrix(float[] matrix, int offset, params Vector2[] vectors)
		{
			Vector2 result = Vector2.zero;
			int max = vectors.Length;

			for (int i = 0; i < max; i++)
			{
				result += matrix[(i + offset) % 4] * vectors[i];
			}

			return result;
		}
	}
}
