using System;

namespace Nebula
{
    public class TransformComponent
    {
        public Vector3 Position;
        public Vector3 Rotation;
        public Vector3 Scale;

        public TransformComponent()
        {
            Position = Vector3.Zero;
            Rotation = Vector3.Zero;
            Scale = Vector3.One;
        }
    }

    public class MeshRendererComponent
    {
        // Add fields as needed
        public MeshRendererComponent()
        {
        }
    }

    public class CameraComponent
    {
        public bool Primary;
        public float FOV;
        public float NearClip;
        public float FarClip;

        public CameraComponent()
        {
            Primary = true;
            FOV = 45.0f;
            NearClip = 0.1f;
            FarClip = 1000.0f;
        }
    }

    public class TagComponent
    {
        public string Tag;

        public TagComponent()
        {
            Tag = "Untagged";
        }
    }

    public class RigidBodyComponent
    {
        public float Mass;
        public bool IsKinematic;

        // Internal field to store the entity ID - do not serialize
        [System.NonSerialized]
        internal uint _entityID;

        public RigidBodyComponent()
        {
            Mass = 1.0f;
            IsKinematic = false;
        }

        public void AddForce(Vector3 force)
        {
            Log.LogInfo($"[RigidBodyComponent.AddForce] START - force={force}, _entityID={_entityID}, this={this?.GetHashCode() ?? 0}");
            
            if (this == null)
            {
                Log.LogError("[RigidBodyComponent.AddForce] ERROR: 'this' is NULL! RigidBodyComponent object itself is null.");
                throw new NullReferenceException("RigidBodyComponent instance is null");
            }
            
            if (_entityID == 0)
            {
                Log.LogError($"[RigidBodyComponent.AddForce] ERROR: _entityID is 0. This component was not properly initialized via GetComponent. Object hash: {this.GetHashCode()}");
                return;
            }
            
            try
            {
                Log.LogInfo($"[RigidBodyComponent.AddForce] Calling InternalCalls.RigidBody_AddForce(_entityID={_entityID}, force={force})");
                InternalCalls.RigidBody_AddForce(_entityID, ref force);
                Log.LogInfo($"[RigidBodyComponent.AddForce] SUCCESS - Force applied to entity {_entityID}");
            }
            catch (NullReferenceException nre)
            {
                Log.LogError($"[RigidBodyComponent.AddForce] NullReferenceException caught!");
                Log.LogError($"  - _entityID: {_entityID}");
                Log.LogError($"  - force: {force}");
                Log.LogError($"  - this: {(this == null ? "NULL" : this.GetHashCode().ToString())}");
                Log.LogError($"  - Exception: {nre.Message}");
                Log.LogError($"  - StackTrace: {nre.StackTrace}");
                throw;
            }
            catch (System.Exception ex)
            {
                Log.LogError($"[RigidBodyComponent.AddForce] Exception: {ex.GetType().Name} - {ex.Message}\\nStack: {ex.StackTrace}");
                throw;
            }
        }

        public void AddForce(Vector3 force, ForceMode mode)
        {
            Log.LogInfo($"AddForce (with mode) called with force={force}, mode={mode}, _entityID={_entityID}");
            
            if (_entityID == 0)
            {
                Log.LogError("RigidBodyComponent: Cannot AddForce - EntityID not set. Make sure to use GetComponent to retrieve the component.");
                return;
            }
            
            try
            {
                InternalCalls.RigidBody_AddForceWithMode(_entityID, ref force, mode);
                Log.LogInfo($"AddForceWithMode completed successfully for entity {_entityID}");
            }
            catch (System.Exception ex)
            {
                Log.LogError($"Exception in AddForceWithMode: {ex.Message}\\nStack: {ex.StackTrace}");
                throw;
            }
        }
    }

    public enum ForceMode
    {
        Force = 0,      // Continuous force (uses mass)
        Impulse = 1,    // Instant force (uses mass)
        VelocityChange = 2, // Instant velocity change (ignores mass)
        Acceleration = 3    // Continuous acceleration (ignores mass)
    }

    public class BoxColliderComponent
    {
        public Vector3 Size;
        public Vector3 Offset;

        public BoxColliderComponent()
        {
            Size = Vector3.One;
            Offset = Vector3.Zero;
        }
    }

    public class SphereColliderComponent
    {
        public float Radius;
        public Vector3 Offset;

        public SphereColliderComponent()
        {
            Radius = 0.5f;
            Offset = Vector3.Zero;
        }
    }

    public class CapsuleColliderComponent
    {
        public float Radius;
        public float Height;
        public Vector3 Offset;

        public CapsuleColliderComponent()
        {
            Radius = 0.5f;
            Height = 2.0f;
            Offset = Vector3.Zero;
        }
    }

    public class AudioSourceComponent
    {
        public bool PlayOnAwake;
        public bool Loop;
        public float Volume;
        public float Pitch;

        public AudioSourceComponent()
        {
            PlayOnAwake = false;
            Loop = false;
            Volume = 1.0f;
            Pitch = 1.0f;
        }
    }

    public class LightComponent
    {
        public enum LightType
        {
            Directional,
            Point,
            Spot
        }

        public LightType Type;
        public Vector3 Color;
        public float Intensity;
        public float Range;

        public LightComponent()
        {
            Type = LightType.Point;
            Color = new Vector3(1.0f, 1.0f, 1.0f);
            Intensity = 1.0f;
            Range = 10.0f;
        }
    }
}
