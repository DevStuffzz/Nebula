-- Example Lua Script Template
-- Available APIs:
--   entity:GetPosition() -> {x, y, z}
--   entity:SetPosition(x, y, z)
--   entity:GetRotation() -> {x, y, z}
--   entity:SetRotation(x, y, z)
--   entity:GetScale() -> {x, y, z}
--   entity:SetScale(x, y, z)
--
--   Nebula.IsKeyPressed(keyCode) -> boolean
--   Nebula.IsMouseButtonPressed(button) -> boolean
--   Nebula.GetMousePosition() -> {x, y}
--
--   Nebula.Log(message)
--   Nebula.LogWarn(message)
--   Nebula.LogError(message)
--
--   Nebula.W, Nebula.A, Nebula.S, Nebula.D, Nebula.Space, Nebula.Escape, etc.
--   Nebula.MouseLeft, Nebula.MouseRight, Nebula.MouseMiddle

function OnCreate(entity)
\t-- Called once when the entity is created
\tNebula.Log(\"Entity script initialized!\")
end

function OnUpdate(entity, ts)
\t-- Called every frame
\t-- ts is the timestep (delta time) in seconds
\t
\t-- Example: Simple movement
\tlocal pos = entity:GetPosition()
\t
\tif Nebula.IsKeyPressed(Nebula.W) then
\t\tentity:SetPosition(pos.x, pos.y + 2.0 * ts, pos.z)
\tend
\tif Nebula.IsKeyPressed(Nebula.S) then
\t\tentity:SetPosition(pos.x, pos.y - 2.0 * ts, pos.z)
\tend
end

function OnDestroy(entity)
\t-- Called when the entity is destroyed
\tNebula.Log(\"Entity script destroyed!\")
end
