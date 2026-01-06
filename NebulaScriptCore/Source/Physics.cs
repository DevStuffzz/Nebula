using System.Runtime.CompilerServices;

namespace Nebula
{
    /// <summary>
    /// Information about a raycast hit
    /// </summary>
    public struct RaycastHit
    {
        /// <summary>
        /// The entity that was hit
        /// </summary>
        public ScriptEntity entity;

        /// <summary>
        /// The impact point in world space
        /// </summary>
        public Vector3 point;

        /// <summary>
        /// The normal of the surface the ray hit
        /// </summary>
        public Vector3 normal;

        /// <summary>
        /// The distance from the ray's origin to the impact point
        /// </summary>
        public float distance;
    }

    /// <summary>
    /// Collision information passed to collision callback functions
    /// </summary>
    public class Collision
    {
        /// <summary>
        /// The entity involved in the collision
        /// </summary>
        public ScriptEntity entity { get; internal set; }

        /// <summary>
        /// The relative linear velocity of the two colliding objects
        /// </summary>
        public Vector3 relativeVelocity { get; internal set; }

        /// <summary>
        /// The first contact point of the collision
        /// </summary>
        public Vector3 contactPoint { get; internal set; }

        /// <summary>
        /// The normal of the contact point
        /// </summary>
        public Vector3 contactNormal { get; internal set; }

        internal Collision() { }
    }

    /// <summary>
    /// Unity-like Physics class for raycasting and physics queries
    /// </summary>
    public static class Physics
    {
        /// <summary>
        /// Global gravity value
        /// </summary>
        public static Vector3 gravity
        {
            get
            {
                GetGravity(out Vector3 g);
                return g;
            }
            set
            {
                SetGravity(ref value);
            }
        }

        /// <summary>
        /// Casts a ray against all colliders in the scene
        /// </summary>
        /// <param name="origin">The starting point of the ray in world coordinates</param>
        /// <param name="direction">The direction of the ray</param>
        /// <param name="maxDistance">The max distance the ray should check for collisions</param>
        /// <returns>True if the ray hits a collider</returns>
        public static bool Raycast(Vector3 origin, Vector3 direction, float maxDistance = float.MaxValue)
        {
            return InternalRaycast(ref origin, ref direction, maxDistance, out RaycastHit hit);
        }

        /// <summary>
        /// Casts a ray against all colliders in the scene
        /// </summary>
        /// <param name="origin">The starting point of the ray in world coordinates</param>
        /// <param name="direction">The direction of the ray</param>
        /// <param name="hitInfo">If true is returned, hitInfo will contain more information about where the ray hit</param>
        /// <param name="maxDistance">The max distance the ray should check for collisions</param>
        /// <returns>True if the ray hits a collider</returns>
        public static bool Raycast(Vector3 origin, Vector3 direction, out RaycastHit hitInfo, float maxDistance = float.MaxValue)
        {
            return InternalRaycast(ref origin, ref direction, maxDistance, out hitInfo);
        }

        /// <summary>
        /// Casts a ray from a point in space with a direction
        /// </summary>
        public static bool Raycast(Ray ray, float maxDistance = float.MaxValue)
        {
            return InternalRaycast(ref ray.origin, ref ray.direction, maxDistance, out RaycastHit hit);
        }

        /// <summary>
        /// Casts a ray from a point in space with a direction
        /// </summary>
        public static bool Raycast(Ray ray, out RaycastHit hitInfo, float maxDistance = float.MaxValue)
        {
            return InternalRaycast(ref ray.origin, ref ray.direction, maxDistance, out hitInfo);
        }

        /// <summary>
        /// Casts a sphere along a ray and returns detailed information on what was hit
        /// </summary>
        public static bool SphereCast(Vector3 origin, float radius, Vector3 direction, out RaycastHit hitInfo, float maxDistance = float.MaxValue)
        {
            return InternalSphereCast(ref origin, radius, ref direction, maxDistance, out hitInfo);
        }

        /// <summary>
        /// Returns all colliders that overlap with a sphere
        /// </summary>
        public static ScriptEntity[] OverlapSphere(Vector3 position, float radius)
        {
            uint[] entityIDs = InternalOverlapSphere(ref position, radius);
            if (entityIDs == null || entityIDs.Length == 0)
                return new ScriptEntity[0];

            ScriptEntity[] entities = new ScriptEntity[entityIDs.Length];
            for (int i = 0; i < entityIDs.Length; i++)
            {
                entities[i] = new ScriptEntity { ID = entityIDs[i] };
            }
            return entities;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool InternalRaycast(ref Vector3 origin, ref Vector3 direction, float maxDistance, out RaycastHit hitInfo);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool InternalSphereCast(ref Vector3 origin, float radius, ref Vector3 direction, float maxDistance, out RaycastHit hitInfo);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern uint[] InternalOverlapSphere(ref Vector3 position, float radius);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetGravity(out Vector3 gravity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetGravity(ref Vector3 gravity);
    }

    /// <summary>
    /// Representation of rays
    /// </summary>
    public struct Ray
    {
        public Vector3 origin;
        public Vector3 direction;

        public Ray(Vector3 origin, Vector3 direction)
        {
            this.origin = origin;
            this.direction = direction.Normalized();
        }

        public Vector3 GetPoint(float distance)
        {
            return origin + direction * distance;
        }
    }
}
