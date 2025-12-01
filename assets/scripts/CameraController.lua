-- Camera Controller Script
-- Controls camera movement with WASD and E/Q for up/down

function OnCreate(entity)
	Nebula.Log("Camera Controller initialized")
end

function OnUpdate(entity, ts)
	-- Get camera position from transform
	local pos = entity:GetPosition()
	
	-- Movement speed
	local speed = 5.0 * ts
	
	-- WASD movement
	if Nebula.IsKeyPressed(Nebula.W) then
		pos.z = pos.z - speed
	end
	
	if Nebula.IsKeyPressed(Nebula.S) then
		pos.z = pos.z + speed
	end
	
	if Nebula.IsKeyPressed(Nebula.A) then
		pos.x = pos.x - speed
	end
	
	if Nebula.IsKeyPressed(Nebula.D) then
		pos.x = pos.x + speed
	end
	
	-- Vertical movement
	if Nebula.IsKeyPressed(Nebula.E) then
		pos.y = pos.y + speed
	end
	
	if Nebula.IsKeyPressed(Nebula.Q) then
		pos.y = pos.y - speed
	end
	
	-- Update camera position
	entity:SetPosition(pos.x, pos.y, pos.z)
end

function OnDestroy(entity)
	Nebula.Log("Camera Controller destroyed")
end
