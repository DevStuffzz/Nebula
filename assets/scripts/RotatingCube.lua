-- Rotating Cube Script
-- Makes an entity rotate continuously

local rotationSpeed = 45.0 -- degrees per second

function OnCreate(entity)
\tNebula.Log(\"Rotating entity created\")
end

function OnUpdate(entity, ts)
\tlocal rot = entity:GetRotation()
\tentity:SetRotation(rot.x, rot.y + rotationSpeed * ts, rot.z)
end

function OnDestroy(entity)
\tNebula.Log(\"Rotating entity destroyed\")
end
