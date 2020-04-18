using System;
using UnityEngine;

namespace OpenRCT2.Unity
{
    [RequireComponent(typeof(Map))]
    public class PeepController : MonoBehaviour
    {
        [SerializeField] GameObject peepPrefab;


        Map map;


        void Start()
        {
            map = GetComponent<Map>();

            Peep[] peeps = OpenRCT2.GetAllPeeps();

            for (int i = 0; i < peeps.Length; i++)
            {
                //SpawnPeep(i, ref peeps[i]);
            }
        }


        GameObject SpawnPeep(int number, ref Peep peep)
        {
            Vector3 position = Map.CoordsToVector3(peep.x, peep.z, peep.y);

            Debug.Log($"Spawn peep: {peep.x}, {peep.y}, {peep.z}");

            GameObject peepObj = Instantiate(peepPrefab, position, Quaternion.identity, transform);
            //peepObj.name = $"Peep {number}";
            return peepObj;
        }
    }
}
