-- Mouse Follower Script
-- Makes the entity follow the mouse cursor

function OnCreate(entity)
    Nebula.Log("Mouse Follower initialized")
end

function OnUpdate(entity, ts)
    -- Get mouse position
    local mousePos = Nebula.GetMousePosition()
    
    -- Get current position
    local pos = entity:GetPosition()
    
    -- Convert screen space to world space (simplified)
    -- You would normally use camera matrices for this
    local targetX = (mousePos.x / 960.0) * 10.0 - 5.0
    local targetY = -(mousePos.y / 540.0) * 10.0 + 5.0
    
    -- Smoothly interpolate to target position
    local speed = 5.0 * ts
    pos.x = pos.x + (targetX - pos.x) * speed
    pos.y = pos.y + (targetY - pos.y) * speed
    
    entity:SetPosition(pos.x, pos.y, pos.z)
    
    -- Scale on mouse click
    if Nebula.IsMouseButtonPressed(Nebula.MouseLeft) then
        entity:SetScale(1.5, 1.5, 1.5)
    elseif Nebula.IsMouseButtonPressed(Nebula.MouseRight) then
        entity:SetScale(0.5, 0.5, 0.5)
    else
        entity:SetScale(1.0, 1.0, 1.0)
    end
end

function OnDestroy(entity)
    Nebula.Log("Mouse Follower destroyed")
end
