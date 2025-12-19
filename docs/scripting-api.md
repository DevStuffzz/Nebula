# Nebula Scripting API Reference

This document provides a complete reference for the Lua scripting API available in the Nebula game engine.

## Table of Contents
- [Getting Started](#getting-started)
- [Script Lifecycle](#script-lifecycle)
- [Script Variables](#script-variables)
- [Entity Methods](#entity-methods)
  - [Transform Methods](#transform-methods)
  - [Physics Methods](#physics-methods)
- [Input Functions](#input-functions)
  - [Keyboard Input](#keyboard-input)
  - [Mouse Input](#mouse-input)
- [Logging Functions](#logging-functions)
- [Scene Management](#scene-management)
- [Key Constants](#key-constants)
- [Mouse Button Constants](#mouse-button-constants)
- [Math Utilities](#math-utilities)
- [Common Patterns](#common-patterns)
- [Performance Tips](#performance-tips)
- [Debugging Scripts](#debugging-scripts)
- [Troubleshooting](#troubleshooting)
- [Complete Examples](#complete-examples)
- [Quick Reference](#quick-reference)

---

## Getting Started

### Creating Your First Script

1. Create a new `.lua` file in `assets/scripts/` directory
2. Implement the `OnCreate` and/or `OnUpdate` functions
3. In the editor, select an entity and add a **Script Component**
4. Choose your script from the file browser
5. Press Play to see your script in action

### Basic Script Template

```lua
-- MyScript.lua
-- Variables (configurable in editor)
-- speed: number = 5.0
-- enabled: boolean = true

function OnCreate(entity)
    -- Initialize your script
    Nebula.Log("Script initialized")
end

function OnUpdate(entity, deltaTime)
    -- Update every frame
    if enabled then
        -- Your game logic here
    end
end
```

### Important Concepts

- **Scripts are attached to entities**: Every script has access to the entity it's attached to
- **Frame-independent movement**: Always multiply movement by `deltaTime`
- **Hot-reloading**: Scripts automatically reload when you save changes (in debug builds)
- **Multiple scripts per entity**: Entities can have multiple script components

---

---

## Script Lifecycle

Every script can implement the following callback functions:

### OnCreate(entity)
Called once when the entity is first created or when the script is loaded.

**Parameters:**
- `entity` (Entity): The entity this script is attached to

**Example:**
```lua
function OnCreate(entity)
    Nebula.Log("Entity created!")
end
```

### OnUpdate(entity, deltaTime)
Called every frame to update the entity.

**Parameters:**
- `entity` (Entity): The entity this script is attached to
- `deltaTime` (number): Time elapsed since the last frame in seconds

**Example:**
```lua
function OnUpdate(entity, deltaTime)
    -- Update logic here
end
```

---

## Script Variables

You can define variables in your scripts that will be exposed in the editor's Script Component inspector. The editor will automatically detect variables and allow you to configure their values without editing code.

**Supported Types:**
- `number` - Floating point numbers
- `string` - Text strings
- `boolean` - True/false values
- `table` - Tables with x, y, z fields (for vectors)

**Syntax:**
Variables must be declared in comments at the top of your script using the format:
```lua
-- variableName: type = defaultValue
```

**Example:**
```lua
-- speed: number = 5.0
-- playerName: string = "Player"
-- isActive: boolean = true
-- startPosition: table = {x = 0, y = 0, z = 0}

function OnUpdate(entity, deltaTime)
    -- Use the variables
    if isActive then
        Nebula.Log(playerName .. " speed: " .. tostring(speed))
    end
end
```

**Important Notes:**
- Variables must be declared before any function definitions
- The editor parses these comments to create input fields
- Changes in the editor override the default values
- Each entity can have different values for the same script

---

## Entity Methods

These methods are called on the `entity` object passed to your script functions.

### Transform Methods

#### entity:GetPosition()
Gets the entity's world position.

**Returns:** Table with fields `x`, `y`, `z`

**Example:**
```lua
local pos = entity:GetPosition()
print(pos.x, pos.y, pos.z)
```

#### entity:SetPosition(x, y, z)
Sets the entity's world position.

**Parameters:**
- `x` (number): X coordinate
- `y` (number): Y coordinate
- `z` (number): Z coordinate

**Example:**
```lua
entity:SetPosition(0, 5, 0)
```

#### entity:GetRotation()
Gets the entity's rotation in Euler angles (degrees).

**Returns:** Table with fields `x`, `y`, `z`

**Example:**
```lua
local rot = entity:GetRotation()
print(rot.x, rot.y, rot.z)
```

#### entity:SetRotation(x, y, z)
Sets the entity's rotation using Euler angles (degrees).

**Parameters:**
- `x` (number): Pitch (rotation around X axis)
- `y` (number): Yaw (rotation around Y axis)
- `z` (number): Roll (rotation around Z axis)

**Example:**
```lua
entity:SetRotation(0, 45, 0) -- Rotate 45 degrees around Y axis
```

#### entity:GetScale()
Gets the entity's scale.

**Returns:** Table with fields `x`, `y`, `z`

**Example:**
```lua
local scale = entity:GetScale()
print(scale.x, scale.y, scale.z)
```

#### entity:SetScale(x, y, z)
Sets the entity's scale.

**Parameters:**
- `x` (number): Scale along X axis
- `y` (number): Scale along Y axis
- `z` (number): Scale along Z axis

**Example:**
```lua
entity:SetScale(2, 2, 2) -- Double the size
```

### Physics Methods

These methods require the entity to have a RigidBody component.

#### entity:AddForce(x, y, z)
Applies a force to the entity's rigid body.

**Parameters:**
- `x` (number): Force along X axis
- `y` (number): Force along Y axis
- `z` (number): Force along Z axis

**Example:**
```lua
entity:AddForce(0, 10, 0) -- Apply upward force
```

#### entity:SetVelocity(x, y, z)
Sets the entity's linear velocity directly.

**Parameters:**
- `x` (number): Velocity along X axis
- `y` (number): Velocity along Y axis
- `z` (number): Velocity along Z axis

**Example:**
```lua
entity:SetVelocity(5, 0, 0) -- Move right at 5 units/second
```

#### entity:GetVelocity()
Gets the entity's current linear velocity.

**Returns:** Table with fields `x`, `y`, `z` or `nil` if no physics body

**Example:**
```lua
local vel = entity:GetVelocity()
if vel then
    print("Speed:", math.sqrt(vel.x^2 + vel.y^2 + vel.z^2))
end
```

#### entity:SetAngularVelocity(x, y, z)
Sets the entity's angular velocity (rotation speed).

**Parameters:**
- `x` (number): Angular velocity around X axis
- `y` (number): Angular velocity around Y axis
- `z` (number): Angular velocity around Z axis

**Example:**
```lua
entity:SetAngularVelocity(0, 5, 0) -- Spin around Y axis
```

#### entity:GetAngularVelocity()
Gets the entity's current angular velocity.

**Returns:** Table with fields `x`, `y`, `z` or `nil` if no physics body

**Example:**
```lua
local angVel = entity:GetAngularVelocity()
if angVel then
    print("Angular speed:", angVel.y)
end
```

#### entity:SetMass(mass)
Sets the entity's mass.

**Parameters:**
- `mass` (number): New mass value

**Example:**
```lua
entity:SetMass(10.0)
```

#### entity:GetMass()
Gets the entity's mass.

**Returns:** number

**Example:**
```lua
local mass = entity:GetMass()
print("Mass:", mass)
```

---

## Input Functions

All input functions are accessed through the `Nebula` namespace.

### Keyboard Input

#### Nebula.IsKeyPressed(keycode)
Check if a keyboard key is currently pressed.

**Parameters:**
- `keycode` (number): Key code constant (see [Key Constants](#key-constants))

**Returns:** boolean

**Example:**
```lua
if Nebula.IsKeyPressed(Nebula.W) then
    -- Move forward
end
```

### Mouse Input

#### Nebula.IsMouseButtonPressed(button)
Check if a mouse button is currently pressed.

**Parameters:**
- `button` (number): Mouse button constant (see [Mouse Button Constants](#mouse-button-constants))

**Returns:** boolean

**Example:**
```lua
if Nebula.IsMouseButtonPressed(Nebula.MouseLeft) then
    -- Handle left click
end
```

#### Nebula.GetMousePosition()
Get the current mouse position.

**Returns:** Table with fields `x`, `y`

**Example:**
```lua
local mousePos = Nebula.GetMousePosition()
print("Mouse at:", mousePos.x, mousePos.y)
```

#### Nebula.GetMouseX()
Get the mouse X position.

**Returns:** number

**Example:**
```lua
local x = Nebula.GetMouseX()
```

#### Nebula.GetMouseY()
Get the mouse Y position.

**Returns:** number

**Example:**
```lua
local y = Nebula.GetMouseY()
```

---

## Logging Functions

### Nebula.Log(message)
Log an info message to the console.

**Parameters:**
- `message` (string): Message to log

**Example:**
```lua
Nebula.Log("Script initialized")
```

### Nebula.LogWarn(message)
Log a warning message to the console.

**Parameters:**
- `message` (string): Warning message

**Example:**
```lua
Nebula.LogWarn("Low health!")
```

### Nebula.LogError(message)
Log an error message to the console.

**Parameters:**
- `message` (string): Error message

**Example:**
```lua
Nebula.LogError("Invalid configuration!")
```

---

## Scene Management

### Nebula.LoadScene(sceneIndex)
Load a scene from the scene list by its index.

**Parameters:**
- `sceneIndex` (number): Index of the scene in SceneList.json (0-based)

**Returns:** boolean - `true` if the scene was loaded successfully, `false` otherwise

**Example:**
```lua
-- Load the first scene in the scene list
if Nebula.LoadScene(0) then
    Nebula.Log("Scene loaded successfully!")
else
    Nebula.LogError("Failed to load scene")
end
```

**Notes:**
- Scene indices are 0-based (first scene is index 0)
- The scene list is defined in `assets/SceneList.json`
- Loading a new scene will replace the current active scene
- This is useful for implementing level transitions, menus, or game state changes

**Common Use Cases:**
```lua
-- Load next level
function LoadNextLevel()
    currentLevel = currentLevel + 1
    Nebula.LoadScene(currentLevel)
end

-- Return to main menu
if Nebula.IsKeyPressed(Nebula.Escape) then
    Nebula.LoadScene(0) -- Assuming index 0 is main menu
end

-- Death/respawn
function OnPlayerDeath()
    Nebula.LoadScene(1) -- Reload current level
end
```

---

## Key Constants

Access these through the `Nebula` namespace (e.g., `Nebula.W`, `Nebula.Space`).

### Alphanumeric Keys
- `A` through `Z` - Letter keys
- `Key0` through `Key9` - Number keys

### Special Keys
- `Space` - Space bar
- `Enter` - Enter/Return key
- `Tab` - Tab key
- `Backspace` - Backspace key
- `Escape` - Escape key
- `Delete` - Delete key
- `Insert` - Insert key

### Arrow Keys
- `Up` - Up arrow
- `Down` - Down arrow
- `Left` - Left arrow
- `Right` - Right arrow

### Function Keys
- `F1` through `F12` - Function keys

### Modifier Keys
- `LeftShift` / `RightShift` - Shift keys
- `LeftControl` / `RightControl` - Control keys
- `LeftAlt` / `RightAlt` - Alt keys
- `LeftSuper` / `RightSuper` - Windows/Command keys
- `Menu` - Menu key

### Keypad
- `Keypad0` through `Keypad9` - Numpad number keys
- `KeypadDecimal` - Decimal point
- `KeypadDivide` - Division
- `KeypadMultiply` - Multiplication
- `KeypadSubtract` - Subtraction
- `KeypadAdd` - Addition
- `KeypadEnter` - Numpad enter
- `KeypadEqual` - Equals

### Lock Keys
- `CapsLock` - Caps lock
- `NumLock` - Num lock
- `ScrollLock` - Scroll lock

### Other Keys
- `PageUp` / `PageDown` - Page navigation
- `Home` / `End` - Home and End keys
- `PrintScreen` - Print screen
- `Pause` - Pause key
- `Apostrophe` - ' key
- `Comma` - , key
- `Minus` - - key
- `Period` - . key
- `Slash` - / key
- `Semicolon` - ; key
- `Equal` - = key
- `LeftBracket` - [ key
- `RightBracket` - ] key
- `Backslash` - \ key
- `GraveAccent` - ` key

---

## Mouse Button Constants

Access these through the `Nebula` namespace (e.g., `Nebula.MouseLeft`).

- `MouseLeft` - Left mouse button
- `MouseRight` - Right mouse button
- `MouseMiddle` - Middle mouse button (scroll wheel)
- `Mouse4` through `Mouse8` - Additional mouse buttons

---

## Complete Examples

### Camera Controller
```lua
-- Variables (set these in the editor)
-- speed: number = 5
-- mouseSensitivity: number = 0.1

function OnUpdate(entity, deltaTime)
    local pos = entity:GetPosition()
    local rot = entity:GetRotation()
    
    -- Movement
    local moveSpeed = speed * deltaTime
    
    if Nebula.IsKeyPressed(Nebula.W) then
        -- Move forward based on rotation
        local radY = math.rad(rot.y)
        pos.x = pos.x + math.sin(radY) * moveSpeed
        pos.z = pos.z + math.cos(radY) * moveSpeed
    end
    
    if Nebula.IsKeyPressed(Nebula.S) then
        -- Move backward
        local radY = math.rad(rot.y)
        pos.x = pos.x - math.sin(radY) * moveSpeed
        pos.z = pos.z - math.cos(radY) * moveSpeed
    end
    
    if Nebula.IsKeyPressed(Nebula.A) then
        -- Strafe left
        local radY = math.rad(rot.y - 90)
        pos.x = pos.x + math.sin(radY) * moveSpeed
        pos.z = pos.z + math.cos(radY) * moveSpeed
    end
    
    if Nebula.IsKeyPressed(Nebula.D) then
        -- Strafe right
        local radY = math.rad(rot.y + 90)
        pos.x = pos.x + math.sin(radY) * moveSpeed
        pos.z = pos.z + math.cos(radY) * moveSpeed
    end
    
    entity:SetPosition(pos.x, pos.y, pos.z)
end
```

### Rotating Cube
```lua
-- Variables
-- rotationSpeed: number = 45

function OnUpdate(entity, deltaTime)
    local rot = entity:GetRotation()
    rot.y = rot.y + (rotationSpeed * deltaTime)
    entity:SetRotation(rot.x, rot.y, rot.z)
end
```

### Jump Controller (with Physics)
```lua
-- Variables
-- jumpForce: number = 500
-- moveSpeed: number = 10

local isGrounded = true

function OnUpdate(entity, deltaTime)
    -- Movement with physics
    local vel = entity:GetVelocity()
    
    if Nebula.IsKeyPressed(Nebula.A) then
        vel.x = -moveSpeed
    elseif Nebula.IsKeyPressed(Nebula.D) then
        vel.x = moveSpeed
    else
        vel.x = 0
    end
    
    -- Jump
    if Nebula.IsKeyPressed(Nebula.Space) and isGrounded then
        entity:AddForce(0, jumpForce, 0)
        isGrounded = false
    end
    
    entity:SetVelocity(vel.x, vel.y, vel.z)
    
    -- Simple ground check (y velocity near zero)
    if math.abs(vel.y) < 0.1 then
        isGrounded = true
    end
end
```

### Mouse Follower
```lua
function OnUpdate(entity, deltaTime)
    local mousePos = Nebula.GetMousePosition()
    local pos = entity:GetPosition()
    
    -- Simple 2D mouse following (assuming Y is up)
    pos.x = (mousePos.x - 400) / 50  -- Convert screen to world coords
    pos.z = (mousePos.y - 300) / 50
    
    entity:SetPosition(pos.x, pos.y, pos.z)
end
```

### Interactive Object
```lua
-- Variables
-- hoverHeight: number = 1
-- bobSpeed: number = 2

local time = 0

function OnUpdate(entity, deltaTime)
    time = time + deltaTime
    
    -- Hovering animation
    local pos = entity:GetPosition()
    pos.y = hoverHeight + math.sin(time * bobSpeed) * 0.5
    entity:SetPosition(pos.x, pos.y, pos.z)
    
    -- Rotate slowly
    local rot = entity:GetRotation()
    rot.y = rot.y + (30 * deltaTime)
    entity:SetRotation(rot.x, rot.y, rot.z)
    
    -- Interact on click
    if Nebula.IsMouseButtonPressed(Nebula.MouseLeft) then
        Nebula.Log("Object clicked!")
        entity:AddForce(0, 100, 0) -- Make it jump
    end
end
```

---

## Math Utilities

Lua provides a standard `math` library for common mathematical operations. Here are the most useful functions for game development:

### Trigonometry
```lua
-- Convert degrees to radians
local radians = math.rad(degrees)

-- Convert radians to degrees  
local degrees = math.deg(radians)

-- Trig functions (work with radians)
local sine = math.sin(angle)
local cosine = math.cos(angle)
local tangent = math.tan(angle)

-- Inverse trig
local angle = math.asin(value)
local angle = math.acos(value)
local angle = math.atan(value)
local angle = math.atan2(y, x) -- Two-argument version
```

### Common Math Operations
```lua
-- Absolute value
local abs = math.abs(-5) -- Returns 5

-- Min/Max
local min = math.min(a, b, c)
local max = math.max(a, b, c)

-- Rounding
local floor = math.floor(3.7) -- Returns 3
local ceil = math.ceil(3.2)   -- Returns 4

-- Power and square root
local squared = math.pow(x, 2) -- or x * x
local cubed = math.pow(x, 3)
local sqrt = math.sqrt(16) -- Returns 4

-- Random numbers
math.randomseed(os.time()) -- Seed once in OnCreate
local rand = math.random() -- Random float 0-1
local randInt = math.random(1, 10) -- Random int 1-10

-- Constants
local pi = math.pi -- 3.14159...
local e = math.exp(1) -- Euler's number
```

### Vector Math Helpers

Since the engine returns positions, rotations, and velocities as tables with x, y, z fields, you'll often need to perform vector operations:

```lua
-- Vector length (magnitude)
function VectorLength(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z)
end

-- Normalize vector (make length = 1)
function VectorNormalize(v)
    local len = VectorLength(v)
    if len > 0 then
        return {x = v.x / len, y = v.y / len, z = v.z / len}
    end
    return {x = 0, y = 0, z = 0}
end

-- Vector dot product
function VectorDot(a, b)
    return a.x * b.x + a.y * b.y + a.z * b.z
end

-- Vector cross product
function VectorCross(a, b)
    return {
        x = a.y * b.z - a.z * b.y,
        y = a.z * b.x - a.x * b.z,
        z = a.x * b.y - a.y * b.x
    }
end

-- Distance between two points
function Distance(pos1, pos2)
    local dx = pos2.x - pos1.x
    local dy = pos2.y - pos1.y
    local dz = pos2.z - pos1.z
    return math.sqrt(dx * dx + dy * dy + dz * dz)
end

-- Lerp (linear interpolation)
function Lerp(a, b, t)
    return a + (b - a) * t
end

-- Clamp value between min and max
function Clamp(value, min, max)
    if value < min then return min end
    if value > max then return max end
    return value
end
```

### Usage Example
```lua
-- Variables
-- targetPosition: table = {x = 10, y = 0, z = 10}
-- moveSpeed: number = 5

function OnUpdate(entity, deltaTime)
    local pos = entity:GetPosition()
    
    -- Calculate direction to target
    local direction = {
        x = targetPosition.x - pos.x,
        y = targetPosition.y - pos.y,
        z = targetPosition.z - pos.z
    }
    
    -- Normalize direction
    direction = VectorNormalize(direction)
    
    -- Move towards target
    pos.x = pos.x + direction.x * moveSpeed * deltaTime
    pos.y = pos.y + direction.y * moveSpeed * deltaTime
    pos.z = pos.z + direction.z * moveSpeed * deltaTime
    
    entity:SetPosition(pos.x, pos.y, pos.z)
    
    -- Look at target
    local angle = math.atan2(direction.x, direction.z)
    entity:SetRotation(0, math.deg(angle), 0)
end
```

---

## Common Patterns

### State Machine
```lua
-- Variables
-- currentState: string = "idle"

local states = {}

states.idle = function(entity, deltaTime)
    if Nebula.IsKeyPressed(Nebula.Space) then
        currentState = "jumping"
        entity:AddForce(0, 500, 0)
    end
end

states.jumping = function(entity, deltaTime)
    local vel = entity:GetVelocity()
    if math.abs(vel.y) < 0.1 then
        currentState = "idle"
    end
end

function OnUpdate(entity, deltaTime)
    if states[currentState] then
        states[currentState](entity, deltaTime)
    end
end
```

### Timer/Cooldown System
```lua
-- Variables
-- fireRate: number = 0.5 -- Seconds between shots

local timeSinceLastFire = 0

function OnUpdate(entity, deltaTime)
    timeSinceLastFire = timeSinceLastFire + deltaTime
    
    if Nebula.IsMouseButtonPressed(Nebula.MouseLeft) then
        if timeSinceLastFire >= fireRate then
            Fire()
            timeSinceLastFire = 0
        end
    end
end

function Fire()
    Nebula.Log("Pew!")
    -- Spawn projectile, etc.
end
```

### Smooth Following/Chase
```lua
-- Variables
-- targetSpeed: number = 10
-- acceleration: number = 5

local currentSpeed = 0

function OnUpdate(entity, deltaTime)
    local targetVelocity = 0
    
    if Nebula.IsKeyPressed(Nebula.W) then
        targetVelocity = targetSpeed
    end
    
    -- Smoothly interpolate to target speed
    currentSpeed = Lerp(currentSpeed, targetVelocity, acceleration * deltaTime)
    
    local vel = entity:GetVelocity()
    entity:SetVelocity(0, vel.y, currentSpeed)
end
```

### Circular Motion
```lua
-- Variables
-- radius: number = 5
-- speed: number = 1

local angle = 0

function OnUpdate(entity, deltaTime)
    angle = angle + speed * deltaTime
    
    local x = math.cos(angle) * radius
    local z = math.sin(angle) * radius
    
    entity:SetPosition(x, 0, z)
end
```

### Input Buffer
```lua
local inputBuffer = {}
local bufferTime = 0.2 -- Remember inputs for 0.2 seconds

function OnUpdate(entity, deltaTime)
    -- Record inputs
    if Nebula.IsKeyPressed(Nebula.Space) then
        inputBuffer.jump = deltaTime
    end
    
    -- Age inputs
    if inputBuffer.jump then
        inputBuffer.jump = inputBuffer.jump + deltaTime
        
        -- Use buffered input if within time window
        if inputBuffer.jump < bufferTime and CanJump() then
            Jump()
            inputBuffer.jump = nil
        end
        
        -- Clear old inputs
        if inputBuffer.jump >= bufferTime then
            inputBuffer.jump = nil
        end
    end
end
```

---

## Performance Tips

### Do's
- ✅ **Cache frequently accessed values**
  ```lua
  local pos = entity:GetPosition() -- Cache once
  pos.x = pos.x + 1
  pos.y = pos.y + 1
  entity:SetPosition(pos.x, pos.y, pos.z) -- Set once
  ```

- ✅ **Use local variables** - They're faster than globals
  ```lua
  local speed = 5 -- Local is faster
  ```

- ✅ **Reuse tables** - Avoid creating new tables every frame
  ```lua
  local direction = {x = 0, y = 0, z = 0} -- Created once
  
  function OnUpdate(entity, deltaTime)
      direction.x = 1 -- Reuse existing table
      direction.y = 0
      direction.z = 0
  end
  ```

- ✅ **Early exit conditions**
  ```lua
  if not enabled then return end
  -- Rest of expensive code
  ```

### Don'ts
- ❌ **Don't create new functions in OnUpdate**
  ```lua
  -- BAD
  function OnUpdate(entity, deltaTime)
      local function doSomething() end -- Creates new function every frame
  end
  
  -- GOOD
  local function doSomething() end -- Created once
  
  function OnUpdate(entity, deltaTime)
      doSomething()
  end
  ```

- ❌ **Don't call get/set methods unnecessarily**
  ```lua
  -- BAD
  entity:SetPosition(
      entity:GetPosition().x + 1,
      entity:GetPosition().y,
      entity:GetPosition().z
  ) -- Gets position 3 times!
  
  -- GOOD
  local pos = entity:GetPosition() -- Get once
  entity:SetPosition(pos.x + 1, pos.y, pos.z)
  ```

- ❌ **Don't use globals without reason**
  ```lua
  -- BAD
  speed = 5 -- Global variable
  
  -- GOOD
  local speed = 5 -- Local variable
  ```

- ❌ **Don't log every frame in release**
  ```lua
  function OnUpdate(entity, deltaTime)
      -- Nebula.Log("Update") -- This will kill performance!
  end
  ```

---

## Debugging Scripts

### Using Print/Log Statements
```lua
function OnUpdate(entity, deltaTime)
    local pos = entity:GetPosition()
    Nebula.Log("Position: " .. pos.x .. ", " .. pos.y .. ", " .. pos.z)
    
    -- Format tables nicely
    local vel = entity:GetVelocity()
    if vel then
        Nebula.Log(string.format("Velocity: %.2f, %.2f, %.2f", vel.x, vel.y, vel.z))
    end
end
```

### Checking Variable Types
```lua
local value = entity:GetPosition()
Nebula.Log("Type: " .. type(value)) -- "table"
Nebula.Log("Has x: " .. tostring(value.x ~= nil))
```

### Error Handling
```lua
function SafeFunction(entity)
    local success, error = pcall(function()
        -- Code that might fail
        entity:SetPosition(x, y, z)
    end)
    
    if not success then
        Nebula.LogError("Error: " .. tostring(error))
    end
end
```

### Debugging Physics
```lua
function OnUpdate(entity, deltaTime)
    if entity:GetVelocity() then
        local vel = entity:GetVelocity()
        local speed = math.sqrt(vel.x^2 + vel.y^2 + vel.z^2)
        Nebula.Log("Speed: " .. string.format("%.2f", speed))
        Nebula.Log("Mass: " .. entity:GetMass())
    else
        Nebula.LogWarn("No RigidBody component!")
    end
end
```

### Visual Debugging
Since the engine doesn't have built-in debug draw from Lua, use visual cues:
```lua
-- Change color or scale to visualize states
function OnUpdate(entity, deltaTime)
    local scale = entity:GetScale()
    
    if isActive then
        entity:SetScale(1.2, 1.2, 1.2) -- Larger when active
    else
        entity:SetScale(1, 1, 1) -- Normal when inactive
    end
end
```

---

## Troubleshooting

### Script Not Running
- ✓ Check the Console window for Lua errors
- ✓ Ensure the script file is in `assets/scripts/`
- ✓ Verify the Script Component has the correct file path
- ✓ Make sure you've implemented `OnUpdate` or `OnCreate`
- ✓ Press Play - scripts only run in Play mode

### Physics Not Working
- ✓ Ensure entity has a **RigidBody Component**
- ✓ Check that the RigidBody Type is set to Dynamic (not Static)
- ✓ Verify the mass is > 0 for dynamic bodies
- ✓ Check if gravity is enabled if you expect falling
- ✓ Make sure you're in Play mode (physics doesn't simulate in editor)

### Input Not Detected
- ✓ Verify you're using the correct key constant (e.g., `Nebula.W`, not "W")
- ✓ Check the window has focus
- ✓ Make sure the input code is in `OnUpdate`, not `OnCreate`
- ✓ Test with a simple log to verify the function is being called

### Variable Not Showing in Editor
- ✓ Check the comment syntax: `-- name: type = value`
- ✓ Variables must be declared before any functions
- ✓ Ensure there's a space after `--`
- ✓ Reload the script component (remove and re-add it)

### Entity Methods Failing
```lua
-- Check if component exists before using it
if entity:GetVelocity() then
    entity:SetVelocity(0, 0, 0)
else
    Nebula.LogError("Entity missing RigidBody component!")
end
```

### Hot-Reload Not Working
- ✓ Hot-reload only works in Debug builds
- ✓ Syntax errors prevent reload - check console
- ✓ Save the file with Ctrl+S
- ✓ The script must already be loaded (press Play first)

### Common Lua Errors

**"attempt to index a nil value"**
```lua
-- BAD
local pos = entity:GetPosition()
print(pos.w) -- 'w' doesn't exist, causes error

-- GOOD
if pos.x then
    print(pos.x)
end
```

**"attempt to call a nil value"**
```lua
-- BAD
entity:NonExistentMethod() -- Method doesn't exist

-- GOOD - Check the API reference for correct method names
entity:GetPosition()
```

**"attempt to perform arithmetic on a nil value"**
```lua
-- BAD
local speed = speed + 1 -- 'speed' is nil

-- GOOD
local speed = 0 -- Initialize first
speed = speed + 1
```

---

## Best Practices

1. **Use Script Variables**: Define configurable values as script variables in the editor for easy tweaking
2. **Cache Values**: Store frequently accessed values in local variables to avoid repeated function calls
3. **Normalize Directions**: When using direction vectors for movement, normalize them for consistent speed
4. **Use deltaTime**: Always multiply movement and rotation by `deltaTime` for frame-rate independent behavior
5. **Check Components**: Before using physics methods, ensure the entity has a RigidBody component
6. **Log Errors**: Use `Nebula.LogError()` to help debug issues in your scripts
7. **Keep Scripts Focused**: Each script should have a single responsibility
8. **Comment Your Code**: Explain complex logic for future reference
9. **Test Incrementally**: Test small changes frequently rather than writing lots of code at once
10. **Use Meaningful Names**: `playerSpeed` is better than `s` or `speed1`

---

## Quick Reference

### Script Structure
```lua
-- Variables (optional)
-- varName: type = defaultValue

-- Local variables and helper functions
local myVar = 0

function HelperFunction()
    -- ...
end

-- Lifecycle callbacks
function OnCreate(entity)
    -- Initialization
end

function OnUpdate(entity, deltaTime)
    -- Update logic
end
```

### Entity Methods Cheat Sheet
```lua
-- Transform
local pos = entity:GetPosition()          -- Returns {x, y, z}
entity:SetPosition(x, y, z)               -- Sets position
local rot = entity:GetRotation()          -- Returns {x, y, z} in degrees
entity:SetRotation(x, y, z)               -- Sets rotation in degrees
local scale = entity:GetScale()           -- Returns {x, y, z}
entity:SetScale(x, y, z)                  -- Sets scale

-- Physics (requires RigidBody component)
entity:AddForce(x, y, z)                  -- Apply force
entity:SetVelocity(x, y, z)               -- Set linear velocity
local vel = entity:GetVelocity()          -- Get linear velocity
entity:SetAngularVelocity(x, y, z)        -- Set angular velocity
local angVel = entity:GetAngularVelocity() -- Get angular velocity
entity:SetMass(mass)                      -- Set mass
local mass = entity:GetMass()             -- Get mass
```

### Input Cheat Sheet
```lua
-- Keyboard
if Nebula.IsKeyPressed(Nebula.W) then end
if Nebula.IsKeyPressed(Nebula.Space) then end
if Nebula.IsKeyPressed(Nebula.Escape) then end

-- Mouse
if Nebula.IsMouseButtonPressed(Nebula.MouseLeft) then end
if Nebula.IsMouseButtonPressed(Nebula.MouseRight) then end
local mousePos = Nebula.GetMousePosition() -- {x, y}
local x = Nebula.GetMouseX()
local y = Nebula.GetMouseY()
```

### Logging Cheat Sheet
```lua
Nebula.Log("Info message")
Nebula.LogWarn("Warning message")
Nebula.LogError("Error message")
```

### Scene Management Cheat Sheet
```lua
if Nebula.LoadScene(0) then
    Nebula.Log("Scene loaded")
end
```

### Common Math Operations
```lua
-- Angles
local rad = math.rad(degrees)
local deg = math.deg(radians)

-- Trig (use radians)
local sin = math.sin(angle)
local cos = math.cos(angle)
local atan2 = math.atan2(y, x)

-- Utility
local abs = math.abs(value)
local min = math.min(a, b)
local max = math.max(a, b)
local sqrt = math.sqrt(value)
local floor = math.floor(value)
local ceil = math.ceil(value)

-- Random
math.randomseed(os.time())
local rand = math.random()        -- 0-1
local randInt = math.random(1, 10) -- 1-10
```

### Vector Operations Template
```lua
-- Copy this to your scripts for common vector operations

function VectorLength(v)
    return math.sqrt(v.x*v.x + v.y*v.y + v.z*v.z)
end

function VectorNormalize(v)
    local len = VectorLength(v)
    if len > 0 then
        return {x=v.x/len, y=v.y/len, z=v.z/len}
    end
    return {x=0, y=0, z=0}
end

function Distance(p1, p2)
    local dx, dy, dz = p2.x-p1.x, p2.y-p1.y, p2.z-p1.z
    return math.sqrt(dx*dx + dy*dy + dz*dz)
end

function Lerp(a, b, t)
    return a + (b - a) * t
end

function Clamp(value, min, max)
    return math.max(min, math.min(max, value))
end
```

### Movement Pattern Templates

**Basic Movement**
```lua
function OnUpdate(entity, deltaTime)
    local pos = entity:GetPosition()
    
    if Nebula.IsKeyPressed(Nebula.W) then
        pos.z = pos.z + speed * deltaTime
    end
    if Nebula.IsKeyPressed(Nebula.S) then
        pos.z = pos.z - speed * deltaTime
    end
    if Nebula.IsKeyPressed(Nebula.A) then
        pos.x = pos.x - speed * deltaTime
    end
    if Nebula.IsKeyPressed(Nebula.D) then
        pos.x = pos.x + speed * deltaTime
    end
    
    entity:SetPosition(pos.x, pos.y, pos.z)
end
```

**Physics Movement**
```lua
function OnUpdate(entity, deltaTime)
    local vel = entity:GetVelocity()
    
    if Nebula.IsKeyPressed(Nebula.W) then
        vel.z = speed
    elseif Nebula.IsKeyPressed(Nebula.S) then
        vel.z = -speed
    else
        vel.z = 0
    end
    
    entity:SetVelocity(vel.x, vel.y, vel.z)
end
```

**Smooth Rotation**
```lua
function OnUpdate(entity, deltaTime)
    local rot = entity:GetRotation()
    rot.y = rot.y + rotationSpeed * deltaTime
    entity:SetRotation(rot.x, rot.y, rot.z)
end
```

**Look At Direction**
```lua
function LookAt(entity, targetPos)
    local pos = entity:GetPosition()
    local dx = targetPos.x - pos.x
    local dz = targetPos.z - pos.z
    local angle = math.atan2(dx, dz)
    entity:SetRotation(0, math.deg(angle), 0)
end
```

---

## Additional Resources

### Lua Documentation
- [Lua 5.3 Reference Manual](https://www.lua.org/manual/5.3/)
- [Programming in Lua (book)](https://www.lua.org/pil/)

### Game Development Patterns
- State machines for AI and game logic
- Object pooling for performance
- Component-based architecture
- Event systems for loose coupling

### Next Steps
1. Explore the example scripts in `assets/scripts/`
2. Experiment with physics by creating dynamic objects
3. Try building a simple game mechanic (e.g., collectibles, enemies)
4. Combine multiple scripts on one entity for complex behavior
5. Create reusable utility functions for common operations

---

**Happy Scripting! 🚀**

For questions or issues, check the console for error messages and refer to the [Troubleshooting](#troubleshooting) section.
