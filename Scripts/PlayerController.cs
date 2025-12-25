using Nebula;
using System;

// Example Unity-like script demonstrating the new features
public class PlayerController : ScriptBehavior
{
    private float moveSpeed = 5.0f;
    private ScriptEntity enemyPrefab;

    public override void OnCreate()
    {
        Console.Log("PlayerController created!");
        
        // Find an object by name (Unity-like)
        ScriptEntity camera = Find("MainCamera");
        if (camera != null)
        {
            Console.Log("Found main camera!");
        }

        // Get component (Unity-like)
        var cameraComp = GetComponent<CameraComponent>();
        if (cameraComp != null)
        {
            Console.Log("Player has a camera component");
        }

        // Find objects of type (Unity-like)
        ScriptEntity[] allCameras = FindObjectsOfType<CameraComponent>();
        Console.Log($"Found {allCameras.Length} cameras in the scene");
    }

    public override void OnUpdate(float deltaTime)
    {
        // Movement using Unity-like Input API
        Vector3 movement = Vector3.Zero;

        if (Input.IsKeyDown(KeyCode.W))
            movement.z -= moveSpeed * deltaTime;
        
        if (Input.IsKeyDown(KeyCode.S))
            movement.z += moveSpeed * deltaTime;
        
        if (Input.IsKeyDown(KeyCode.A))
            movement.x -= moveSpeed * deltaTime;
        
        if (Input.IsKeyDown(KeyCode.D))
            movement.x += moveSpeed * deltaTime;

        // Update position (Unity-like transform access)
        transform.position += movement;

        // Spawn enemy on space key (Unity-like Instantiate)
        if (Input.IsKeyPressed(KeyCode.Space) && enemyPrefab != null)
        {
            Vector3 spawnPos = transform.position + new Vector3(0, 0, -5);
            ScriptEntity enemy = Instantiate(enemyPrefab, spawnPos, Vector3.Zero);
            Console.Log("Spawned enemy!");
        }

        // Find and destroy nearest enemy on key press
        if (Input.IsKeyPressed(KeyCode.E))
        {
            ScriptEntity[] enemies = FindObjectsOfType<MeshRendererComponent>();
            if (enemies.Length > 0)
            {
                // Destroy first enemy found
                Destroy(enemies[0]);
                Console.Log("Destroyed an enemy!");
            }
        }
    }

    public override void OnDestroy()
    {
        Console.Log("PlayerController destroyed!");
    }
}
