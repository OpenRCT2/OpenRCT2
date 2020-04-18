using UnityEngine;

public static class Maths
{
    /// <summary>
    /// Returns the world angle from one position to the other, while the 
	/// Y-axis is ignored.
    /// </summary>
    public static float GetAngleTo2D(Vector3 from, Vector3 to)
	{
		return ((Mathf.Atan2(to.x - from.x, to.z - from.z) * 180 / Mathf.PI) + 360) % 360;
	}

    /// <summary>
    /// Returns the difference between two world angles.
    /// </summary>
	public static float GetAngleDifference(float angleA, float angleB)
	{
        // Only clock-wise? otherwise Mathf.DeltaAngle
        //float dif = (((((angleA - angleB) % 360) + 540) % 360) - 180); //((angleA - angleB) % 360);
        return ((angleB - angleA) + 360) % 360;
    }

    /// <summary>
    /// Returns the location at a certain distance from the given position, 
	/// with help of a world angle.
    /// </summary>
	public static Vector3 GetPosAtAngle(Vector3 position, float angle, float distance)
	{
		Vector3 newpos = position;
		//float angle_radians = -((angle - 90) * (Mathf.PI / 180));
		newpos.x += (distance * Mathf.Sin(angle * Mathf.Deg2Rad));
		newpos.z += (distance * Mathf.Cos(angle * Mathf.Deg2Rad));
		return newpos;
	}

    /// <summary>
    /// Returns the location (normal) at a certain distance from local position
	/// 0,0,0 , with help of a world angle.
    /// </summary>
	public static Vector3 GetNormalAtAngle(float angle, float distance)
    {
        return new Vector3(
            (distance * Mathf.Sin(angle * Mathf.Deg2Rad)),
            0f,
            (distance * Mathf.Cos(angle * Mathf.Deg2Rad))
        );
    }

    /// <summary>
    /// Checks whether posB is in range of posA, while ignoring the Y-axis.
    /// </summary>
	public static bool InRange2D(Vector3 posA, Vector3 posB, float range)
	{
		float dx = posA.x - posB.x,
			  dz = posA.z - posB.z;
		return (Mathf.Sqrt(dx * dx + dz * dz) <= range);
	}


    /// <summary>
    /// Returns the point closest to target, or Vector3.zero if there are no 
    /// points.
    /// </summary>
    public static Vector3 GetClosestTo(Vector3 target, params Vector3[] points)
    {
        Vector3 closest = Vector3.zero;
        float distance = float.MaxValue;
        for (int i = 0; i < points.Length; i++)
        {
            float current = Vector3.Distance(target, points[i]);
            if (current < distance)
            {
                distance = current;
                closest = points[i];
            }
        }
        return closest;
    }


    /// <summary>
    /// Returns the point furthest from target, or Vector3.zero if there are no 
    /// points.
    /// </summary>
    public static Vector3 GetFurthestFrom(Vector3 target, params Vector3[] points)
    {
        Vector3 closest = Vector3.zero;
        float distance = 0;
        for (int i = 0; i < points.Length; i++)
        {
            float current = Vector3.Distance(target, points[i]);
            if (current > distance)
            {
                distance = current;
                closest = points[i];
            }
        }
        return closest;
    }


    /// <summary>
    /// Gets the center position in between the two specified positions.
    /// </summary>
    public static Vector3 GetCenter(Vector3 left, Vector3 right)
    {
        return (left + right) / 2;
    }


    /// <summary>
    /// Gets the center position in between the two specified positions.
    /// </summary>
    public static Vector2 GetCenter(Vector2 left, Vector2 right)
    {
        return (left + right) / 2;
    }


    /// <summary>
    /// Gets the center position in between all specified positions.
    /// </summary>
    public static Vector3 GetCenter(params Vector3[] positions)
	{
		Vector3 result = Vector3.zero;

		int len = positions.Length;
		for (int i = 0; i < len; i++)
			result += positions[i];

		result.x /= len;
		result.y /= len;
		result.z /= len;

		return result;
    }


    /// <summary>
    /// Gets the center position in between all specified positions.
    /// </summary>
    public static Vector2 GetCenter(params Vector2[] positions)
    {
        Vector2 result = Vector2.zero;

        int len = positions.Length;
        for (int i = 0; i < len; i++)
            result += positions[i];

        result.x /= len;
        result.y /= len;

        return result;
    }


    /// <summary>
    /// Calculates the remainder using the Modulo operation, which makes sure 
    /// negative numbers follow the positive % sign, instead of reversing. 
    /// </summary>
    public static int Modulo(int value, int divider)
	{
		int remainder = value % divider;
		return remainder < 0 ? remainder + divider : remainder;
	}
}
