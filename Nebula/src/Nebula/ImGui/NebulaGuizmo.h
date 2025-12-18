#pragma once

#include <glm/glm.hpp>
#include "Nebula/Core.h"

namespace Nebula {

	enum class NEBULA_API GuizmoOperation
	{
		Translate = 1 << 0,
		Rotate = 1 << 1,
		Scale = 1 << 2,
		Bounds = 1 << 3
	};

	enum class NEBULA_API GuizmoMode
	{
		Local,
		World
	};

	class NEBULA_API NebulaGuizmo
	{
	public:
		// Setup
		static void SetDrawlist();
		static void BeginFrame();
		
		// Rect management
		static void SetRect(float x, float y, float width, float height);
		static void SetOrthographic(bool isOrthographic);
		
		// Main manipulation function
		static bool Manipulate(
			const float* view,
			const float* projection,
			GuizmoOperation operation,
			GuizmoMode mode,
			float* matrix,
			float* deltaMatrix = nullptr,
			const float* snap = nullptr
		);

		// Decompose matrix
		static void DecomposeMatrixToComponents(
			const float* matrix,
			float* translation,
			float* rotation,
			float* scale
		);

		// Recompose matrix
		static void RecomposeMatrixFromComponents(
			const float* translation,
			const float* rotation,
			const float* scale,
			float* matrix
		);

		// Utility
		static bool IsOver();
		static bool IsUsing();
		static void Enable(bool enable);
		static void SetID(int id);
	};

}
