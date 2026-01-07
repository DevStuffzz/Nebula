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

        public Vector3 velocity
        {
            get
            {
                if (_entityID == 0)
                {
                    Log.LogError("RigidBodyComponent: Cannot get velocity - EntityID not set");
                    return Vector3.Zero;
                }
                InternalCalls.RigidBody_GetVelocity(_entityID, out Vector3 vel);
                return vel;
            }
            set
            {
                if (_entityID == 0)
                {
                    Log.LogError("RigidBodyComponent: Cannot set velocity - EntityID not set");
                    return;
                }
                InternalCalls.RigidBody_SetVelocity(_entityID, ref value);
            }
        }

        public void AddForce(Vector3 force)
        {
            if (_entityID == 0)
            {
                Log.LogError("RigidBodyComponent: Cannot AddForce - EntityID not set");
                return;
            }
            
            InternalCalls.RigidBody_AddForce(_entityID, ref force);
        }

        public void AddForce(Vector3 force, ForceMode mode)
        {
            if (_entityID == 0)
            {
                Log.LogError("RigidBodyComponent: Cannot AddForce - EntityID not set");
                return;
            }
            
            InternalCalls.RigidBody_AddForceWithMode(_entityID, ref force, mode);
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
        public float Volume;
        public float Pitch;
        public bool Loop;
        public bool PlayOnAwake;
        public bool Spatial;

        [System.NonSerialized]
        internal uint _entityID;

        public AudioSourceComponent()
        {
            Volume = 1.0f;
            Pitch = 1.0f;
            Loop = false;
            PlayOnAwake = false;
            Spatial = true;
        }

        public void Play()
        {
            if (_entityID == 0)
            {
                Log.LogError("AudioSourceComponent: Cannot Play - EntityID not set.");
                return;
            }
            InternalCalls.AudioSource_Play(_entityID);
        }

        public void Stop()
        {
            if (_entityID == 0)
            {
                Log.LogError("AudioSourceComponent: Cannot Stop - EntityID not set.");
                return;
            }
            InternalCalls.AudioSource_Stop(_entityID);
        }

        public void Pause()
        {
            if (_entityID == 0)
            {
                Log.LogError("AudioSourceComponent: Cannot Pause - EntityID not set.");
                return;
            }
            InternalCalls.AudioSource_Pause(_entityID);
        }
    }

    public class LineRendererComponent
    {
        public Vector3 Color;
        public float Width;
        public bool Loop;

        [System.NonSerialized]
        internal uint _entityID;

        public LineRendererComponent()
        {
            Color = new Vector3(1.0f, 1.0f, 1.0f);
            Width = 1.0f;
            Loop = false;
        }

        public void SetPoints(Vector3[] points)
        {
            if (_entityID == 0)
            {
                Log.LogError("LineRendererComponent: Cannot SetPoints - EntityID not set.");
                return;
            }
            InternalCalls.LineRenderer_SetPoints(_entityID, points, points.Length);
        }

        public void AddPoint(Vector3 point)
        {
            if (_entityID == 0)
            {
                Log.LogError("LineRendererComponent: Cannot AddPoint - EntityID not set.");
                return;
            }
            InternalCalls.LineRenderer_AddPoint(_entityID, ref point);
        }

        public void ClearPoints()
        {
            if (_entityID == 0)
            {
                Log.LogError("LineRendererComponent: Cannot ClearPoints - EntityID not set.");
                return;
            }
            InternalCalls.LineRenderer_ClearPoints(_entityID);
        }
    }

    public class PointLightComponent
    {
        public Vector3 Position;
        public Vector3 Color;
        public float Intensity;
        public float Radius;

        public PointLightComponent()
        {
            Position = Vector3.Zero;
            Color = new Vector3(1.0f, 1.0f, 1.0f);
            Intensity = 1.0f;
            Radius = 5.0f;
        }
    }

    public class DirectionalLightComponent
    {
        public Vector3 Color;
        public float Intensity;

        public DirectionalLightComponent()
        {
            Color = new Vector3(1.0f, 1.0f, 1.0f);
            Intensity = 1.0f;
        }
    }
}
