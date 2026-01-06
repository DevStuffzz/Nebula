# C++ Engine Implementation Checklist

This document lists all the C++ internal calls that need to be implemented in your Nebula engine to support the new scripting features.

## Time System Internal Calls

Add these to your Mono script engine bindings:

```cpp
// Time.cs bindings
float Time_GetDeltaTime();
float Time_GetTime();
float Time_GetTimeScale();
void Time_SetTimeScale(float value);
float Time_GetFixedDeltaTime();
void Time_SetFixedDeltaTime(float value);
float Time_GetUnscaledTime();
float Time_GetUnscaledDeltaTime();
int Time_GetFrameCount();
```

**Implementation Notes:**
- Track `m_Time`, `m_DeltaTime`, `m_UnscaledTime`, `m_UnscaledDeltaTime` in your application
- Store `m_TimeScale` (default 1.0f) and apply it to delta time
- Increment `m_FrameCount` each frame
- Track `m_FixedDeltaTime` for physics timestep

## Input System Internal Calls

```cpp
// Mouse position/delta (Input.cs)
void Input_GetMousePosition(Vector2* outPosition);
void Input_GetMouseDelta(Vector2* outDelta);
void Input_GetMouseScrollDelta(Vector2* outScroll);

// Axis input
float Input_GetAxisValue(MonoString* axisName);
float Input_GetAxisRawValue(MonoString* axisName);
```

**Implementation Notes:**
- Track mouse position each frame
- Calculate mouse delta (current - previous position)
- Store scroll wheel delta from input events
- Implement input axis mapping (e.g., "Horizontal" → A/D keys, "Vertical" → W/S keys)
- GetAxis should smooth values, GetAxisRaw should be instant (-1, 0, or 1)

## Physics System Internal Calls

```cpp
// Physics.cs bindings
bool Physics_InternalRaycast(Vector3* origin, Vector3* direction, float maxDistance, RaycastHit* outHit);
bool Physics_InternalSphereCast(Vector3* origin, float radius, Vector3* direction, float maxDistance, RaycastHit* outHit);
MonoArray* Physics_InternalOverlapSphere(Vector3* position, float radius); // Returns uint[]
void Physics_GetGravity(Vector3* outGravity);
void Physics_SetGravity(Vector3* gravity);
```

**RaycastHit Structure (must match C# struct):**
```cpp
struct RaycastHit {
    uint32_t entityID;      // Entity that was hit
    Vector3 point;          // Impact point
    Vector3 normal;         // Surface normal
    float distance;         // Distance from origin
};
```

**Implementation Notes:**
- Integrate with Bullet physics for raycasting
- Use `btCollisionWorld::rayTest()` or similar
- Fill `RaycastHit` structure with collision data
- For `OverlapSphere`, use `btCollisionWorld::getCollidingPairsTest()` or sphere sweep

## Entity System Internal Calls

### Script Access
```cpp
// ScriptEntity.cs additions
MonoObject* Entity_GetScriptByName(uint32_t entityID, MonoString* className);
```

**Implementation:**
- Search entity's script components for matching class name
- Return the Mono script instance object

### Tag System
```cpp
// GameObject.cs additions
uint32_t Entity_FindByTag(MonoString* tag);
MonoArray* Entity_FindAllByTag(MonoString* tag); // Returns uint[]

// ScriptEntity.cs additions
MonoString* Entity_GetTag(uint32_t entityID);
void Entity_SetTag(uint32_t entityID, MonoString* tag);
```

**Implementation:**
- Store tag string in entity (or TagComponent)
- Maintain tag index for fast lookup
- `FindByTag` returns first match, `FindAllByTag` returns all

### Active State
```cpp
// ScriptEntity.cs additions
bool Entity_GetActiveSelf(uint32_t entityID);
bool Entity_GetActiveInHierarchy(uint32_t entityID);
void Entity_SetActive(uint32_t entityID, bool active);
```

**Implementation:**
- `activeSelf`: Entity's own active state
- `activeInHierarchy`: Active AND all parents are active
- `SetActive`: Enable/disable entity and all its systems
- When disabled, skip Update/Render but keep entity alive

### Hierarchy
```cpp
// Transform hierarchy (ScriptEntity.cs additions)
uint32_t Entity_GetParent(uint32_t entityID);
void Entity_SetParent(uint32_t entityID, uint32_t parentID);
void Entity_SetParentWithTransform(uint32_t entityID, uint32_t parentID, bool worldPositionStays);
int Entity_GetChildCount(uint32_t entityID);
uint32_t Entity_GetChild(uint32_t entityID, int index);
```

**Implementation:**
- Store parent entity ID in each entity
- Maintain children list/array in parent
- `worldPositionStays = true`: Keep world position when re-parenting
- `worldPositionStays = false`: Keep local position (change world position)
- Transform calculations must respect hierarchy (world = local * parent)

### Delayed Destruction
```cpp
// GameObject.cs addition
void Entity_DestroyDelayed(uint32_t entityID, float delay);
```

**Implementation:**
- Queue entity for destruction after `delay` seconds
- Store in list: `{ entityID, timeRemaining }`
- Update list each frame, decrement timers
- When timer reaches 0, call regular `Entity_Destroy`

## Collision Callbacks

**No new internal calls needed**, but the engine must:

1. Detect collisions in physics step
2. For each collision, call appropriate C# methods on ScriptBehavior instances:

```cpp
// When collision starts
void InvokeCollisionEnter(MonoObject* scriptInstance, Collision* collision)
{
    // Create Collision C# object
    MonoObject* collisionObj = CreateCollisionObject(collision);
    
    // Call method: void OnCollisionEnter(Collision collision)
    MonoMethod* method = GetMethod(scriptInstance, "OnCollisionEnter");
    if (method) {
        void* args[] = { collisionObj };
        mono_runtime_invoke(method, scriptInstance, args, nullptr);
    }
}
```

**Collision object creation:**
```cpp
Collision* collision = new Collision();
collision->entity = GetScriptEntity(otherEntityID);
collision->relativeVelocity = CalculateRelativeVelocity();
collision->contactPoint = GetContactPoint();
collision->contactNormal = GetContactNormal();
return CreateMonoObject("Nebula.Collision", collision);
```

**Callbacks to invoke:**
- `OnCollisionEnter(Collision)` - First frame of collision
- `OnCollisionStay(Collision)` - Every frame while colliding
- `OnCollisionExit(Collision)` - Frame when collision ends
- `OnTriggerEnter(ScriptEntity)` - Trigger entered
- `OnTriggerStay(ScriptEntity)` - While in trigger
- `OnTriggerExit(ScriptEntity)` - Trigger exited

## Lifecycle Callbacks

Add these invocations to your script system:

```cpp
// Call these at appropriate times
InvokeMethod(scriptInstance, "OnCreate");          // When script added/entity created
InvokeMethod(scriptInstance, "OnUpdate", dt);      // Every frame
InvokeMethod(scriptInstance, "OnFixedUpdate");     // Fixed timestep (physics)
InvokeMethod(scriptInstance, "LateUpdate");        // After all Update calls
InvokeMethod(scriptInstance, "OnEnable");          // When entity enabled
InvokeMethod(scriptInstance, "OnDisable");         // When entity disabled
InvokeMethod(scriptInstance, "OnDestroy");         // Before destruction
```

**Call order each frame:**
```
1. OnEnable() - if entity just became active
2. OnFixedUpdate() - at fixed intervals
3. OnUpdate(dt) - every frame
4. LateUpdate() - after all updates
5. OnDisable() - if entity just became inactive
```

## Inspector Serialization

**No internal calls needed for attributes themselves**, but:

1. **Reflection**: When loading a scene/prefab:
   ```cpp
   // Read all fields with [SerializeField] or public fields without [HideInInspector]
   MonoClass* klass = mono_object_get_class(scriptInstance);
   void* iter = nullptr;
   while (MonoClassField* field = mono_class_get_fields(klass, &iter)) {
       MonoCustomAttrInfo* attrs = mono_custom_attrs_from_field(klass, field);
       
       // Check for SerializeField, HideInInspector, Range, etc.
       bool serialize = IsPublic(field) || HasAttribute(attrs, "SerializeField");
       if (HasAttribute(attrs, "HideInInspector"))
           serialize = false;
       
       if (serialize) {
           // Save/load field value
       }
   }
   ```

2. **Editor Integration**: In your Cosmic editor:
   - Parse attributes using Mono reflection
   - Generate appropriate UI widgets (slider for Range, color picker for ColorUsage, etc.)
   - Allow editing and save back to script instance

## Components

The new components (SphereColliderComponent, CapsuleColliderComponent, AudioSourceComponent, LightComponent) should work with existing `Entity_GetComponent` / `Entity_AddComponent` infrastructure. Just ensure:

1. Register component types with ECS/entity system
2. C++ component classes match C# field layout
3. Implement physics collision for Sphere/Capsule colliders in Bullet

## Testing Checklist

After implementation, verify:

- [ ] Time.deltaTime matches actual frame time
- [ ] Time.timeScale affects game speed
- [ ] Mouse position/delta updates correctly
- [ ] Raycasts hit colliders and return correct data
- [ ] Collision callbacks fire at right times
- [ ] GetScript returns script instances with editable fields
- [ ] SetActive enables/disables entities
- [ ] Parent/child hierarchy transforms correctly
- [ ] Tags work for finding entities
- [ ] Destroy with delay waits correct time
- [ ] Inspector attributes display correctly in editor
- [ ] OnFixedUpdate calls at fixed rate
- [ ] LateUpdate calls after Update

## Performance Notes

- Cache Mono method lookups (don't search by name every frame)
- Use method thunks for frequent callbacks
- Consider object pooling for frequently created Collision objects
- Optimize tag lookups with hash map: `unordered_map<string, vector<EntityID>>`
- Don't iterate all entities for FindByTag; maintain indices

## Example Internal Call Registration

```cpp
// In your ScriptEngine::RegisterInternalCalls()
mono_add_internal_call("Nebula.Time::GetDeltaTime", (void*)Time_GetDeltaTime);
mono_add_internal_call("Nebula.Time::GetTime", (void*)Time_GetTime);
mono_add_internal_call("Nebula.Input::GetMousePosition", (void*)Input_GetMousePosition);
mono_add_internal_call("Nebula.Physics::InternalRaycast", (void*)Physics_InternalRaycast);
mono_add_internal_call("Nebula.InternalCalls::Entity_GetScriptByName", (void*)Entity_GetScriptByName);
mono_add_internal_call("Nebula.InternalCalls::Entity_GetTag", (void*)Entity_GetTag);
mono_add_internal_call("Nebula.InternalCalls::Entity_SetActive", (void*)Entity_SetActive);
mono_add_internal_call("Nebula.InternalCalls::Entity_GetParent", (void*)Entity_GetParent);
mono_add_internal_call("Nebula.InternalCalls::Entity_DestroyDelayed", (void*)Entity_DestroyDelayed);
// ... etc
```
