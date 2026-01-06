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

        public RigidBodyComponent()
        {
            Mass = 1.0f;
            IsKinematic = false;
        }
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
