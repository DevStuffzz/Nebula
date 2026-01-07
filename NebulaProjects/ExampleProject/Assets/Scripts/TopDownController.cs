using Nebula;

namespace ExampleProject
{
    public class TopDownController : ScriptBehavior
    {
        [SerializeField]
        public float Speed = 5.0f;

        RigidBodyComponent rb;

        public override void OnCreate()
        {
            Log.LogInfo("TopDownController created!");
            rb = GetComponent<RigidBodyComponent>();
        }

        public override void OnUpdate(float deltaTime)
        {
            Vector3 movement = Vector3.Zero;

            // WASD movement
            if (Input.IsKeyDown(KeyCode.W))
                movement.Z -= 1.0f;
            if (Input.IsKeyDown(KeyCode.S))
                movement.Z += 1.0f;
            if (Input.IsKeyDown(KeyCode.A))
                movement.X -= 1.0f;
            if (Input.IsKeyDown(KeyCode.D))
                movement.X += 1.0f;

            // Apply movement
            if (movement.X != 0 || movement.Z != 0)
            {
                if (rb != null)
                    rb.AddForce(movement * Speed, ForceMode.Force);
                else
                    Log.LogError("Error: RigidBody is null!");
            }
        }

        public override void OnCollisionEnter(Collision collision)
        {
            Log.LogInfo("Collision Enter");
            Log.LogInfo($"Collided with {collision.entity.name}");
            Log.LogInfo($"Contact point: {collision.contactPoint}");
            Log.LogInfo($"Impact velocity: {collision.relativeVelocity}");
        }
    }
}
