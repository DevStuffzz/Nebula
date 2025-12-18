#include "nbpch.h"
#include "NebulaGuizmo.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Nebula {

	void NebulaGuizmo::SetDrawlist()
	{
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	}

	void NebulaGuizmo::BeginFrame()
	{
		ImGuizmo::BeginFrame();
	}

	void NebulaGuizmo::SetRect(float x, float y, float width, float height)
	{
		ImGuizmo::SetRect(x, y, width, height);
	}

	void NebulaGuizmo::SetOrthographic(bool isOrthographic)
	{
		ImGuizmo::SetOrthographic(isOrthographic);
	}

	bool NebulaGuizmo::Manipulate(
		const float* view,
		const float* projection,
		GuizmoOperation operation,
		GuizmoMode mode,
		float* matrix,
		float* deltaMatrix,
		const float* snap)
	{
		ImGuizmo::OPERATION op;
		switch (operation)
		{
		case GuizmoOperation::Translate: op = ImGuizmo::TRANSLATE; break;
		case GuizmoOperation::Rotate: op = ImGuizmo::ROTATE; break;
		case GuizmoOperation::Scale: op = ImGuizmo::SCALE; break;
		case GuizmoOperation::Bounds: op = ImGuizmo::BOUNDS; break;
		default: op = ImGuizmo::TRANSLATE; break;
		}

		ImGuizmo::MODE md = (mode == GuizmoMode::Local) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;

		return ImGuizmo::Manipulate(view, projection, op, md, matrix, deltaMatrix, snap);
	}

	void NebulaGuizmo::DecomposeMatrixToComponents(
		const float* matrix,
		float* translation,
		float* rotation,
		float* scale)
	{
		ImGuizmo::DecomposeMatrixToComponents(matrix, translation, rotation, scale);
	}

	void NebulaGuizmo::RecomposeMatrixFromComponents(
		const float* translation,
		const float* rotation,
		const float* scale,
		float* matrix)
	{
		ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, matrix);
	}

	bool NebulaGuizmo::IsOver()
	{
		return ImGuizmo::IsOver();
	}

	bool NebulaGuizmo::IsUsing()
	{
		return ImGuizmo::IsUsing();
	}

	void NebulaGuizmo::Enable(bool enable)
	{
		ImGuizmo::Enable(enable);
	}

	void NebulaGuizmo::SetID(int id)
	{
		ImGuizmo::SetID(id);
	}

}