#pragma once

#include "Nebula/Core.h"
#include <glm/glm.hpp>
#include <memory>

// Forward declarations for Bullet Physics
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;
class btRigidBody;
class btCollisionShape;

namespace Nebula {

	class Entity;
	class Scene;

	class NEBULA_API PhysicsWorld
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();

		void Init();
		void Shutdown();

		// Simulation
		void Step(float deltaTime);
		void SetGravity(const glm::vec3& gravity);
		glm::vec3 GetGravity() const;

		// Rigidbody management
		void AddRigidBody(Entity entity);
		void RemoveRigidBody(Entity entity);
		void UpdateRigidBodyTransform(Entity entity);
		void SyncTransformFromPhysics(Entity entity);

		// Collider management
		void AddBoxCollider(Entity entity);
		void AddSphereCollider(Entity entity);
		void RemoveCollider(Entity entity);

		// Access
		btDiscreteDynamicsWorld* GetDynamicsWorld() { return m_DynamicsWorld; }

	private:
		void CreateRigidBodyForEntity(Entity entity);
		btCollisionShape* CreateBoxShape(const glm::vec3& size);
		btCollisionShape* CreateSphereShape(float radius);

	private:
		btDefaultCollisionConfiguration* m_CollisionConfiguration = nullptr;
		btCollisionDispatcher* m_Dispatcher = nullptr;
		btBroadphaseInterface* m_Broadphase = nullptr;
		btSequentialImpulseConstraintSolver* m_Solver = nullptr;
		btDiscreteDynamicsWorld* m_DynamicsWorld = nullptr;

		glm::vec3 m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	};

}
