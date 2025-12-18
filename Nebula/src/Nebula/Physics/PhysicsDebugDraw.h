#pragma once
#pragma warning(disable: 4251)

#include <LinearMath/btIDebugDraw.h>
#include <glm/glm.hpp>
#include <vector>

#include "Nebula/Core.h"

namespace Nebula {

	class NEBULA_API PhysicsDebugDraw : public btIDebugDraw
	{
	public:
		PhysicsDebugDraw();
		~PhysicsDebugDraw() override = default;

		// btIDebugDraw interface
		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
		void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) override;
		void reportErrorWarning(const char* warningString) override;
		void draw3dText(const btVector3& location, const char* textString) override;
		void setDebugMode(int debugMode) override;
		int getDebugMode() const override;

		// Rendering
		void ClearLines();
		const std::vector<glm::vec3>& GetLineVertices() const { return m_LineVertices; }
		const std::vector<glm::vec3>& GetLineColors() const { return m_LineColors; }

	private:
		int m_DebugMode;
		std::vector<glm::vec3> m_LineVertices;
		std::vector<glm::vec3> m_LineColors;
	};

}
