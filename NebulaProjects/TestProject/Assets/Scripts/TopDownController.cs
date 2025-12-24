using Nebula;

namespace MyGame
{
    public class TopDownController : ScriptBehavior
    {
        public float Speed = 5.0f;

        public override void OnCreate()
        {
            Console.Log("TopDownController created!");
            Console.Log("Testing Console.Log from OnCreate");
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

            // Apply movement using transform property
            if (movement.X != 0 || movement.Z != 0)
            {
       
                Vector3 currentPos = transform.position;
                
                Vector3 scaledMovement = movement * Speed * deltaTime;
                
                Vector3 newPos = currentPos + scaledMovement;
                
                transform.position = newPos;
            }
        }
    }
}
