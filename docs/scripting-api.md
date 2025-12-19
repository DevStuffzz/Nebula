# Nebula Scripting API Reference

This document provides a complete reference for the Lua scripting API available in the Nebula game engine.

## Table of Contents
- [Script Lifecycle](#script-lifecycle)
- [Entity Methods](#entity-methods)
- [Input Functions](#input-functions)
- [Logging Functions](#logging-functions)
- [Key Constants](#key-constants)
- [Mouse Button Constants](#mouse-button-constants)
- [Complete Examples](#complete-examples)

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

## Best Practices

1. **Use Script Variables**: Define configurable values as script variables in the editor for easy tweaking
2. **Cache Values**: Store frequently accessed values in local variables to avoid repeated function calls
3. **Normalize Directions**: When using direction vectors for movement, normalize them for consistent speed
4. **Use deltaTime**: Always multiply movement and rotation by `deltaTime` for frame-rate independent behavior
5. **Check Components**: Before using physics methods, ensure the entity has a RigidBody component
6. **Log Errors**: Use `Nebula.LogError()` to help debug issues in your scripts

---

## Script Variables

You can define variables in your scripts that will be exposed in the editor's Script Component inspector. The editor will automatically detect variables and allow you to configure their values without editing code.

**Supported Types:**
- `number` - Floating point numbers
- `string` - Text strings
- `boolean` - True/false values
- `table` - Tables with x, y, z fields (for vectors)

**Example:**
```lua
-- Define these at the top of your script
-- speed: number = 5.0
-- playerName: string = "Player"
-- isActive: boolean = true

function OnUpdate(entity, deltaTime)
    -- Use the variables
    if isActive then
        Nebula.Log(playerName .. " speed: " .. speed)
    end
end
```

The editor will parse the comments and create appropriate input fields for each variable.
