-- Top-Down Physics Controller
-- This script requires a RigidBody component on the entity
-- Controls: WASD for movement, R to reload scene

-- Variables (configurable in editor)
moveSpeed = 10
rotationSpeed = 180

function OnCreate(entity)
    -- Called once when the entity is created
    Nebula.Log("Top-down controller initialized!")
end

function OnUpdate(entity, deltaTime)
    -- Called every frame
    -- deltaTime is the timestep (time since last frame) in seconds
    
    -- Scene reload
    if Nebula.IsKeyPressed(Nebula.R) then
        Nebula.Log("Reloading scene...")
        Nebula.LoadScene(0)
        return
    end
    
    -- Get current velocity
    local vel = entity:GetVelocity()
    if not vel then
        Nebula.LogWarn("Entity missing RigidBody component!")
        return
    end
    
    -- Top-down movement (X and Z axes, Y is up)
    local moveX = 0
    local moveZ = 0
    
    if Nebula.IsKeyDown(Nebula.W) then
        moveZ = moveSpeed
    end
    if Nebula.IsKeyDown(Nebula.S) then
        moveZ = -moveSpeed
    end
    if Nebula.IsKeyDown(Nebula.A) then
        moveX = -moveSpeed
    end
    if Nebula.IsKeyDown(Nebula.D) then
        moveX = moveSpeed
    end
    
    -- Apply velocity (preserve Y velocity for physics/gravity)
    entity:SetVelocity(moveX, vel.y, moveZ)
    
    -- Optional: Rotate to face movement direction
    if moveX ~= 0 or moveZ ~= 0 then
        local angle = math.atan(moveX, moveZ)
        local targetRotation = math.deg(angle)
        
        local rot = entity:GetRotation()
        -- Smooth rotation towards target
        local currentY = rot.y
        local diff = targetRotation - currentY
        
        -- Normalize angle difference to -180 to 180
        while diff > 180 do diff = diff - 360 end
        while diff < -180 do diff = diff + 360 end
        
        local newY = currentY + diff * rotationSpeed * deltaTime / 180
        entity:SetRotation(rot.x, newY, rot.z)
    end
end