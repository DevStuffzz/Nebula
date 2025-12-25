# Unity-like Scripting Features for Nebula

This document describes the Unity-like scripting features added to Nebula's C# scripting system.

## Overview

Nebula now supports Unity-style scripting with familiar APIs including:
- GetComponent<T>() for accessing components
- FindObjectsOfType<T>() for scene queries
- GameObject.Instantiate() for prefab instantiation
- GameObject.Find() for finding entities by name
- GameObject.Destroy() for destroying entities

## Component Access

### GetComponent<T>()

Access components on an entity similar to Unity:

```csharp
public class MyScript : ScriptBehavior
{
    public override void OnCreate()
    {
        // Get component from current entity
        var camera = GetComponent<CameraComponent>();
        if (camera != null)
        {
            camera.FOV = 60.0f;
        }

        // Also works on Entity references
        var transform = Entity.GetComponent<TransformComponent>();
    }
}
```

### HasComponent<T>()

Check if an entity has a specific component:

```csharp
if (HasComponent<MeshRendererComponent>())
{
    Console.Log("Entity has a mesh renderer!");
}
```

### AddComponent<T>() / RemoveComponent<T>()

Add or remove components at runtime:

```csharp
// Add a component
var camera = AddComponent<CameraComponent>();

// Remove a component
RemoveComponent<CameraComponent>();
```

## Scene Queries

### FindObjectsOfType<T>()

Find all entities in the scene with a specific component type:

```csharp
public override void OnUpdate(float deltaTime)
{
    // Find all entities with cameras
    ScriptEntity[] cameras = FindObjectsOfType<CameraComponent>();
    
    foreach (var cam in cameras)
    {
        Console.Log($"Found camera: {cam.name}");
    }
}
```

### FindObjectOfType<T>()

Find the first entity with a specific component:

```csharp
ScriptEntity mainCamera = FindObjectOfType<CameraComponent>();
```

### Find(string name)

Find an entity by name:

```csharp
ScriptEntity player = GameObject.Find("Player");
if (player != null)
{
    player.transform.position = new Vector3(0, 0, 0);
}
```

## Prefab System

### GameObject.Instantiate()

Create copies of entities at runtime (prefab system):

```csharp
public class EnemySpawner : ScriptBehavior
{
    private ScriptEntity enemyPrefab;

    public override void OnCreate()
    {
        // Find or assign the prefab entity
        enemyPrefab = GameObject.Find("EnemyTemplate");
    }

    public override void OnUpdate(float deltaTime)
    {
        if (Input.IsKeyPressed(KeyCode.Space))
        {
            // Instantiate at origin
            ScriptEntity enemy = GameObject.Instantiate(enemyPrefab);
            
            // Or instantiate at specific position/rotation
            Vector3 spawnPos = new Vector3(5, 0, 0);
            Vector3 spawnRot = new Vector3(0, 45, 0);
            ScriptEntity enemy2 = GameObject.Instantiate(enemyPrefab, spawnPos, spawnRot);
        }
    }
}
```

### GameObject.Destroy()

Destroy entities at runtime:

```csharp
// Destroy immediately
GameObject.Destroy(enemy);

// Destroy after delay (coming soon)
GameObject.Destroy(enemy, 3.0f);
```

## Entity Properties

### transform

Access the entity's transform component:

```csharp
// Get/Set position
transform.position = new Vector3(1, 2, 3);
Vector3 pos = transform.position;

// Get/Set rotation (Euler angles in degrees)
transform.rotation = new Vector3(0, 90, 0);

// Get/Set scale
transform.scale = new Vector3(2, 2, 2);
```

### name

Get or set the entity's name (from TagComponent):

```csharp
// Get name
string entityName = Entity.name;

// Set name
Entity.name = "Player";
```

## Available Component Types

The following component types are currently supported:

- `TransformComponent` - Position, rotation, scale
- `MeshRendererComponent` - Visual mesh rendering
- `CameraComponent` - Camera settings
- `TagComponent` - Entity name/tag
- `RigidBodyComponent` - Physics body
- `BoxColliderComponent` - Box collision shape

## Example: Complete Player Controller

```csharp
using Nebula;

public class PlayerController : ScriptBehavior
{
    private float moveSpeed = 5.0f;
    private ScriptEntity bulletPrefab;

    public override void OnCreate()
    {
        Console.Log("Player initialized!");
        bulletPrefab = GameObject.Find("BulletTemplate");
    }

    public override void OnUpdate(float deltaTime)
    {
        // Movement
        Vector3 movement = Vector3.Zero;
        
        if (Input.IsKeyDown(KeyCode.W))
            movement.z -= moveSpeed * deltaTime;
        if (Input.IsKeyDown(KeyCode.S))
            movement.z += moveSpeed * deltaTime;
        if (Input.IsKeyDown(KeyCode.A))
            movement.x -= moveSpeed * deltaTime;
        if (Input.IsKeyDown(KeyCode.D))
            movement.x += moveSpeed * deltaTime;
        
        transform.position += movement;

        // Shooting
        if (Input.IsKeyPressed(KeyCode.Space))
        {
            ShootBullet();
        }

        // Find and interact with enemies
        if (Input.IsKeyPressed(KeyCode.E))
        {
            ScriptEntity[] enemies = FindObjectsOfType<MeshRendererComponent>();
            if (enemies.Length > 0)
            {
                Vector3 enemyPos = enemies[0].transform.position;
                Console.Log($"Nearest enemy at: {enemyPos}");
            }
        }
    }

    private void ShootBullet()
    {
        if (bulletPrefab != null)
        {
            Vector3 spawnPos = transform.position + new Vector3(0, 0, -1);
            ScriptEntity bullet = GameObject.Instantiate(bulletPrefab, spawnPos, Vector3.Zero);
            bullet.name = "Bullet";
        }
    }
}
```

## ScriptBehavior Base Class

Your scripts should inherit from `ScriptBehavior` which provides:

- `OnCreate()` - Called when the script is initialized
- `OnUpdate(float deltaTime)` - Called every frame
- `OnDestroy()` - Called when the entity is destroyed
- `Entity` - Reference to the owning entity
- `transform` - Shortcut to entity's transform
- All the component and scene query methods listed above

## Tips

1. **Prefab Workflow**: Create template entities in your scene and name them appropriately (e.g., "EnemyTemplate"). Use `GameObject.Find()` to reference them and `Instantiate()` to spawn copies.

2. **Component Queries**: Use `FindObjectsOfType<T>()` sparingly in Update() as it queries the entire scene. Cache results when possible.

3. **Null Checks**: Always check if `GetComponent<T>()` or `Find()` return null before using the result.

4. **Transform Access**: Use the `transform` property for quick access to position, rotation, and scale rather than calling `GetComponent<TransformComponent>()` repeatedly.

## Future Enhancements

Planned features:
- GameObject.FindWithTag() for tag-based queries
- Delayed destruction support
- Component serialization for saving prefab data
- Parent-child entity hierarchies
- More component types (Light, AudioSource, etc.)
