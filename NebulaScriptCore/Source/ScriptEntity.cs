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

        protected ScriptEntity() { }

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
    }
}
