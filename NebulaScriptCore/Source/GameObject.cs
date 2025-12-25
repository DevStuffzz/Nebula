using System;
using System.Runtime.CompilerServices;

namespace Nebula
{
    /// <summary>
    /// Unity-like GameObject class for entity management
    /// </summary>
    public static class GameObject
    {
        /// <summary>
        /// Finds a GameObject by name
        /// </summary>
        public static ScriptEntity Find(string name)
        {
            uint entityID = InternalCalls.Entity_FindByName(name);
            if (entityID == 0)
                return null;

            ScriptEntity entity = new ScriptEntity();
            entity.ID = entityID;
            return entity;
        }

        /// <summary>
        /// Finds all GameObjects with a specific component type
        /// </summary>
        public static ScriptEntity[] FindObjectsOfType<T>() where T : class
        {
            uint[] entityIDs = InternalCalls.Entity_GetAllEntitiesWithComponent(typeof(T));
            if (entityIDs == null || entityIDs.Length == 0)
                return new ScriptEntity[0];

            ScriptEntity[] entities = new ScriptEntity[entityIDs.Length];
            for (int i = 0; i < entityIDs.Length; i++)
            {
                entities[i] = new ScriptEntity { ID = entityIDs[i] };
            }

            return entities;
        }

        /// <summary>
        /// Finds the first GameObject with a specific component type
        /// </summary>
        public static ScriptEntity FindObjectOfType<T>() where T : class
        {
            uint[] entityIDs = InternalCalls.Entity_GetAllEntitiesWithComponent(typeof(T));
            if (entityIDs == null || entityIDs.Length == 0)
                return null;

            return new ScriptEntity { ID = entityIDs[0] };
        }

        /// <summary>
        /// Instantiates a prefab
        /// </summary>
        public static ScriptEntity Instantiate(ScriptEntity prefab)
        {
            if (prefab == null)
            {
                Console.LogError("Cannot instantiate null prefab");
                return null;
            }

            uint newEntityID = InternalCalls.Entity_Instantiate(prefab.ID);
            if (newEntityID == 0)
                return null;

            return new ScriptEntity { ID = newEntityID };
        }

        /// <summary>
        /// Instantiates a prefab at a specific position and rotation
        /// </summary>
        public static ScriptEntity Instantiate(ScriptEntity prefab, Vector3 position, Vector3 rotation)
        {
            ScriptEntity entity = Instantiate(prefab);
            if (entity != null)
            {
                entity.transform.position = position;
                entity.transform.rotation = rotation;
            }
            return entity;
        }

        /// <summary>
        /// Destroys a GameObject
        /// </summary>
        public static void Destroy(ScriptEntity entity)
        {
            if (entity == null)
            {
                Console.LogWarning("Cannot destroy null entity");
                return;
            }

            InternalCalls.Entity_Destroy(entity.ID);
        }

        /// <summary>
        /// Destroys a GameObject after a delay
        /// </summary>
        public static void Destroy(ScriptEntity entity, float delay)
        {
            // TODO: Implement delayed destruction
            Console.LogWarning("Destroy with delay not yet implemented, destroying immediately");
            Destroy(entity);
        }
    }
}
