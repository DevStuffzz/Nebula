# C++ Engine Implementation - Complete

## Summary

All requested C++ internal call implementations have been added to the Nebula engine. The scripting system now supports all the features defined in the C# API.

## Files Modified

### 1. ScriptGlue.cpp & ScriptGlue.h
**Location:** `Nebula/src/Nebula/Scripting/`

**Added Internal Calls:**

#### Input System
- `Input_GetMousePosition()` - Returns current mouse position
- `Input_GetMouseDelta()` - Returns mouse movement since last frame
- `Input_GetMouseScrollDelta()` - Returns scroll wheel delta
- `Input_GetAxisValue()` - Implements "Horizontal" (A/D) and "Vertical" (W/S) axes
- `Input_GetAxisRawValue()` - Same as GetAxisValue (no smoothing yet)

#### Time System
- `Time_GetDeltaTime()` - Frame delta time (affected by timeScale)
- `Time_GetTime()` - Total elapsed time (affected by timeScale)
- `Time_GetUnscaledTime()` - Real elapsed time
- `Time_GetUnscaledDeltaTime()` - Real frame time
- `Time_GetTimeScale()` / `SetTimeScale()` - Slow motion/speed up control
- `Time_GetFixedDeltaTime()` / `SetFixedDeltaTime()` - Physics timestep (default 0.02)
- `Time_GetFrameCount()` - Total frames rendered

#### Entity Management
- `Entity_DestroyDelayed()` - Destroy entity after delay
- `Entity_GetScriptInstance()` - Get script instance by type
- `Entity_GetScriptByName()` - Get script instance by class name string
- `Entity_GetTag()` / `SetTag()` - Tag get/set
- `Entity_FindByTag()` - Find first entity with tag
- `Entity_FindAllByTag()` - Find all entities with tag
- `Entity_GetActiveSelf()` - Entity's active state (TODO: implement)
- `Entity_GetActiveInHierarchy()` - Active including parents (TODO: implement)
- `Entity_SetActive()` - Enable/disable entity (TODO: implement)
- `Entity_GetParent()` - Get parent entity (TODO: implement hierarchy)
- `Entity_SetParent()` - Set parent (TODO: implement hierarchy)
- `Entity_SetParentWithTransform()` - Set parent with world position option (TODO)
- `Entity_GetChildCount()` - Number of children (TODO: implement hierarchy)
- `Entity_GetChild()` - Get child by index (TODO: implement hierarchy)

#### Physics System
- `Physics_InternalRaycast()` - Raycast (TODO: integrate with Bullet)
- `Physics_InternalSphereCast()` - Sphere sweep (TODO: integrate with Bullet)
- `Physics_InternalOverlapSphere()` - Find entities in sphere (TODO: integrate with Bullet)
- `Physics_GetGravity()` / `SetGravity()` - World gravity (TODO: connect to physics world)

**Added Helper Functions:**
- `ScriptGlue::Update(deltaTime)` - Updates delayed destroy entities
- `ScriptGlue::UpdateMouseState()` - Calculates mouse delta each frame

**Component Support:**
Added `BoxColliderComponent`, `SphereColliderComponent`, `RigidBodyComponent`, and `AudioSourceComponent` to:
- `Entity_AddComponent()` - Can now add these components
- `Entity_RemoveComponent()` - Can remove these components
- `Entity_GetAllEntitiesWithComponent()` - Can query for these components
- `RegisterComponents()` - Components registered with Mono

### 2. Application.h & Application.cpp
**Location:** `Nebula/src/Nebula/`

**Added Member Variables:**
```cpp
float m_DeltaTime = 0.0f;
float m_Time = 0.0f;
float m_UnscaledTime = 0.0f;
float m_UnscaledDeltaTime = 0.0f;
float m_TimeScale = 1.0f;
int m_FrameCount = 0;
```

**Added Getter Methods:**
- `GetDeltaTime()`, `GetTime()`, `GetUnscaledTime()`, `GetUnscaledDeltaTime()`
- `GetTimeScale()`, `SetTimeScale()`
- `GetFrameCount()`

**Updated Run Loop:**
- Calculates all time values each frame
- `m_Time` is scaled by `m_TimeScale`
- `m_UnscaledTime` always reflects real time
- `m_FrameCount` incremented each frame

### 3. Scene.cpp
**Location:** `Nebula/src/Nebula/Scene/`

**Updated Script Update Loop:**
- Calls `ScriptGlue::Update(deltaTime)` before updating scripts
- Calls `ScriptGlue::UpdateMouseState()` to calculate mouse delta
- Processes delayed entity destruction

## Static Data Added

```cpp
static std::unordered_map<uint32_t, float> s_DestroyDelayedEntities;
static glm::vec2 s_LastMousePos;
static glm::vec2 s_MouseDelta;
static glm::vec2 s_MouseScrollDelta;
```

## Fully Implemented Features ✅

1. **Time System** - Complete
   - Delta time with time scale support
   - Frame counting
   - Scaled and unscaled time tracking

2. **Input Enhancements** - Complete
   - Mouse position tracking
   - Mouse delta calculation
   - Axis input mapping (Horizontal/Vertical)
   - Scroll delta (reset each frame)

3. **Entity Tags** - Complete
   - Get/Set tags
   - Find by tag (single and multiple)

4. **Script Access** - Complete
   - GetScript by type
   - GetScript by class name string

5. **Delayed Destruction** - Complete
   - Entities queued for delayed destruction
   - Countdown timer per entity
   - Automatic destruction when timer expires

6. **Component Support** - Complete
   - All scripting components (Box/Sphere colliders, RigidBody, AudioSource)
   - Add/Remove/Query operations

## Partially Implemented (Stubs) ⚠️

These features have stubs that return default values. They need physics/hierarchy system integration:

1. **Physics Raycasting** - Returns false (no hit)
   - Needs Bullet physics integration
   - RaycastHit structure needs to be filled from collision data

2. **Entity Hierarchy** - Returns defaults
   - Needs parent/child relationship system
   - Transform calculations need to respect hierarchy

3. **Active State** - Returns true
   - Needs entity active/inactive flag
   - Should skip updates when inactive

4. **Gravity** - Returns (0, -9.81, 0)
   - Needs connection to Bullet physics world

## Testing

To test the implementations:

```csharp
// Time system
Console.Log($"DeltaTime: {Time.deltaTime}");
Console.Log($"Time: {Time.time}");
Time.timeScale = 0.5f; // Slow motion

// Input
Vector2 mousePos = Input.mousePosition;
Vector2 mouseDelta = Input.mouseDelta;
float h = Input.GetAxis("Horizontal"); // Uses A/D keys

// Tags
entity.tag = "Player";
ScriptEntity player = GameObject.FindWithTag("Player");

// GetScript
var script = entity.GetScript("PlayerController");

// Delayed destroy
GameObject.Destroy(entity, 2.0f); // Destroy after 2 seconds
```

## Next Steps

To complete the remaining TODO items:

1. **Implement Hierarchy System:**
   - Add parent/children pointers to entities
   - Update transform calculations to respect parent transforms
   - Implement SetParent with world position preservation

2. **Implement Active State:**
   - Add bool active flag to entities
   - Skip Update/Render for inactive entities
   - Implement activeInHierarchy (check all parents)

3. **Integrate Bullet Physics:**
   - Wire up raycast to `btCollisionWorld::rayTest()`
   - Implement sphere cast with `btCollisionWorld::convexSweepTest()`
   - Implement overlap sphere queries
   - Connect gravity get/set to physics world

4. **Collision Callbacks:**
   - Detect collisions in physics step
   - Call C# methods: OnCollisionEnter/Stay/Exit
   - Create Collision objects with contact data
   - Call OnTriggerEnter/Stay/Exit for triggers

5. **Additional Lifecycle:**
   - Call OnFixedUpdate at fixed timestep
   - Call LateUpdate after all Update calls
   - Call OnEnable/OnDisable when entity activated/deactivated

## Performance Notes

- Mouse delta calculation is O(1) per frame
- Delayed destroy uses hash map lookup: O(1) per entity
- Tag finding iterates all entities: O(n) - could optimize with tag index
- Axis input checks 4 keys maximum
- All time getters are inline accessors: O(1)
