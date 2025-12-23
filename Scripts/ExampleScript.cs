using Nebula;

namespace GameScripts
{
    public class ExampleScript : ScriptBehavior
    {
        public float MoveSpeed = 2.0f;
        public bool EnableMovement = true;

        private float time = 0.0f;

        public override void OnCreate()
        {
            Console.Log("Example Script initialized!");
            Vector3 pos = Entity.GetPosition();
            Console.Log($"Starting position: {pos}");
        }

        public override void OnUpdate(float deltaTime)
        {
            time += deltaTime;

            if (EnableMovement)
            {
                Vector3 pos = Entity.GetPosition();

                // Oscillate up and down
                pos.Y = (float)System.Math.Sin(time * MoveSpeed) * 2.0f;

                Entity.SetPosition(pos);
            }

            // Example input handling
            if (Input.IsKeyPressed(KeyCode.Space))
            {
                Console.Log("Space key pressed!");
            }
        }

        public override void OnDestroy()
        {
            Console.Log("Example Script destroyed");
        }
    }
}
