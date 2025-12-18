#include "nbpch.h"
#include "PhysicsDebugDraw.h"
#include "Nebula/Log.h"

namespace Nebula {

	PhysicsDebugDraw::PhysicsDebugDraw()
		: m_DebugMode(0)
	{
	}

	void PhysicsDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		m_LineVertices.push_back(glm::vec3(from.x(), from.y(), from.z()));
		m_LineVertices.push_back(glm::vec3(to.x(), to.y(), to.z()));
		
		m_LineColors.push_back(glm::vec3(color.x(), color.y(), color.z()));
		m_LineColors.push_back(glm::vec3(color.x(), color.y(), color.z()));
	}

	void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
		// Draw a small cross at contact points
		btVector3 from = PointOnB;
		btVector3 to = PointOnB + normalOnB * 0.1f;
		drawLine(from, to, color);
	}

	void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
	{
		NB_WARN("Bullet Physics Warning: {0}", warningString);
	}

	void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
	{
		// Not implemented
	}

	void PhysicsDebugDraw::setDebugMode(int debugMode)
	{
		m_DebugMode = debugMode;
	}

	int PhysicsDebugDraw::getDebugMode() const
	{
		return m_DebugMode;
	}

	void PhysicsDebugDraw::ClearLines()
	{
		m_LineVertices.clear();
		m_LineColors.clear();
	}

}
