using Nebula;
using System;
using System.Runtime.Serialization;

namespace ExampleProject
{
    class CameraController : ScriptBehavior
    {
        public float Sensitivity = -8.0f;
        public float MinVerticalAngle = -89.0f;
        public float MaxVerticalAngle = 89.0f;

        private float pitch = 0.0f; // Up/Down rotation
        private float yaw = 0.0f;   // Left/Right rotation

        private ScriptEntity player;
        public override void OnCreate()
        {
            // Lock and hide the cursor for FPS controls
            Cursor.lockMode = CursorLockMode.Locked;
            Cursor.visible = false;

            

            // Initialize rotation from current transform
            Vector3 currentRotation = transform.rotation;
            pitch = currentRotation.X;
            yaw = currentRotation.Y;

            player = GameObject.Find("Player");
        }

        public override void OnUpdate(float deltaTime)
        {
            Vector2 mouseDelta = Input.mouseDelta;

            yaw += mouseDelta.x * Sensitivity * deltaTime;
            pitch += mouseDelta.y * Sensitivity * deltaTime;

            pitch = MathUtil.Clamp(pitch, MinVerticalAngle, MaxVerticalAngle);

            // Camera rotation (pitch + yaw)
            transform.rotation = new Vector3(pitch, yaw, 0.0f);

            // Player rotation ONLY on Y axis (yaw for left/right)
            player.transform.rotation = new Vector3(
                player.transform.rotation.X,
                yaw,
                player.transform.rotation.Z
            );

            // Camera follows player
            transform.position = player.transform.position + new Vector3(0.0f, 0.5f, 0.0f);

            if(Input.IsKeyPressed(KeyCode.Escape))
            {
                Nebula.Application.Exit(0);
            }
        }

    }
}