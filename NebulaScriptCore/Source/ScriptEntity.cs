using System.Runtime.CompilerServices;

namespace Nebula
{
    public class ScriptEntity
    {
        public uint ID { get; internal set; }

        protected ScriptEntity() { }

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
