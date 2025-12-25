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
}
