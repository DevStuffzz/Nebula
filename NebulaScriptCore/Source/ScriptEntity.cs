using System.Runtime.CompilerServices;

namespace Nebula
{
    public class Transform
    {
        private uint entityID;

        internal Transform(uint id)
        {
            entityID = id;
        }

        public Vector3 position
        {
            get
            {
                InternalCalls.Entity_GetPosition(entityID, out Vector3 pos);
                return pos;
            }
            set
            {
                InternalCalls.Entity_SetPosition(entityID, ref value);
            }
        }

        public Vector3 rotation
        {
            get
            {
                InternalCalls.Entity_GetRotation(entityID, out Vector3 rot);
                return rot;
            }
            set
            {
                InternalCalls.Entity_SetRotation(entityID, ref value);
            }
        }

        public Vector3 scale
        {
            get
            {
                InternalCalls.Entity_GetScale(entityID, out Vector3 scl);
                return scl;
            }
            set
            {
                InternalCalls.Entity_SetScale(entityID, ref value);
            }
        }
    }

    public class ScriptEntity
    {
        public uint ID { get; internal set; }

        private Transform _transform;
        public Transform transform
        {
            get
            {
                if (_transform == null)
                    _transform = new Transform(ID);
                return _transform;
            }
        }

        public string name
        {
            get { return InternalCalls.Entity_GetName(ID); }
            set { InternalCalls.Entity_SetName(ID, value); }
        }

        protected ScriptEntity() { }

        // Unity-like component access methods
        public T GetComponent<T>() where T : class, new()
        {
            if (!HasComponent<T>())
                return null;

            T component = new T();
            if (!InternalCalls.Entity_GetComponent(ID, typeof(T), component))
                return null;

            return component;
        }

        public bool HasComponent<T>() where T : class
        {
            return InternalCalls.Entity_HasComponent(ID, typeof(T));
        }

        public T AddComponent<T>() where T : class, new()
        {
            if (HasComponent<T>())
            {
                Console.LogWarning($"Entity already has component of type {typeof(T).Name}");
                return GetComponent<T>();
            }

            InternalCalls.Entity_AddComponent(ID, typeof(T));
            return GetComponent<T>();
        }

        public void RemoveComponent<T>() where T : class
        {
            if (!HasComponent<T>())
            {
                Console.LogWarning($"Entity does not have component of type {typeof(T).Name}");
                return;
            }

            InternalCalls.Entity_RemoveComponent(ID, typeof(T));
        }

        // Legacy methods for backward compatibility
        public Vector3 GetPosition()
        {
            InternalCalls.Entity_GetPosition(ID, out Vector3 position);
            return position;
        }

        public void SetPosition(Vector3 position)
        {
            InternalCalls.Entity_SetPosition(ID, ref position);
        }

        public Vector3 GetRotation()
        {
            InternalCalls.Entity_GetRotation(ID, out Vector3 rotation);
            return rotation;
        }

        public void SetRotation(Vector3 rotation)
        {
            InternalCalls.Entity_SetRotation(ID, ref rotation);
        }

        public Vector3 GetScale()
        {
            InternalCalls.Entity_GetScale(ID, out Vector3 scale);
            return scale;
        }

        public void SetScale(Vector3 scale)
        {
            InternalCalls.Entity_SetScale(ID, ref scale);
        }
    }

    internal static class InternalCalls
    {
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

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_HasComponent(uint entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern bool Entity_GetComponent(uint entityID, Type componentType, object outComponent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_AddComponent(uint entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_RemoveComponent(uint entityID, Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern string Entity_GetName(uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_SetName(uint entityID, string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint[] Entity_GetAllEntitiesWithComponent(Type componentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_FindByName(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern uint Entity_Instantiate(uint prefabID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Entity_Destroy(uint entityID);
    }
}
