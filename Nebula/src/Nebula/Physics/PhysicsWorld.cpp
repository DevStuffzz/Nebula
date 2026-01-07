#include "nbpch.h"
#include "PhysicsWorld.h"
#include "PhysicsDebugDraw.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Log.h"

#include <btBulletDynamicsCommon.h>

namespace Nebula {

	PhysicsWorld::PhysicsWorld()
	{
	}

	PhysicsWorld::~PhysicsWorld()
	{
		Shutdown();
	}

	void PhysicsWorld::Init()
	{
		// Create Bullet physics world
		m_CollisionConfiguration = new btDefaultCollisionConfiguration();
		m_Dispatcher = new btCollisionDispatcher(m_CollisionConfiguration);
		m_Broadphase = new btDbvtBroadphase();
		m_Solver = new btSequentialImpulseConstraintSolver();
		m_DynamicsWorld = new btDiscreteDynamicsWorld(m_Dispatcher, m_Broadphase, m_Solver, m_CollisionConfiguration);

		// Set gravity
		m_DynamicsWorld->setGravity(btVector3(m_Gravity.x, m_Gravity.y, m_Gravity.z));

		// Create debug drawer
		m_DebugDrawer = std::make_unique<PhysicsDebugDraw>();
		m_DebugDrawer->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		m_DynamicsWorld->setDebugDrawer(m_DebugDrawer.get());

		NB_INFO("Physics World initialized");
	}

	void PhysicsWorld::Shutdown()
	{
		if (m_DynamicsWorld)
		{
			// Clean up all rigid bodies
			for (int i = m_DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject* obj = m_DynamicsWorld->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}
				m_DynamicsWorld->removeCollisionObject(obj);
				delete obj;
			}

			delete m_DynamicsWorld;
			delete m_Solver;
			delete m_Broadphase;
			delete m_Dispatcher;
			delete m_CollisionConfiguration;

			m_DynamicsWorld = nullptr;
			m_Solver = nullptr;
			m_Broadphase = nullptr;
			m_Dispatcher = nullptr;
			m_CollisionConfiguration = nullptr;
		}
	}

	void PhysicsWorld::Step(float deltaTime)
	{
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->stepSimulation(deltaTime, 10);
			
			// Debug: Check for collisions
			int numManifolds = m_DynamicsWorld->getDispatcher()->getNumManifolds();
			if (numManifolds > 0)
			{
				for (int i = 0; i < numManifolds; i++)
				{
					btPersistentManifold* contactManifold = m_DynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
					int numContacts = contactManifold->getNumContacts();
					if (numContacts > 0)
					{
					}
				}
			}
		}
	}

	void PhysicsWorld::SetGravity(const glm::vec3& gravity)
	{
		m_Gravity = gravity;
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->setGravity(btVector3(gravity.x, gravity.y, gravity.z));
		}
	}

	glm::vec3 PhysicsWorld::GetGravity() const
	{
		return m_Gravity;
	}

	void PhysicsWorld::SetDebugDrawEnabled(bool enabled)
	{
		m_DebugDrawEnabled = enabled;
	}

	void PhysicsWorld::DebugDraw()
	{
		if (m_DynamicsWorld && m_DebugDrawer && m_DebugDrawEnabled)
		{
			m_DebugDrawer->ClearLines();
			m_DynamicsWorld->debugDrawWorld();
		}
	}

	const std::vector<glm::vec3>& PhysicsWorld::GetDebugLineVertices() const
	{
		static std::vector<glm::vec3> empty;
		return m_DebugDrawer ? m_DebugDrawer->GetLineVertices() : empty;
	}

	const std::vector<glm::vec3>& PhysicsWorld::GetDebugLineColors() const
	{
		static std::vector<glm::vec3> empty;
		return m_DebugDrawer ? m_DebugDrawer->GetLineColors() : empty;
	}

	btCollisionShape* PhysicsWorld::CreateBoxShape(const glm::vec3& size)
	{
		// Bullet btBoxShape takes half-extents, so divide full size by 2
		return new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
	}

	btCollisionShape* PhysicsWorld::CreateSphereShape(float radius)
	{
		return new btSphereShape(radius);
	}

	void PhysicsWorld::AddBoxCollider(Entity entity)
	{
		if (!entity.HasComponent<BoxColliderComponent>())
			return;

		auto& collider = entity.GetComponent<BoxColliderComponent>();
		
		// Clean up old shape if exists
		if (collider.RuntimeShape)
		{
			delete collider.RuntimeShape;
		}

		// Apply entity scale to collision shape
		glm::vec3 finalSize = collider.Size;
		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			finalSize *= transform.Scale;
			collider.LastScale = transform.Scale; // Track current scale
			collider.LastSize = collider.Size;     // Track current size
			NB_INFO("Creating BoxCollider - Size: ({0}, {1}, {2}), Scale: ({3}, {4}, {5}), Final: ({6}, {7}, {8})",
				collider.Size.x, collider.Size.y, collider.Size.z,
				transform.Scale.x, transform.Scale.y, transform.Scale.z,
				finalSize.x, finalSize.y, finalSize.z);
		}
		else
		{
			collider.LastSize = collider.Size; // Track current size even without transform
		}

		// Create new box shape
		collider.RuntimeShape = CreateBoxShape(finalSize);
	}

	void PhysicsWorld::AddSphereCollider(Entity entity)
	{
		if (!entity.HasComponent<SphereColliderComponent>())
			return;

		auto& collider = entity.GetComponent<SphereColliderComponent>();
		
		// Clean up old shape if exists
		if (collider.RuntimeShape)
		{
			delete collider.RuntimeShape;
		}

		// Apply entity scale to collision shape (use max component for uniform sphere scaling)
		float finalRadius = collider.Radius;
		if (entity.HasComponent<TransformComponent>())
		{
			auto& transform = entity.GetComponent<TransformComponent>();
			float maxScale = glm::max(glm::max(transform.Scale.x, transform.Scale.y), transform.Scale.z);
			finalRadius *= maxScale;
			collider.LastScale = transform.Scale; // Track current scale
			collider.LastRadius = collider.Radius; // Track current radius
		}
		else
		{
			collider.LastRadius = collider.Radius; // Track current radius even without transform
		}

		// Create new sphere shape
		collider.RuntimeShape = CreateSphereShape(finalRadius);
	}

	void PhysicsWorld::RemoveCollider(Entity entity)
	{
		if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& collider = entity.GetComponent<BoxColliderComponent>();
			if (collider.RuntimeShape)
			{
				delete collider.RuntimeShape;
				collider.RuntimeShape = nullptr;
			}
		}

		if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& collider = entity.GetComponent<SphereColliderComponent>();
			if (collider.RuntimeShape)
			{
				delete collider.RuntimeShape;
				collider.RuntimeShape = nullptr;
			}
		}
	}

	void PhysicsWorld::CreateRigidBodyForEntity(Entity entity)
	{
		if (!entity.HasComponent<RigidBodyComponent>() || !entity.HasComponent<TransformComponent>())
			return;

		auto& rb = entity.GetComponent<RigidBodyComponent>();
		auto& transform = entity.GetComponent<TransformComponent>();

		// Determine collision shape
		btCollisionShape* shape = nullptr;
		if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
			if (!boxCollider.RuntimeShape)
				AddBoxCollider(entity);
			shape = boxCollider.RuntimeShape;
		}
		else if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
			if (!sphereCollider.RuntimeShape)
				AddSphereCollider(entity);
			shape = sphereCollider.RuntimeShape;
		}

		if (!shape)
		{
			NB_WARN("RigidBody requires a collider component");
			return;
		}

		NB_INFO("Creating RigidBody at position ({0}, {1}, {2})", 
			transform.Position.x, transform.Position.y, transform.Position.z);

		// Calculate mass and inertia
		btScalar mass = (rb.Type == RigidBodyComponent::BodyType::Dynamic) ? rb.Mass : 0.0f;
		btVector3 localInertia(0, 0, 0);
		if (mass != 0.0f)
			shape->calculateLocalInertia(mass, localInertia);

		// Calculate final position including collider offset
		glm::vec3 finalPosition = transform.Position;
		if (entity.HasComponent<BoxColliderComponent>())
		{
			auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
			// Apply rotation to offset
			glm::quat rotQuat = glm::quat(glm::radians(transform.Rotation));
			glm::vec3 rotatedOffset = rotQuat * boxCollider.Offset;
			finalPosition += rotatedOffset;
		}
		else if (entity.HasComponent<SphereColliderComponent>())
		{
			auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
			// Apply rotation to offset
			glm::quat rotQuat = glm::quat(glm::radians(transform.Rotation));
			glm::vec3 rotatedOffset = rotQuat * sphereCollider.Offset;
			finalPosition += rotatedOffset;
		}

		// Set initial transform
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(finalPosition.x, finalPosition.y, finalPosition.z));
		
		// Convert euler angles to quaternion
		glm::quat rotQuat = glm::quat(glm::radians(transform.Rotation));
		startTransform.setRotation(btQuaternion(rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w));

		// Create motion state
		btDefaultMotionState* motionState = new btDefaultMotionState(startTransform);

		// Create rigid body
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
		rbInfo.m_linearDamping = rb.LinearDrag;
		rbInfo.m_angularDamping = rb.AngularDrag;

		btRigidBody* body = new btRigidBody(rbInfo);

		// Set flags based on body type
		if (rb.Type == RigidBodyComponent::BodyType::Static)
		{
			// Static bodies should have the static flag set
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_STATIC_OBJECT);
		}
		else if (rb.IsKinematic || rb.Type == RigidBodyComponent::BodyType::Kinematic)
		{
			// Kinematic bodies can be moved but aren't affected by forces
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
		}

		// Set initial velocity (only relevant for dynamic bodies)
		if (rb.Type == RigidBodyComponent::BodyType::Dynamic)
		{
			body->setLinearVelocity(btVector3(rb.LinearVelocity.x, rb.LinearVelocity.y, rb.LinearVelocity.z));
			body->setAngularVelocity(btVector3(rb.AngularVelocity.x, rb.AngularVelocity.y, rb.AngularVelocity.z));
		}

		// Freeze rotation if needed
		if (rb.FreezeRotation)
		{
			body->setAngularFactor(btVector3(0, 0, 0));
		}

		// Disable gravity if needed
		if (!rb.UseGravity)
		{
			body->setGravity(btVector3(0, 0, 0));
		}

		// Set friction and restitution for realistic collisions
		body->setFriction(0.5f);
		body->setRestitution(0.3f);

		// Store in component
		rb.RuntimeBody = body;

		// Add to world
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->addRigidBody(body);
			
			// Log rigid body creation for debugging
			const char* typeStr = "Unknown";
			if (rb.Type == RigidBodyComponent::BodyType::Static) typeStr = "Static";
			else if (rb.Type == RigidBodyComponent::BodyType::Dynamic) typeStr = "Dynamic";
			else if (rb.Type == RigidBodyComponent::BodyType::Kinematic) typeStr = "Kinematic";
			
			NB_INFO("Added {0} RigidBody (mass: {1}) to physics world", typeStr, rb.Mass);
		}
	}

	void PhysicsWorld::AddRigidBody(Entity entity)
	{
		CreateRigidBodyForEntity(entity);
	}

	void PhysicsWorld::RemoveRigidBody(Entity entity)
	{
		if (!entity.HasComponent<RigidBodyComponent>())
			return;

		auto& rb = entity.GetComponent<RigidBodyComponent>();
		if (rb.RuntimeBody)
		{
			if (m_DynamicsWorld)
			{
				m_DynamicsWorld->removeRigidBody(rb.RuntimeBody);
			}

			if (rb.RuntimeBody->getMotionState())
			{
				delete rb.RuntimeBody->getMotionState();
			}

			delete rb.RuntimeBody;
			rb.RuntimeBody = nullptr;
		}
	}

	void PhysicsWorld::UpdateRigidBodyTransform(Entity entity)
	{
		if (!entity.HasComponent<RigidBodyComponent>() || !entity.HasComponent<TransformComponent>())
			return;

		auto& rb = entity.GetComponent<RigidBodyComponent>();
		auto& transform = entity.GetComponent<TransformComponent>();

		if (rb.RuntimeBody)
		{
			// Update position and rotation
			btTransform btTrans;
			btTrans.setIdentity();
			btTrans.setOrigin(btVector3(transform.Position.x, transform.Position.y, transform.Position.z));
			
			glm::quat rotQuat = glm::quat(glm::radians(transform.Rotation));
			btTrans.setRotation(btQuaternion(rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w));

			rb.RuntimeBody->setWorldTransform(btTrans);
			rb.RuntimeBody->getMotionState()->setWorldTransform(btTrans);
			
			// For kinematic objects, we need to activate them so the transform update takes effect
			if (rb.Type == RigidBodyComponent::BodyType::Kinematic || rb.IsKinematic)
			{
				rb.RuntimeBody->activate(true);
			}

			// Update collision shape if scale changed
			btCollisionShape* currentShape = rb.RuntimeBody->getCollisionShape();
			if (currentShape)
			{
				btCollisionShape* newShape = nullptr;
				
				// Recreate shape with new scale if entity has colliders
				if (entity.HasComponent<BoxColliderComponent>())
				{
					auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
					
					// Only recreate if scale or size changed
					if (boxCollider.LastScale != transform.Scale || boxCollider.LastSize != boxCollider.Size)
					{
						glm::vec3 finalSize = boxCollider.Size * transform.Scale;
						newShape = CreateBoxShape(finalSize);
						boxCollider.LastScale = transform.Scale;
						boxCollider.LastSize = boxCollider.Size;
					
						// Clean up old shape (stored in component)
						if (boxCollider.RuntimeShape && boxCollider.RuntimeShape == currentShape)
						{
							delete boxCollider.RuntimeShape;
						}
						boxCollider.RuntimeShape = newShape;
					}
				}
				else if (entity.HasComponent<SphereColliderComponent>())
				{
					auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
					
					// Only recreate if scale or radius changed
					if (sphereCollider.LastScale != transform.Scale || sphereCollider.LastRadius != sphereCollider.Radius)
					{
						float maxScale = glm::max(glm::max(transform.Scale.x, transform.Scale.y), transform.Scale.z);
						float finalRadius = sphereCollider.Radius * maxScale;
						newShape = CreateSphereShape(finalRadius);
						sphereCollider.LastScale = transform.Scale;
						sphereCollider.LastRadius = sphereCollider.Radius;
					
						// Clean up old shape (stored in component)
						if (sphereCollider.RuntimeShape && sphereCollider.RuntimeShape == currentShape)
						{
							delete sphereCollider.RuntimeShape;
						}
						sphereCollider.RuntimeShape = newShape;
					}
				}

				// Update rigid body to use new shape
				if (newShape)
				{
					rb.RuntimeBody->setCollisionShape(newShape);
					
					// Recalculate inertia if it's a dynamic body
					if (rb.Type == RigidBodyComponent::BodyType::Dynamic && rb.Mass > 0.0f)
					{
						btVector3 localInertia(0, 0, 0);
						newShape->calculateLocalInertia(rb.Mass, localInertia);
						rb.RuntimeBody->setMassProps(rb.Mass, localInertia);
					}
				}
			}
		}
	}

	void PhysicsWorld::SyncTransformFromPhysics(Entity entity)
	{
		if (!entity.HasComponent<RigidBodyComponent>() || !entity.HasComponent<TransformComponent>())
			return;

		auto& rb = entity.GetComponent<RigidBodyComponent>();
		auto& transform = entity.GetComponent<TransformComponent>();

		if (rb.RuntimeBody && rb.RuntimeBody->getMotionState())
		{
			btTransform btTrans;
			rb.RuntimeBody->getMotionState()->getWorldTransform(btTrans);

			// Update position
			btVector3 origin = btTrans.getOrigin();
			glm::vec3 physicsPosition = glm::vec3(origin.x(), origin.y(), origin.z());

			// Update rotation first (needed for offset calculation)
			btQuaternion rotation = btTrans.getRotation();
			glm::quat glmQuat(rotation.w(), rotation.x(), rotation.y(), rotation.z());
			transform.Rotation = glm::degrees(glm::eulerAngles(glmQuat));

			// Remove collider offset from physics position to get entity position
			if (entity.HasComponent<BoxColliderComponent>())
			{
				auto& boxCollider = entity.GetComponent<BoxColliderComponent>();
				glm::vec3 rotatedOffset = glmQuat * boxCollider.Offset;
				physicsPosition -= rotatedOffset;
			}
			else if (entity.HasComponent<SphereColliderComponent>())
			{
				auto& sphereCollider = entity.GetComponent<SphereColliderComponent>();
				glm::vec3 rotatedOffset = glmQuat * sphereCollider.Offset;
				physicsPosition -= rotatedOffset;
			}

			transform.Position = physicsPosition;

			// Sync velocities
			btVector3 linVel = rb.RuntimeBody->getLinearVelocity();
			rb.LinearVelocity = glm::vec3(linVel.x(), linVel.y(), linVel.z());

			btVector3 angVel = rb.RuntimeBody->getAngularVelocity();
			rb.AngularVelocity = glm::vec3(angVel.x(), angVel.y(), angVel.z());
		}
	}

	void PhysicsWorld::SyncAllRigidBodyTransforms(Scene* scene)
	{
		if (!scene)
			return;

		// Iterate all entities with rigid bodies and sync their physics transforms
		auto view = scene->GetRegistry().view<RigidBodyComponent, TransformComponent>();
		for (auto entity : view)
		{
			Entity ent{ entity, scene };
			UpdateRigidBodyTransform(ent);
		}
	}

}
