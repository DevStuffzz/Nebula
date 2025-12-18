#include "nbpch.h"
#include "PhysicsWorld.h"
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

	btCollisionShape* PhysicsWorld::CreateBoxShape(const glm::vec3& size)
	{
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

		// Create new box shape
		collider.RuntimeShape = CreateBoxShape(collider.Size);
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

		// Create new sphere shape
		collider.RuntimeShape = CreateSphereShape(collider.Radius);
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

		// Calculate mass and inertia
		btScalar mass = (rb.Type == RigidBodyComponent::BodyType::Dynamic) ? rb.Mass : 0.0f;
		btVector3 localInertia(0, 0, 0);
		if (mass != 0.0f)
			shape->calculateLocalInertia(mass, localInertia);

		// Set initial transform
		btTransform startTransform;
		startTransform.setIdentity();
		startTransform.setOrigin(btVector3(transform.Position.x, transform.Position.y, transform.Position.z));
		
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

		// Set kinematic flag
		if (rb.IsKinematic || rb.Type == RigidBodyComponent::BodyType::Kinematic)
		{
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
			body->setActivationState(DISABLE_DEACTIVATION);
		}

		// Set initial velocity
		body->setLinearVelocity(btVector3(rb.LinearVelocity.x, rb.LinearVelocity.y, rb.LinearVelocity.z));
		body->setAngularVelocity(btVector3(rb.AngularVelocity.x, rb.AngularVelocity.y, rb.AngularVelocity.z));

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

		// Store in component
		rb.RuntimeBody = body;

		// Add to world
		if (m_DynamicsWorld)
		{
			m_DynamicsWorld->addRigidBody(body);
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
			btTransform btTrans;
			btTrans.setIdentity();
			btTrans.setOrigin(btVector3(transform.Position.x, transform.Position.y, transform.Position.z));
			
			glm::quat rotQuat = glm::quat(glm::radians(transform.Rotation));
			btTrans.setRotation(btQuaternion(rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w));

			rb.RuntimeBody->setWorldTransform(btTrans);
			rb.RuntimeBody->getMotionState()->setWorldTransform(btTrans);
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
			transform.Position = glm::vec3(origin.x(), origin.y(), origin.z());

			// Update rotation
			btQuaternion rotation = btTrans.getRotation();
			glm::quat glmQuat(rotation.w(), rotation.x(), rotation.y(), rotation.z());
			transform.Rotation = glm::degrees(glm::eulerAngles(glmQuat));

			// Sync velocities
			btVector3 linVel = rb.RuntimeBody->getLinearVelocity();
			rb.LinearVelocity = glm::vec3(linVel.x(), linVel.y(), linVel.z());

			btVector3 angVel = rb.RuntimeBody->getAngularVelocity();
			rb.AngularVelocity = glm::vec3(angVel.x(), angVel.y(), angVel.z());
		}
	}

}
