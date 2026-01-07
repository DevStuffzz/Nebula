using Nebula;

namespace ExampleProject
{
    public class TopDownController : ScriptBehavior
    {
        [SerializeField]
        public float Speed = 5.0f;
        [SerializeField]
        public float JumpForce = 5.0f;

        [SerializeField]
        public float MaxSpeed = 6.0f;

        [SerializeField]
        public float Drag = 8.0f;


        [SerializeField]
        RigidBodyComponent rb; 

        public override void OnCreate()
        {
            Log.LogInfo("TopDownController created!");
            rb = GetComponent<RigidBodyComponent>();
        }

        public override void OnUpdate(float deltaTime)
        {
            Vector3 inputDir = Vector3.Zero;

            // Use world-space directions for top-down movement
            Vector3 forward = Vector3.Forward;  // World forward (0, 0, 1)
            Vector3 right = Vector3.Right;      // World right (1, 0, 0)

            // Movement input
            if (Input.IsKeyDown(KeyCode.W))
                inputDir += forward;
            if (Input.IsKeyDown(KeyCode.S))
                inputDir -= forward;
            if (Input.IsKeyDown(KeyCode.A))
                inputDir -= right;
            if (Input.IsKeyDown(KeyCode.D))
                inputDir += right;

            if (inputDir.Length() > 0.0f)
                inputDir = inputDir.Normalized();

            Vector3 velocity = rb.velocity;

            // Horizontal velocity only (XZ plane)
            Vector3 horizontalVel = new Vector3(velocity.X, 0.0f, velocity.Z);

            // Desired velocity
            Vector3 targetVel = inputDir * MaxSpeed;

            // Difference between current and target velocity
            Vector3 velocityDelta = targetVel - horizontalVel; 

            // Accelerate toward target velocity
            rb.AddForce(velocityDelta * Speed, ForceMode.Force);

            // Counter movement when no input
            if (inputDir.Length() == 0.0f)
            {
                rb.AddForce(-horizontalVel * Drag, ForceMode.Force);
            }

            // Jump
            if (Input.IsKeyPressed(KeyCode.Space))
            {
                rb.AddForce(Vector3.Up * JumpForce, ForceMode.Impulse);
            }
        }


    }
}
