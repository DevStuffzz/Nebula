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
                Log.LogError("Cannot instantiate null prefab");
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
                Log.LogWarning("Cannot destroy null entity");
                return;
            }

            InternalCalls.Entity_Destroy(entity.ID);
        }

        /// <summary>
        /// Destroys a GameObject after a delay
        /// </summary>
        public static void Destroy(ScriptEntity entity, float delay)
        {
            if (entity == null)
            {
                Log.LogWarning("Cannot destroy null entity");
                return;
            }

            InternalCalls.Entity_DestroyDelayed(entity.ID, delay);
        }

        /// <summary>
        /// Finds a GameObject by tag
        /// </summary>
        public static ScriptEntity FindWithTag(string tag)
        {
            uint entityID = InternalCalls.Entity_FindByTag(tag);
            if (entityID == 0)
                return null;

            return new ScriptEntity { ID = entityID };
        }

        /// <summary>
        /// Finds all GameObjects with the specified tag
        /// </summary>
        public static ScriptEntity[] FindGameObjectsWithTag(string tag)
        {
            uint[] entityIDs = InternalCalls.Entity_FindAllByTag(tag);
            if (entityIDs == null || entityIDs.Length == 0)
                return new ScriptEntity[0];

            ScriptEntity[] entities = new ScriptEntity[entityIDs.Length];
            for (int i = 0; i < entityIDs.Length; i++)
            {
                entities[i] = new ScriptEntity { ID = entityIDs[i] };
            }

            return entities;
        }
    }

    internal static class InternalCalls
    {
        // Transform methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetPosition(uint entityID, out Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetPosition(uint entityID, ref Vector3 position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetRotation(uint entityID, out Vector3 rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetRotation(uint entityID, ref Vector3 rotation);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_GetScale(uint entityID, out Vector3 scale);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetScale(uint entityID, ref Vector3 scale);

        // Component methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(uint entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_GetComponent(uint entityID, Type componentType, object outComponent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_AddComponent(uint entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_RemoveComponent(uint entityID, Type componentType);

        // Entity management
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Entity_GetName(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetName(uint entityID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint[] Entity_GetAllEntitiesWithComponent(Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_FindByName(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_FindByTag(string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint[] Entity_FindAllByTag(string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_Instantiate(uint prefabID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_Destroy(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_DestroyDelayed(uint entityID, float delay);

        // Script methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Entity_GetScriptInstance(uint entityID, Type scriptType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern object Entity_GetScriptByName(uint entityID, string className);

        // Tag/Active methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Entity_GetTag(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetTag(uint entityID, string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_GetActiveSelf(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_GetActiveInHierarchy(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetActive(uint entityID, bool active);

        // Hierarchy methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_GetParent(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetParent(uint entityID, uint parentID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetParentWithTransform(uint entityID, uint parentID, bool worldPositionStays);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int Entity_GetChildCount(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_GetChild(uint entityID, int index);

        // RigidBody methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void RigidBody_AddForce(uint entityID, ref Vector3 force);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void RigidBody_AddForceWithMode(uint entityID, ref Vector3 force, ForceMode mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void RigidBody_GetVelocity(uint entityID, out Vector3 velocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void RigidBody_SetVelocity(uint entityID, ref Vector3 velocity);

        // AudioSource methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void AudioSource_Play(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void AudioSource_Stop(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void AudioSource_Pause(uint entityID);

        // LineRenderer methods
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void LineRenderer_SetPoints(uint entityID, Vector3[] points, int count);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void LineRenderer_AddPoint(uint entityID, ref Vector3 point);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void LineRenderer_ClearPoints(uint entityID);
    }
}
