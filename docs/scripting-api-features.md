# Nebula Scripting API - New Features

## Overview
This document describes all the newly added features to the Nebula scripting system.

## 1. Time Management

Access time-related values through the `Time` class:

```csharp
// Delta time (time since last frame)
float dt = Time.deltaTime;

// Total time elapsed
float elapsed = Time.time;

// Slow motion / speed up
Time.timeScale = 0.5f; // Half speed
Time.timeScale = 2.0f; // Double speed

// Physics timestep
Time.fixedDeltaTime = 0.02f;

// Unscaled time (not affected by timeScale)
float realTime = Time.unscaledTime;
float realDt = Time.unscaledDeltaTime;

// Frame counter
int frames = Time.frameCount;
```

## 2. Enhanced Input System

### Mouse Input
```csharp
// Mouse position (screen coordinates)
Vector2 pos = Input.mousePosition;

// Mouse movement delta
Vector2 delta = Input.mouseDelta;

// Mouse scroll
Vector2 scroll = Input.mouseScrollDelta;

// Mouse buttons
if (Input.IsMouseButtonPressed(MouseButton.Left)) { }
if (Input.IsMouseButtonDown(MouseButton.Right)) { }
if (Input.IsMouseButtonReleased(MouseButton.Middle)) { }
```

### Axis Input
```csharp
// Get mapped axis value (smooth, -1 to 1)
float horizontal = Input.GetAxis("Horizontal");
float vertical = Input.GetAxis("Vertical");

// Get raw axis value (instant, no smoothing)
float h = Input.GetAxisRaw("Horizontal");
```

## 3. Physics & Raycasting

### Raycasting
```csharp
// Simple raycast
if (Physics.Raycast(origin, direction, 100f))
{
    // Hit something
}

// Raycast with hit info
if (Physics.Raycast(origin, direction, out RaycastHit hit, 100f))
{
    Console.Log($"Hit: {hit.entity.name}");
    Console.Log($"Point: {hit.point}");
    Console.Log($"Normal: {hit.normal}");
    Console.Log($"Distance: {hit.distance}");
}

// Ray-based raycast
Ray ray = new Ray(transform.position, transform.forward);
if (Physics.Raycast(ray, out RaycastHit hit))
{
    // Process hit
}

// Sphere cast
if (Physics.SphereCast(origin, radius, direction, out RaycastHit hit, 100f))
{
    // Hit with a sphere
}
```

### Physics Queries
```csharp
// Find all entities in a sphere
ScriptEntity[] entities = Physics.OverlapSphere(position, radius);

// Gravity
Physics.gravity = new Vector3(0, -9.81f, 0);
Vector3 g = Physics.gravity;
```

## 4. Collision Callbacks

Override these methods in your ScriptBehavior:

```csharp
public override void OnCollisionEnter(Collision collision)
{
    Console.Log($"Hit: {collision.entity.name}");
    Console.Log($"Contact: {collision.contactPoint}");
    Console.Log($"Normal: {collision.contactNormal}");
    Console.Log($"Velocity: {collision.relativeVelocity}");
}

public override void OnCollisionStay(Collision collision)
{
    // Called every frame while colliding
}

public override void OnCollisionExit(Collision collision)
{
    // Called when collision ends
}
```

### Trigger Callbacks
```csharp
public override void OnTriggerEnter(ScriptEntity other)
{
    if (other.CompareTag("Player"))
    {
        // Do something
    }
}

public override void OnTriggerStay(ScriptEntity other) { }
public override void OnTriggerExit(ScriptEntity other) { }
```

## 5. Additional Lifecycle Methods

```csharp
public override void OnCreate() { }      // Called once when created
public override void OnUpdate(float dt) { }  // Called every frame
public override void OnFixedUpdate() { }  // Called at fixed timesteps (physics)
public override void LateUpdate() { }     // Called after all Update calls
public override void OnEnable() { }       // Called when entity is enabled
public override void OnDisable() { }      // Called when entity is disabled
public override void OnDestroy() { }      // Called when destroyed
```

## 6. Coroutines

Start and manage coroutines for time-based logic:

```csharp
private IEnumerator MyCoroutine()
{
    Console.Log("Start");
    
    // Wait for seconds
    yield return new WaitForSeconds(2.0f);
    Console.Log("2 seconds later");
    
    // Wait for condition
    yield return new WaitUntil(() => someCondition);
    Console.Log("Condition met");
}

public override void OnCreate()
{
    // Start coroutine
    Coroutine co = StartCoroutine(MyCoroutine());
    
    // Stop coroutine
    StopCoroutine(co);
}
```

## 7. GetScript Methods

Access script components by type or name:

```csharp
// Get by type
PlayerController player = entity.GetScript<PlayerController>();

// Get by class name (string)
ScriptBehavior script = entity.GetScript("PlayerController");

// Get by name with type
PlayerController player = entity.GetScriptByName<PlayerController>("PlayerController");

// Use it to access/modify public variables
if (script != null)
{
    var pc = script as PlayerController;
    pc.moveSpeed = 10.0f; // Modify public variable
}
```

## 8. Inspector Attributes

Make fields editable in the inspector:

```csharp
// Basic serialization
[SerializeField]
private float health = 100f;

// Range slider
[Range(0f, 100f)]
public float volume = 50f;

// Header and tooltip
[Header("Movement Settings")]
[Tooltip("How fast the player moves")]
public float moveSpeed = 5.0f;

// Hide public field
[HideInInspector]
public float internalValue;

// Spacing
[Space(20)]
public string nextField;

// Text area for strings
[TextArea(3, 10)]
public string description;

// Color picker
[ColorUsage(true, false)]
public Vector3 color;

// Read-only display
[ReadOnly]
public int score = 0;
```

## 9. Entity Active State

Control entity activation:

```csharp
// Check if active
if (entity.activeSelf) { }
if (entity.activeInHierarchy) { }

// Set active state
entity.SetActive(false); // Disable
entity.SetActive(true);  // Enable
```

## 10. Transform Hierarchy

Work with parent-child relationships:

```csharp
// Get parent
Transform parent = transform.parent;

// Set parent
transform.SetParent(otherTransform);
transform.SetParent(otherTransform, worldPositionStays: true);

// Get children
int count = transform.childCount;
Transform firstChild = transform.GetChild(0);

// Iterate children
for (int i = 0; i < transform.childCount; i++)
{
    Transform child = transform.GetChild(i);
    // Do something with child
}

// Direction vectors
Vector3 forward = transform.forward;
Vector3 right = transform.right;
Vector3 up = transform.up;
```

## 11. Tag System

Find entities by tag:

```csharp
// Set tag
entity.tag = "Player";

// Get tag
string tag = entity.tag;

// Compare tag
if (entity.CompareTag("Enemy")) { }

// Find by tag
ScriptEntity enemy = GameObject.FindWithTag("Enemy");

// Find all by tag
ScriptEntity[] enemies = GameObject.FindGameObjectsWithTag("Enemy");
```

## 12. Destroy with Delay

```csharp
// Destroy immediately
GameObject.Destroy(entity);

// Destroy after delay
GameObject.Destroy(entity, 2.0f); // Destroy after 2 seconds
```

## 13. Quaternion Support

Proper rotation handling:

```csharp
// Create from Euler angles
Quaternion rot = Quaternion.Euler(0, 45, 0);

// Spherical interpolation
Quaternion result = Quaternion.Slerp(rotA, rotB, t);

// Look at direction
Quaternion lookRot = Quaternion.LookRotation(forward, up);

// Angle-axis rotation
Quaternion axisRot = Quaternion.AngleAxis(45, Vector3.Up);

// Rotate vector
Vector3 rotated = rotation * Vector3.Forward;

// Get Euler angles
Vector3 euler = rotation.eulerAngles;
```

## 14. New Components

Additional component types:

```csharp
// Sphere collider
SphereColliderComponent sphere = entity.GetComponent<SphereColliderComponent>();
sphere.Radius = 1.0f;

// Capsule collider
CapsuleColliderComponent capsule = entity.GetComponent<CapsuleColliderComponent>();
capsule.Radius = 0.5f;
capsule.Height = 2.0f;

// Audio source
AudioSourceComponent audio = entity.GetComponent<AudioSourceComponent>();
audio.Volume = 0.8f;
audio.Loop = true;

// Light
LightComponent light = entity.GetComponent<LightComponent>();
light.Type = LightComponent.LightType.Point;
light.Intensity = 1.0f;
```

## 15. Vector2 (Unity-compatible)

```csharp
Vector2 v = new Vector2(1, 2);

// Properties
float mag = v.magnitude;
Vector2 normalized = v.normalized;

// Static methods
float dist = Vector2.Distance(a, b);
Vector2 lerped = Vector2.Lerp(a, b, t);
Vector2 moved = Vector2.MoveTowards(current, target, maxDelta);
```

## Required C++ Implementation

Note: These features require corresponding C++ internal call implementations in your engine:

### Time Internal Calls
- `GetDeltaTime()`, `GetTime()`, `GetTimeScale()`, `SetTimeScale()`
- `GetFixedDeltaTime()`, `SetFixedDeltaTime()`
- `GetUnscaledTime()`, `GetUnscaledDeltaTime()`, `GetFrameCount()`

### Input Internal Calls
- `GetMousePosition()`, `GetMouseDelta()`, `GetMouseScrollDelta()`
- `GetAxisValue()`, `GetAxisRawValue()`

### Physics Internal Calls
- `InternalRaycast()`, `InternalSphereCast()`, `InternalOverlapSphere()`
- `GetGravity()`, `SetGravity()`

### Entity Internal Calls
- `Entity_GetScriptByName()`, `Entity_GetTag()`, `Entity_SetTag()`
- `Entity_GetActiveSelf()`, `Entity_GetActiveInHierarchy()`, `Entity_SetActive()`
- `Entity_GetParent()`, `Entity_SetParent()`, `Entity_SetParentWithTransform()`
- `Entity_GetChildCount()`, `Entity_GetChild()`
- `Entity_FindByTag()`, `Entity_FindAllByTag()`
- `Entity_DestroyDelayed()`

### Collision Callbacks
The engine must call these methods on ScriptBehavior instances:
- `OnCollisionEnter(Collision)`, `OnCollisionStay(Collision)`, `OnCollisionExit(Collision)`
- `OnTriggerEnter(ScriptEntity)`, `OnTriggerStay(ScriptEntity)`, `OnTriggerExit(ScriptEntity)`
- `OnFixedUpdate()`, `LateUpdate()`, `OnEnable()`, `OnDisable()`
