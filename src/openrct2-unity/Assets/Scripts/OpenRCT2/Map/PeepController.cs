using System.Collections.Generic;
using UnityEngine;

namespace OpenRCT2.Unity
{
    /// <summary>
    /// Controller which moves and updates all the peeps in the park.
    /// </summary>
    [RequireComponent(typeof(Map))]
    public class PeepController : MonoBehaviour
    {
        [SerializeField] GameObject peepPrefab;


        const int MaxPeeps = 8000;


        Peep[] peepBuffer;
        Dictionary<ushort, PeepObject> peepObjects;
        int currentUpdateTick;


        class PeepObject
        {
            public GameObject gameObject;
            public float timeSinceStart;
            public int lastUpdate;
            public Vector3 from;
            public Vector3 towards; 
        }


        void Start()
        {
            peepBuffer = new Peep[MaxPeeps];
            int amount = OpenRCT2.GetAllPeeps(peepBuffer);
            peepObjects = new Dictionary<ushort, PeepObject>(amount);
            Debug.Log("This guest is a" + peepBuffer[1].type);
            for (int i = 0; i < amount; i++)
            {
                AddPeep(ref peepBuffer[i]);
            }
        }


        void FixedUpdate()
        {
            currentUpdateTick++;

            int amount = OpenRCT2.GetAllPeeps(peepBuffer);

            for (int i = 0; i < amount; i++)
            {
                SetPeepPositions(ref peepBuffer[i]);
            }
        }


        void LateUpdate()
        {
            foreach (var peep in peepObjects.Values)
            {
                UpdatePeepPosition(peep);
            }
        }


        /// <summary>
        /// Adds a new peep object to the dictionary.
        /// </summary>
        PeepObject AddPeep(ref Peep peep)
        {
            ushort id = peep.Id;
            var type = peep.type;

            GameObject peepObj = Instantiate(peepPrefab, Vector3.zero, Quaternion.identity, transform);
            peepObj.name = $"{type} {id}";

            Vector3 position = Map.CoordsToVector3(peep.Position);

            PeepObject instance = new PeepObject
            {
                gameObject = peepObj,
                from = position,
                towards = position
            };

            peepObjects.Add(peep.Id, instance);
            return instance;
        }


        /// <summary>
        /// Sets the new start and end positions for this game tick.
        /// </summary>
        void SetPeepPositions(ref Peep peep)
        {
            ushort id = peep.Id;

            if (!peepObjects.TryGetValue(id, out PeepObject obj))
                obj = AddPeep(ref peep);

            obj.lastUpdate = currentUpdateTick;

            Vector3 target = Map.CoordsToVector3(peep.Position);

            if (obj.towards == target)
                return;

            obj.from = obj.towards;
            obj.towards = target;
            obj.timeSinceStart = Time.timeSinceLevelLoad;
        }


        /// <summary>
        /// Updates the actual object position by lerping between the information
        /// from last game tick.
        /// </summary>
        void UpdatePeepPosition(PeepObject obj)
        {
            if (obj.lastUpdate < currentUpdateTick)
            {
                DisablePeep(obj);
                return;
            }
            else
                EnablePeep(obj);

            Transform transf = obj.gameObject.transform;

            if (transf.position == obj.towards)
                return;

            // Update position
            float time = (Time.timeSinceLevelLoad - obj.timeSinceStart) / Time.fixedDeltaTime;
            Vector3 lerped = Vector3.Lerp(transf.position, obj.towards, time);

            transf.position = lerped;

            // Update rotation
            Vector3 forward = new Vector3(obj.from.x - obj.towards.x, 0, obj.from.z - obj.towards.z);

            if (forward != Vector3.zero)
                transf.rotation = Quaternion.LookRotation(forward);
        }


        void EnablePeep(PeepObject obj)
        {
            if (!obj.gameObject.activeSelf)
                obj.gameObject.SetActive(true);
        }


        void DisablePeep(PeepObject obj)
        {
            if (obj.gameObject.activeSelf)
                obj.gameObject.SetActive(false);
        }
    }
}
