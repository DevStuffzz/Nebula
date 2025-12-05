#include "Gizmo.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Renderer/Camera.h"
#include "Nebula/Application.h"
#include "Nebula/Input.h"
#include "Nebula/ImGui/NebulaGui.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Cosmic {

	Gizmo::Gizmo()
	{
	}

	void Gizmo::Render(Nebula::Entity& entity, Mode mode, const glm::vec2 viewportBounds[2])
	{
		if (!entity)
			return;

		if (!entity.HasComponent<Nebula::TransformComponent>())
			return;

		auto& tc = entity.GetComponent<Nebula::TransformComponent>();
		glm::vec3 origin = tc.Position;

		// Get camera for projection
		auto& camera = Nebula::Application::Get().GetCamera();
		auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
		if (!perspCam) return;

		glm::mat4 viewProj = perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix();

		if (!viewportBounds)
			return;

		// Validate viewport bounds
		float viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
		float viewportHeight = viewportBounds[1].y - viewportBounds[0].y;
		if (viewportWidth <= 0.0f || viewportHeight <= 0.0f)
			return;

		// Draw list for rendering
		void* drawListPtr = Nebula::NebulaGui::GetWindowDrawList();
		if (!drawListPtr)
			return;

		ImDrawList* drawList = static_cast<ImDrawList*>(drawListPtr);

		// Clip drawing to viewport bounds
		drawList->PushClipRect(ImVec2(viewportBounds[0].x, viewportBounds[0].y), 
							  ImVec2(viewportBounds[1].x, viewportBounds[1].y), true);

		// Projection function
		auto project = [&](const glm::vec3& worldPos) -> glm::vec2 {
			glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);
			if (clip.w == 0.0f) clip.w = 0.001f;
			glm::vec3 ndc = glm::vec3(clip) / clip.w;

			float x = viewportBounds[0].x + (ndc.x + 1.0f) * 0.5f * viewportWidth;
			float y = viewportBounds[0].y + (1.0f - ndc.y) * 0.5f * viewportHeight;

			return glm::vec2(x, y);
		};

		// If mode is None, just render a small grey circle at the origin
		if (mode == Mode::None)
		{
			glm::vec2 originScreen = project(origin);
			float circleRadius = 5.0f;
			ImU32 greyColor = IM_COL32(128, 128, 128, 255);
			
			drawList->AddCircleFilled(ImVec2(originScreen.x, originScreen.y), circleRadius, greyColor);
			
			// Draw directional indicators for lights and cameras
			RenderDirectionIndicators(entity, origin, drawList, viewportBounds, project);
			
			drawList->PopClipRect();
			return;
		}

		// Get hovered axis
		auto [mouseX, mouseY] = Nebula::Input::GetMousePos();
		Axis hoveredAxis = m_IsUsingGizmo ? m_ActiveAxis : GetHoveredAxis(entity, mode, glm::vec2(mouseX, mouseY), viewportBounds);

		// Render based on mode
		switch (mode)
		{
		case Mode::Translate:
			RenderTranslateGizmo(origin, hoveredAxis, drawList, viewportBounds, project);
			break;
		case Mode::Rotate:
			RenderRotateGizmo(origin, tc.Rotation, hoveredAxis, drawList, viewportBounds, project);
			break;
		case Mode::Scale:
			RenderScaleGizmo(origin, hoveredAxis, drawList, viewportBounds, project);
			break;
		}

		// Draw directional indicators for lights and cameras
		RenderDirectionIndicators(entity, origin, drawList, viewportBounds, project);

		// Restore clip rect
		drawList->PopClipRect();
	}

	bool Gizmo::HandleInput(Nebula::Entity& entity, Mode mode, const glm::vec2& mousePos, 
						   bool mousePressed, const glm::vec2 viewportBounds[2])
	{
		if (!entity || mode == Mode::None)
			return false;

		if (!entity.HasComponent<Nebula::TransformComponent>())
			return false;

		auto& tc = entity.GetComponent<Nebula::TransformComponent>();

		// Start dragging
		if (mousePressed && !m_IsUsingGizmo)
		{
			m_ActiveAxis = GetHoveredAxis(entity, mode, mousePos, viewportBounds);
			if (m_ActiveAxis != Axis::None)
			{
				m_IsUsingGizmo = true;
				m_MouseStartPos = mousePos;
				m_GizmoStartPos = tc.Position;
				m_GizmoStartRot = tc.Rotation;
				m_GizmoStartScale = tc.Scale;

				// Calculate initial intersection
				auto& camera = Nebula::Application::Get().GetCamera();
				auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
				if (perspCam && viewportBounds)
				{
					auto screenToRay = [&](const glm::vec2& mp) -> std::pair<glm::vec3, glm::vec3> {
						float viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
						float viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

						float ndcX = ((mp.x - viewportBounds[0].x) / viewportWidth) * 2.0f - 1.0f;
						float ndcY = 1.0f - ((mp.y - viewportBounds[0].y) / viewportHeight) * 2.0f;

						glm::mat4 invViewProj = glm::inverse(perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix());
						glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
						glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

						glm::vec4 rayWorldNear = invViewProj * rayClipNear;
						glm::vec4 rayWorldFar = invViewProj * rayClipFar;

						rayWorldNear /= rayWorldNear.w;
						rayWorldFar /= rayWorldFar.w;

						glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
						glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar) - glm::vec3(rayWorldNear));

						return { rayOrigin, rayDir };
					};

					auto rayPlaneIntersect = [](const glm::vec3& rayOrigin, const glm::vec3& rayDir,
											   const glm::vec3& planePoint, const glm::vec3& planeNormal) -> glm::vec3 {
						float denom = glm::dot(planeNormal, rayDir);
						if (glm::abs(denom) > 0.0001f) {
							float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
							if (t >= 0) {
								return rayOrigin + rayDir * t;
							}
						}
						return rayOrigin;
					};

					auto [rayOrigin, rayDir] = screenToRay(mousePos);

					// Initialize drag start intersection based on mode and axis
					if (m_ActiveAxis == Axis::X || m_ActiveAxis == Axis::Y || m_ActiveAxis == Axis::Z)
					{
						if (mode == Mode::Rotate)
						{
							glm::quat quat = glm::quat(glm::radians(m_GizmoStartRot));
							glm::mat4 rotationMatrix = glm::toMat4(quat);

							glm::vec3 planeNormal(0.0f);
							if (m_ActiveAxis == Axis::X) planeNormal = glm::vec3(1, 0, 0);
							else if (m_ActiveAxis == Axis::Y) planeNormal = glm::vec3(0, 1, 0);
							else if (m_ActiveAxis == Axis::Z) planeNormal = glm::vec3(0, 0, 1);

							planeNormal = glm::vec3(rotationMatrix * glm::vec4(planeNormal, 0.0f));
							m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);
						}
						else // Translate or Scale
						{
							glm::vec3 axis(0.0f);
							if (m_ActiveAxis == Axis::X) axis = glm::vec3(1, 0, 0);
							else if (m_ActiveAxis == Axis::Y) axis = glm::vec3(0, 1, 0);
							else if (m_ActiveAxis == Axis::Z) axis = glm::vec3(0, 0, 1);

							glm::vec3 cameraRight = glm::normalize(glm::cross(rayDir, axis));
							glm::vec3 planeNormal = glm::normalize(glm::cross(axis, cameraRight));
							glm::vec3 intersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);

							glm::vec3 toIntersection = intersection - m_GizmoStartPos;
							float projection = glm::dot(toIntersection, axis);
							m_DragStartIntersection = m_GizmoStartPos + axis * projection;
						}
					}
					else if (m_ActiveAxis == Axis::XY)
					{
						m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(0, 0, 1));
					}
					else if (m_ActiveAxis == Axis::XZ)
					{
						m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(0, 1, 0));
					}
					else if (m_ActiveAxis == Axis::YZ)
					{
						m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(1, 0, 0));
					}
					else if (m_ActiveAxis == Axis::Screen && mode == Mode::Rotate)
					{
						glm::vec3 toCamera = glm::normalize(perspCam->GetPosition() - m_GizmoStartPos);
						m_DragStartIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, toCamera);
					}
				}
			}
		}

		// Update during drag
		if (m_IsUsingGizmo && mousePressed)
		{
			auto& camera = Nebula::Application::Get().GetCamera();
			auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
			if (!perspCam || !viewportBounds) return true;

			auto screenToRay = [&](const glm::vec2& mp) -> std::pair<glm::vec3, glm::vec3> {
				float viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
				float viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

				float ndcX = ((mp.x - viewportBounds[0].x) / viewportWidth) * 2.0f - 1.0f;
				float ndcY = 1.0f - ((mp.y - viewportBounds[0].y) / viewportHeight) * 2.0f;

				glm::mat4 invViewProj = glm::inverse(perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix());
				glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
				glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

				glm::vec4 rayWorldNear = invViewProj * rayClipNear;
				glm::vec4 rayWorldFar = invViewProj * rayClipFar;

				rayWorldNear /= rayWorldNear.w;
				rayWorldFar /= rayWorldFar.w;

				glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
				glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar) - glm::vec3(rayWorldNear));

				return { rayOrigin, rayDir };
			};

			auto rayPlaneIntersect = [](const glm::vec3& rayOrigin, const glm::vec3& rayDir,
									   const glm::vec3& planePoint, const glm::vec3& planeNormal) -> glm::vec3 {
				float denom = glm::dot(planeNormal, rayDir);
				if (glm::abs(denom) > 0.0001f) {
					float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
					if (t >= 0) {
						return rayOrigin + rayDir * t;
					}
				}
				return rayOrigin;
			};

			auto [rayOrigin, rayDir] = screenToRay(mousePos);

			if (mode == Mode::Translate)
			{
				glm::vec3 currentIntersection = m_GizmoStartPos;

				if (m_ActiveAxis == Axis::X || m_ActiveAxis == Axis::Y || m_ActiveAxis == Axis::Z)
				{
					glm::vec3 axis(0.0f);
					if (m_ActiveAxis == Axis::X) axis = glm::vec3(1, 0, 0);
					else if (m_ActiveAxis == Axis::Y) axis = glm::vec3(0, 1, 0);
					else if (m_ActiveAxis == Axis::Z) axis = glm::vec3(0, 0, 1);

					glm::vec3 cameraRight = glm::normalize(glm::cross(rayDir, axis));
					glm::vec3 planeNormal = glm::normalize(glm::cross(axis, cameraRight));
					glm::vec3 intersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);

					glm::vec3 toIntersection = intersection - m_GizmoStartPos;
					float projection = glm::dot(toIntersection, axis);
					currentIntersection = m_GizmoStartPos + axis * projection;
				}
				else if (m_ActiveAxis == Axis::XY)
				{
					currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(0, 0, 1));
					currentIntersection.z = m_GizmoStartPos.z;
				}
				else if (m_ActiveAxis == Axis::XZ)
				{
					currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(0, 1, 0));
					currentIntersection.y = m_GizmoStartPos.y;
				}
				else if (m_ActiveAxis == Axis::YZ)
				{
					currentIntersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, glm::vec3(1, 0, 0));
					currentIntersection.x = m_GizmoStartPos.x;
				}

				glm::vec3 delta = currentIntersection - m_DragStartIntersection;
				tc.Position = m_GizmoStartPos + delta;
			}
			else if (mode == Mode::Rotate)
			{
				if (m_ActiveAxis == Axis::Screen)
				{
					glm::vec3 toCamera = glm::normalize(perspCam->GetPosition() - m_GizmoStartPos);
					glm::vec3 currentPoint = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, toCamera);

					glm::vec3 startVec = m_DragStartIntersection - m_GizmoStartPos;
					glm::vec3 currentVec = currentPoint - m_GizmoStartPos;

					startVec = startVec - glm::dot(startVec, toCamera) * toCamera;
					currentVec = currentVec - glm::dot(currentVec, toCamera) * toCamera;

					float startLen = glm::length(startVec);
					float currentLen = glm::length(currentVec);

					if (startLen > 0.001f && currentLen > 0.001f)
					{
						startVec = glm::normalize(startVec);
						currentVec = glm::normalize(currentVec);

						float cosAngle = glm::clamp(glm::dot(startVec, currentVec), -1.0f, 1.0f);
						float angle = glm::acos(cosAngle);

						glm::vec3 cross = glm::cross(startVec, currentVec);
						if (glm::dot(cross, toCamera) < 0.0f)
							angle = -angle;

						glm::quat startQuat = glm::quat(glm::radians(m_GizmoStartRot));
						glm::quat deltaQuat = glm::angleAxis(angle, toCamera);
						glm::quat finalQuat = deltaQuat * startQuat;

						glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(finalQuat));

						auto normalizeAngle = [](float angle) {
							while (angle > 180.0f) angle -= 360.0f;
							while (angle < -180.0f) angle += 360.0f;
							return angle;
						};

						eulerAngles.x = normalizeAngle(eulerAngles.x);
						eulerAngles.y = normalizeAngle(eulerAngles.y);
						eulerAngles.z = normalizeAngle(eulerAngles.z);

						tc.Rotation = eulerAngles;
					}
				}
				else
				{
					glm::quat startQuat = glm::quat(glm::radians(m_GizmoStartRot));
					glm::mat4 rotationMatrix = glm::toMat4(startQuat);

					glm::vec3 localAxis(0.0f);
					glm::vec3 planeNormal(0.0f);

					if (m_ActiveAxis == Axis::X) {
						localAxis = glm::vec3(1, 0, 0);
						planeNormal = glm::vec3(1, 0, 0);
					}
					else if (m_ActiveAxis == Axis::Y) {
						localAxis = glm::vec3(0, 1, 0);
						planeNormal = glm::vec3(0, 1, 0);
					}
					else if (m_ActiveAxis == Axis::Z) {
						localAxis = glm::vec3(0, 0, 1);
						planeNormal = glm::vec3(0, 0, 1);
					}

					glm::vec3 worldAxis = glm::vec3(rotationMatrix * glm::vec4(localAxis, 0.0f));
					planeNormal = glm::vec3(rotationMatrix * glm::vec4(planeNormal, 0.0f));

					glm::vec3 currentPoint = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);

					glm::vec3 startVec = m_DragStartIntersection - m_GizmoStartPos;
					glm::vec3 currentVec = currentPoint - m_GizmoStartPos;

					startVec = startVec - glm::dot(startVec, planeNormal) * planeNormal;
					currentVec = currentVec - glm::dot(currentVec, planeNormal) * planeNormal;

					float startLen = glm::length(startVec);
					float currentLen = glm::length(currentVec);

					if (startLen > 0.001f && currentLen > 0.001f)
					{
						startVec = glm::normalize(startVec);
						currentVec = glm::normalize(currentVec);

						float cosAngle = glm::clamp(glm::dot(startVec, currentVec), -1.0f, 1.0f);
						float angle = glm::acos(cosAngle);

						glm::vec3 cross = glm::cross(startVec, currentVec);
						if (glm::dot(cross, planeNormal) < 0.0f)
							angle = -angle;

						glm::quat deltaQuat = glm::angleAxis(angle, worldAxis);
						glm::quat finalQuat = deltaQuat * startQuat;

						glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(finalQuat));

						auto normalizeAngle = [](float angle) {
							while (angle > 180.0f) angle -= 360.0f;
							while (angle < -180.0f) angle += 360.0f;
							return angle;
						};

						eulerAngles.x = normalizeAngle(eulerAngles.x);
						eulerAngles.y = normalizeAngle(eulerAngles.y);
						eulerAngles.z = normalizeAngle(eulerAngles.z);

						tc.Rotation = eulerAngles;
					}
				}
			}
			else if (mode == Mode::Scale)
			{
				if (m_ActiveAxis == Axis::Screen)
				{
					float mouseDelta = mousePos.y - m_MouseStartPos.y;
					float scaleFactor = 1.0f - (mouseDelta * 0.005f);
					scaleFactor = glm::max(scaleFactor, 0.01f);

					tc.Scale = m_GizmoStartScale * scaleFactor;
				}
				else if (m_ActiveAxis == Axis::X || m_ActiveAxis == Axis::Y || m_ActiveAxis == Axis::Z)
				{
					glm::vec3 axis(0.0f);
					if (m_ActiveAxis == Axis::X) axis = glm::vec3(1, 0, 0);
					else if (m_ActiveAxis == Axis::Y) axis = glm::vec3(0, 1, 0);
					else if (m_ActiveAxis == Axis::Z) axis = glm::vec3(0, 0, 1);

					glm::vec3 cameraRight = glm::normalize(glm::cross(rayDir, axis));
					glm::vec3 planeNormal = glm::normalize(glm::cross(axis, cameraRight));
					glm::vec3 intersection = rayPlaneIntersect(rayOrigin, rayDir, m_GizmoStartPos, planeNormal);

					glm::vec3 toIntersection = intersection - m_GizmoStartPos;
					float currentProjection = glm::dot(toIntersection, axis);

					glm::vec3 toDragStart = m_DragStartIntersection - m_GizmoStartPos;
					float startProjection = glm::dot(toDragStart, axis);

					float scaleFactor = 1.0f;
					if (glm::abs(startProjection) > 0.001f)
					{
						scaleFactor = currentProjection / startProjection;
						scaleFactor = glm::max(scaleFactor, 0.01f);
					}

					glm::vec3 newScale = m_GizmoStartScale;
					if (m_ActiveAxis == Axis::X) newScale.x = m_GizmoStartScale.x * scaleFactor;
					else if (m_ActiveAxis == Axis::Y) newScale.y = m_GizmoStartScale.y * scaleFactor;
					else if (m_ActiveAxis == Axis::Z) newScale.z = m_GizmoStartScale.z * scaleFactor;

					tc.Scale = newScale;
				}
			}
		}

		// End dragging
		if (!mousePressed)
		{
			m_IsUsingGizmo = false;
			m_ActiveAxis = Axis::None;
		}

		return m_IsUsingGizmo;
	}

	void Gizmo::RenderTranslateGizmo(const glm::vec3& origin, Axis hoveredAxis,
									void* drawList, const glm::vec2 viewportBounds[2],
									std::function<glm::vec2(const glm::vec3&)> project)
	{
		ImDrawList* dl = static_cast<ImDrawList*>(drawList);

		float axisLength = 1.0f;
		float lineThickness = 3.0f;
		float planeSize = 0.25f;
		float planeOffset = 0.3f;
		float arrowSize = 12.0f;

		// Colors
		ImU32 xColor = (hoveredAxis == Axis::X) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
		ImU32 yColor = (hoveredAxis == Axis::Y) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 255, 0, 255);
		ImU32 zColor = (hoveredAxis == Axis::Z) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 0, 255, 255);

		// Project points
		glm::vec2 originScreen = project(origin);
		glm::vec2 xScreen = project(origin + glm::vec3(axisLength, 0, 0));
		glm::vec2 yScreen = project(origin + glm::vec3(0, axisLength, 0));
		glm::vec2 zScreen = project(origin + glm::vec3(0, 0, axisLength));

		// Draw arrow helper
		auto drawArrow = [&](const glm::vec2& start, const glm::vec2& end, ImU32 color) {
			dl->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), color, lineThickness);

			glm::vec2 dir = glm::normalize(end - start);
			glm::vec2 perpendicular(-dir.y, dir.x);

			glm::vec2 arrowTip = end;
			glm::vec2 arrowBase = end - dir * arrowSize;
			glm::vec2 arrowLeft = arrowBase + perpendicular * (arrowSize * 0.4f);
			glm::vec2 arrowRight = arrowBase - perpendicular * (arrowSize * 0.4f);

			dl->AddTriangleFilled(
				ImVec2(arrowTip.x, arrowTip.y),
				ImVec2(arrowLeft.x, arrowLeft.y),
				ImVec2(arrowRight.x, arrowRight.y),
				color
			);
		};

		// Draw axes
		drawArrow(originScreen, xScreen, xColor);
		drawArrow(originScreen, yScreen, yColor);
		drawArrow(originScreen, zScreen, zColor);

		// Plane colors
		ImU32 xyColor = (hoveredAxis == Axis::XY) ? IM_COL32(255, 255, 0, 180) : IM_COL32(255, 255, 0, 100);
		ImU32 xzColor = (hoveredAxis == Axis::XZ) ? IM_COL32(255, 0, 255, 180) : IM_COL32(255, 0, 255, 100);
		ImU32 yzColor = (hoveredAxis == Axis::YZ) ? IM_COL32(0, 255, 255, 180) : IM_COL32(0, 255, 255, 100);

		// Draw planes
		auto drawPlane = [&](const glm::vec3& center, const glm::vec3& v1, const glm::vec3& v2, ImU32 color) {
			glm::vec3 p1 = center + (-v1 - v2) * 0.5f;
			glm::vec3 p2 = center + (v1 - v2) * 0.5f;
			glm::vec3 p3 = center + (v1 + v2) * 0.5f;
			glm::vec3 p4 = center + (-v1 + v2) * 0.5f;

			glm::vec2 s1 = project(p1);
			glm::vec2 s2 = project(p2);
			glm::vec2 s3 = project(p3);
			glm::vec2 s4 = project(p4);

			dl->AddQuadFilled(
				ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y),
				ImVec2(s3.x, s3.y), ImVec2(s4.x, s4.y),
				color
			);
		};

		glm::vec3 centerXY = origin + glm::vec3(planeOffset, planeOffset, 0) * axisLength;
		drawPlane(centerXY, glm::vec3(planeSize, 0, 0), glm::vec3(0, planeSize, 0), xyColor);

		glm::vec3 centerXZ = origin + glm::vec3(planeOffset, 0, planeOffset) * axisLength;
		drawPlane(centerXZ, glm::vec3(planeSize, 0, 0), glm::vec3(0, 0, planeSize), xzColor);

		glm::vec3 centerYZ = origin + glm::vec3(0, planeOffset, planeOffset) * axisLength;
		drawPlane(centerYZ, glm::vec3(0, planeSize, 0), glm::vec3(0, 0, planeSize), yzColor);
	}

	void Gizmo::RenderRotateGizmo(const glm::vec3& origin, const glm::vec3& rotation, Axis hoveredAxis,
								 void* drawList, const glm::vec2 viewportBounds[2],
								 std::function<glm::vec2(const glm::vec3&)> project)
	{
		ImDrawList* dl = static_cast<ImDrawList*>(drawList);

		float radius = 1.2f;
		int numSegments = 64;
		float thickness = 2.5f;

		// Colors
		ImU32 xColor = (hoveredAxis == Axis::X) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
		ImU32 yColor = (hoveredAxis == Axis::Y) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 255, 0, 255);
		ImU32 zColor = (hoveredAxis == Axis::Z) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 0, 255, 255);

		glm::quat quat = glm::quat(glm::radians(rotation));
		glm::mat4 rotationMatrix = glm::toMat4(quat);

		auto drawCircle = [&](const glm::vec3& center, const glm::vec3& normal, float radius, ImU32 color) {
			glm::vec3 rotatedNormal = glm::vec3(rotationMatrix * glm::vec4(normal, 0.0f));

			glm::vec3 tangent, bitangent;
			if (glm::abs(rotatedNormal.y) < 0.9f) {
				tangent = glm::normalize(glm::cross(rotatedNormal, glm::vec3(0, 1, 0)));
			}
			else {
				tangent = glm::normalize(glm::cross(rotatedNormal, glm::vec3(1, 0, 0)));
			}
			bitangent = glm::cross(rotatedNormal, tangent);

			for (int i = 0; i < numSegments; i++) {
				float angle1 = (float)i / numSegments * glm::two_pi<float>();
				float angle2 = (float)(i + 1) / numSegments * glm::two_pi<float>();

				glm::vec3 p1 = center + radius * (glm::cos(angle1) * tangent + glm::sin(angle1) * bitangent);
				glm::vec3 p2 = center + radius * (glm::cos(angle2) * tangent + glm::sin(angle2) * bitangent);

				glm::vec2 s1 = project(p1);
				glm::vec2 s2 = project(p2);

				dl->AddLine(ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y), color, thickness);
			}
		};

		drawCircle(origin, glm::vec3(1, 0, 0), radius, xColor);
		drawCircle(origin, glm::vec3(0, 1, 0), radius, yColor);
		drawCircle(origin, glm::vec3(0, 0, 1), radius, zColor);

		// Center quad for screen rotation
		auto& camera = Nebula::Application::Get().GetCamera();
		auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
		if (perspCam)
		{
			float quadSize = 0.15f;
			ImU32 screenColor = (hoveredAxis == Axis::Screen) ? IM_COL32(255, 255, 255, 255) : IM_COL32(200, 200, 200, 150);

			glm::vec3 cameraPos = perspCam->GetPosition();
			glm::vec3 toCamera = glm::normalize(cameraPos - origin);

			glm::vec3 cameraRot = perspCam->GetRotation();
			float pitch = glm::radians(cameraRot.x);
			float yaw = glm::radians(cameraRot.y);

			glm::mat4 camRotMatrix = glm::mat4(1.0f);
			camRotMatrix = glm::rotate(camRotMatrix, yaw, glm::vec3(0, 1, 0));
			camRotMatrix = glm::rotate(camRotMatrix, pitch, glm::vec3(1, 0, 0));

			glm::vec3 up = glm::vec3(camRotMatrix * glm::vec4(0, 1, 0, 0));
			glm::vec3 right = glm::normalize(glm::cross(up, toCamera));
			up = glm::normalize(glm::cross(toCamera, right));

			glm::vec3 p1 = origin + (-right - up) * quadSize;
			glm::vec3 p2 = origin + (right - up) * quadSize;
			glm::vec3 p3 = origin + (right + up) * quadSize;
			glm::vec3 p4 = origin + (-right + up) * quadSize;

			glm::vec2 s1 = project(p1);
			glm::vec2 s2 = project(p2);
			glm::vec2 s3 = project(p3);
			glm::vec2 s4 = project(p4);

			dl->AddQuadFilled(
				ImVec2(s1.x, s1.y), ImVec2(s2.x, s2.y),
				ImVec2(s3.x, s3.y), ImVec2(s4.x, s4.y),
				screenColor
			);
		}
	}

	void Gizmo::RenderScaleGizmo(const glm::vec3& origin, Axis hoveredAxis,
								void* drawList, const glm::vec2 viewportBounds[2],
								std::function<glm::vec2(const glm::vec3&)> project)
	{
		ImDrawList* dl = static_cast<ImDrawList*>(drawList);

		float axisLength = 1.0f;
		float lineThickness = 3.0f;
		float circleRadius = 8.0f;

		// Colors
		ImU32 xColor = (hoveredAxis == Axis::X) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 0, 0, 255);
		ImU32 yColor = (hoveredAxis == Axis::Y) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 255, 0, 255);
		ImU32 zColor = (hoveredAxis == Axis::Z) ? IM_COL32(255, 255, 0, 255) : IM_COL32(0, 0, 255, 255);

		// Project points
		glm::vec2 originScreen = project(origin);
		glm::vec2 xScreen = project(origin + glm::vec3(axisLength, 0, 0));
		glm::vec2 yScreen = project(origin + glm::vec3(0, axisLength, 0));
		glm::vec2 zScreen = project(origin + glm::vec3(0, 0, axisLength));

		// Draw axes
		dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(xScreen.x, xScreen.y), xColor, lineThickness);
		dl->AddCircleFilled(ImVec2(xScreen.x, xScreen.y), circleRadius, xColor);

		dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(yScreen.x, yScreen.y), yColor, lineThickness);
		dl->AddCircleFilled(ImVec2(yScreen.x, yScreen.y), circleRadius, yColor);

		dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(zScreen.x, zScreen.y), zColor, lineThickness);
		dl->AddCircleFilled(ImVec2(zScreen.x, zScreen.y), circleRadius, zColor);

		// Center box for uniform scaling
		float boxSize = 12.0f;
		ImU32 boxColor = (hoveredAxis == Axis::Screen) ? IM_COL32(255, 255, 0, 255) : IM_COL32(255, 255, 255, 200);

		dl->AddRectFilled(
			ImVec2(originScreen.x - boxSize / 2, originScreen.y - boxSize / 2),
			ImVec2(originScreen.x + boxSize / 2, originScreen.y + boxSize / 2),
			boxColor
		);

		dl->AddRect(
			ImVec2(originScreen.x - boxSize / 2, originScreen.y - boxSize / 2),
			ImVec2(originScreen.x + boxSize / 2, originScreen.y + boxSize / 2),
			IM_COL32(0, 0, 0, 150),
			0.0f, 0, 2.0f
		);
	}

	Gizmo::Axis Gizmo::GetHoveredAxis(Nebula::Entity& entity, Mode mode, const glm::vec2& mousePos,
									 const glm::vec2 viewportBounds[2])
	{
		if (!entity || !entity.HasComponent<Nebula::TransformComponent>())
			return Axis::None;

		auto& tc = entity.GetComponent<Nebula::TransformComponent>();
		glm::vec3 origin = tc.Position;

		auto& camera = Nebula::Application::Get().GetCamera();
		auto* perspCam = dynamic_cast<Nebula::PerspectiveCamera*>(&camera);
		if (!perspCam) return Axis::None;

		glm::mat4 viewProj = perspCam->GetProjectionMatrix() * perspCam->GetViewMatrix();
		if (!viewportBounds) return Axis::None;

		float axisLength = 1.0f;
		float hitRadius = 15.0f;

		auto project = [&](const glm::vec3& worldPos) -> glm::vec2 {
			glm::vec4 clip = viewProj * glm::vec4(worldPos, 1.0f);
			if (clip.w == 0.0f) clip.w = 0.001f;
			glm::vec3 ndc = glm::vec3(clip) / clip.w;

			float viewportWidth = viewportBounds[1].x - viewportBounds[0].x;
			float viewportHeight = viewportBounds[1].y - viewportBounds[0].y;

			float x = viewportBounds[0].x + (ndc.x + 1.0f) * 0.5f * viewportWidth;
			float y = viewportBounds[0].y + (1.0f - ndc.y) * 0.5f * viewportHeight;

			return glm::vec2(x, y);
		};

		glm::vec2 originScreen = project(origin);
		glm::vec2 xScreen = project(origin + glm::vec3(axisLength, 0, 0));
		glm::vec2 yScreen = project(origin + glm::vec3(0, axisLength, 0));
		glm::vec2 zScreen = project(origin + glm::vec3(0, 0, axisLength));

		auto distanceToLineSegment = [](const glm::vec2& p, const glm::vec2& a, const glm::vec2& b) -> float {
			glm::vec2 ab = b - a;
			glm::vec2 ap = p - a;
			float abLenSq = glm::dot(ab, ab);
			if (abLenSq == 0.0f) return glm::length(ap);

			float t = glm::clamp(glm::dot(ap, ab) / abLenSq, 0.0f, 1.0f);
			glm::vec2 projection = a + t * ab;
			return glm::distance(p, projection);
		};

		auto isInsideQuad = [](const glm::vec2& p, const glm::vec2& p1, const glm::vec2& p2,
							  const glm::vec2& p3, const glm::vec2& p4) -> bool {
			auto sign = [](const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3) -> float {
				return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
			};

			float d1 = sign(p, p1, p2);
			float d2 = sign(p, p2, p3);
			float d3 = sign(p, p3, p4);
			float d4 = sign(p, p4, p1);

			bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0) || (d4 < 0);
			bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0) || (d4 > 0);

			return !(hasNeg && hasPos);
		};

		// Check plane handles for translate mode
		if (mode == Mode::Translate)
		{
			float planeSize = 0.25f;
			float planeOffset = 0.3f;

			auto checkPlane = [&](const glm::vec3& offset, const glm::vec3& v1, const glm::vec3& v2) -> bool {
				glm::vec3 center = origin + offset * axisLength;
				glm::vec3 p1 = center + (-v1 - v2) * 0.5f;
				glm::vec3 p2 = center + (v1 - v2) * 0.5f;
				glm::vec3 p3 = center + (v1 + v2) * 0.5f;
				glm::vec3 p4 = center + (-v1 + v2) * 0.5f;

				glm::vec2 s1 = project(p1);
				glm::vec2 s2 = project(p2);
				glm::vec2 s3 = project(p3);
				glm::vec2 s4 = project(p4);

				return isInsideQuad(mousePos, s1, s2, s3, s4);
			};

			if (checkPlane(glm::vec3(planeOffset, planeOffset, 0), glm::vec3(planeSize, 0, 0), glm::vec3(0, planeSize, 0)))
				return Axis::XY;
			if (checkPlane(glm::vec3(planeOffset, 0, planeOffset), glm::vec3(planeSize, 0, 0), glm::vec3(0, 0, planeSize)))
				return Axis::XZ;
			if (checkPlane(glm::vec3(0, planeOffset, planeOffset), glm::vec3(0, planeSize, 0), glm::vec3(0, 0, planeSize)))
				return Axis::YZ;
		}

		// Check scale circles and center box
		if (mode == Mode::Scale)
		{
			float circleRadius = 8.0f;
			float boxSize = 12.0f;

			if (mousePos.x >= originScreen.x - boxSize / 2 && mousePos.x <= originScreen.x + boxSize / 2 &&
				mousePos.y >= originScreen.y - boxSize / 2 && mousePos.y <= originScreen.y + boxSize / 2)
			{
				return Axis::Screen;
			}

			if (glm::distance(mousePos, xScreen) <= circleRadius + 5.0f)
				return Axis::X;
			if (glm::distance(mousePos, yScreen) <= circleRadius + 5.0f)
				return Axis::Y;
			if (glm::distance(mousePos, zScreen) <= circleRadius + 5.0f)
				return Axis::Z;
		}

		// Check rotation circles
		if (mode == Mode::Rotate)
		{
			float radius = 1.2f;
			int numSegments = 64;
			float hitThreshold = 10.0f;

			glm::quat quat = glm::quat(glm::radians(tc.Rotation));
			glm::mat4 rotationMatrix = glm::toMat4(quat);

			auto distanceToCircle = [&](const glm::vec3& center, const glm::vec3& normal, float radius) -> float {
				glm::vec3 rotatedNormal = glm::vec3(rotationMatrix * glm::vec4(normal, 0.0f));

				glm::vec3 tangent, bitangent;
				if (glm::abs(rotatedNormal.y) < 0.9f) {
					tangent = glm::normalize(glm::cross(rotatedNormal, glm::vec3(0, 1, 0)));
				}
				else {
					tangent = glm::normalize(glm::cross(rotatedNormal, glm::vec3(1, 0, 0)));
				}
				bitangent = glm::cross(rotatedNormal, tangent);

				float minDist = FLT_MAX;
				for (int i = 0; i < numSegments; i++) {
					float angle1 = (float)i / numSegments * glm::two_pi<float>();
					float angle2 = (float)(i + 1) / numSegments * glm::two_pi<float>();

					glm::vec3 p1 = center + radius * (glm::cos(angle1) * tangent + glm::sin(angle1) * bitangent);
					glm::vec3 p2 = center + radius * (glm::cos(angle2) * tangent + glm::sin(angle2) * bitangent);

					glm::vec2 s1 = project(p1);
					glm::vec2 s2 = project(p2);

					float dist = distanceToLineSegment(mousePos, s1, s2);
					minDist = glm::min(minDist, dist);
				}
				return minDist;
			};

			if (distanceToCircle(origin, glm::vec3(1, 0, 0), radius) < hitThreshold)
				return Axis::X;
			if (distanceToCircle(origin, glm::vec3(0, 1, 0), radius) < hitThreshold)
				return Axis::Y;
			if (distanceToCircle(origin, glm::vec3(0, 0, 1), radius) < hitThreshold)
				return Axis::Z;

			// Check center quad for screen rotation
			float quadSize = 0.15f;
			glm::vec3 cameraPos = perspCam->GetPosition();
			glm::vec3 toCamera = glm::normalize(cameraPos - origin);

			glm::vec3 cameraRot = perspCam->GetRotation();
			float pitch = glm::radians(cameraRot.x);
			float yaw = glm::radians(cameraRot.y);

			glm::mat4 camRotMatrix = glm::mat4(1.0f);
			camRotMatrix = glm::rotate(camRotMatrix, yaw, glm::vec3(0, 1, 0));
			camRotMatrix = glm::rotate(camRotMatrix, pitch, glm::vec3(1, 0, 0));

			glm::vec3 up = glm::vec3(camRotMatrix * glm::vec4(0, 1, 0, 0));
			glm::vec3 right = glm::normalize(glm::cross(up, toCamera));
			up = glm::normalize(glm::cross(toCamera, right));

			glm::vec3 qp1 = origin + (-right - up) * quadSize;
			glm::vec3 qp2 = origin + (right - up) * quadSize;
			glm::vec3 qp3 = origin + (right + up) * quadSize;
			glm::vec3 qp4 = origin + (-right + up) * quadSize;

			glm::vec2 qs1 = project(qp1);
			glm::vec2 qs2 = project(qp2);
			glm::vec2 qs3 = project(qp3);
			glm::vec2 qs4 = project(qp4);

			if (isInsideQuad(mousePos, qs1, qs2, qs3, qs4))
				return Axis::Screen;
		}

		// Check arrow heads for translate mode
		if (mode == Mode::Translate)
		{
			float arrowHitRadius = 15.0f;

			if (glm::distance(mousePos, zScreen) <= arrowHitRadius)
				return Axis::Z;
			if (glm::distance(mousePos, yScreen) <= arrowHitRadius)
				return Axis::Y;
			if (glm::distance(mousePos, xScreen) <= arrowHitRadius)
				return Axis::X;
		}

		// Check axis lines
		if (distanceToLineSegment(mousePos, originScreen, zScreen) < hitRadius)
			return Axis::Z;
		if (distanceToLineSegment(mousePos, originScreen, yScreen) < hitRadius)
			return Axis::Y;
		if (distanceToLineSegment(mousePos, originScreen, xScreen) < hitRadius)
			return Axis::X;

		return Axis::None;
	}

	void Gizmo::RenderDirectionIndicators(Nebula::Entity& entity, const glm::vec3& origin,
										  void* drawList, const glm::vec2 viewportBounds[2],
										  std::function<glm::vec2(const glm::vec3&)> project)
	{
		ImDrawList* dl = static_cast<ImDrawList*>(drawList);
		
		auto& tc = entity.GetComponent<Nebula::TransformComponent>();
		glm::quat quat = glm::quat(glm::radians(tc.Rotation));
		glm::vec3 forward = glm::normalize(quat * glm::vec3(0.0f, 0.0f, -1.0f));
		
		// Directional Light Indicator
		if (entity.HasComponent<Nebula::DirectionalLightComponent>())
		{
			auto& dirLight = entity.GetComponent<Nebula::DirectionalLightComponent>();
			
			// Draw arrow showing light direction
			float arrowLength = 2.0f;
			glm::vec3 arrowEnd = origin + forward * arrowLength;
			
			glm::vec2 originScreen = project(origin);
			glm::vec2 endScreen = project(arrowEnd);
			
			// Draw main line
			ImU32 lightColor = IM_COL32(255, 255, 0, 200); // Yellow
			dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(endScreen.x, endScreen.y), lightColor, 3.0f);
			
			// Draw arrow head
			glm::vec2 dir = glm::normalize(endScreen - originScreen);
			glm::vec2 perpendicular(-dir.y, dir.x);
			
			float arrowHeadSize = 12.0f;
			glm::vec2 arrowTip = endScreen;
			glm::vec2 arrowBase = endScreen - dir * arrowHeadSize;
			glm::vec2 arrowLeft = arrowBase + perpendicular * (arrowHeadSize * 0.4f);
			glm::vec2 arrowRight = arrowBase - perpendicular * (arrowHeadSize * 0.4f);
			
			dl->AddTriangleFilled(
				ImVec2(arrowTip.x, arrowTip.y),
				ImVec2(arrowLeft.x, arrowLeft.y),
				ImVec2(arrowRight.x, arrowRight.y),
				lightColor
			);
			
			// Draw additional rays to show it's a directional light
			for (int i = 0; i < 3; i++)
			{
				float offset = (i - 1) * 0.15f;
				glm::vec3 up = glm::normalize(quat * glm::vec3(0.0f, 1.0f, 0.0f));
				glm::vec3 right = glm::normalize(quat * glm::vec3(1.0f, 0.0f, 0.0f));
				glm::vec3 rayStart = origin + (up * offset * 0.3f) + (right * offset * 0.3f);
				glm::vec3 rayEnd = rayStart + forward * (arrowLength * 0.7f);
				
				glm::vec2 rayStartScreen = project(rayStart);
				glm::vec2 rayEndScreen = project(rayEnd);
				
				dl->AddLine(ImVec2(rayStartScreen.x, rayStartScreen.y), 
						   ImVec2(rayEndScreen.x, rayEndScreen.y), 
						   IM_COL32(255, 255, 0, 120), 1.5f);
			}
		}
		
		// Camera Indicator
		if (entity.HasComponent<Nebula::CameraComponent>())
		{
			// Draw camera frustum visualization
			float frustumLength = 1.5f;
			float frustumSize = 0.6f;
			
			glm::vec3 up = glm::normalize(quat * glm::vec3(0.0f, 1.0f, 0.0f));
			glm::vec3 right = glm::normalize(quat * glm::vec3(1.0f, 0.0f, 0.0f));
			
			// Frustum far corners
			glm::vec3 farCenter = origin + forward * frustumLength;
			glm::vec3 farTL = farCenter + up * frustumSize - right * frustumSize;
			glm::vec3 farTR = farCenter + up * frustumSize + right * frustumSize;
			glm::vec3 farBL = farCenter - up * frustumSize - right * frustumSize;
			glm::vec3 farBR = farCenter - up * frustumSize + right * frustumSize;
			
			// Project to screen
			glm::vec2 originScreen = project(origin);
			glm::vec2 farTLScreen = project(farTL);
			glm::vec2 farTRScreen = project(farTR);
			glm::vec2 farBLScreen = project(farBL);
			glm::vec2 farBRScreen = project(farBR);
			
			ImU32 cameraColor = IM_COL32(0, 200, 255, 200); // Cyan
			
			// Draw frustum lines from camera to far corners
			dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(farTLScreen.x, farTLScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(farTRScreen.x, farTRScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(farBLScreen.x, farBLScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(originScreen.x, originScreen.y), ImVec2(farBRScreen.x, farBRScreen.y), cameraColor, 2.0f);
			
			// Draw far rectangle
			dl->AddLine(ImVec2(farTLScreen.x, farTLScreen.y), ImVec2(farTRScreen.x, farTRScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(farTRScreen.x, farTRScreen.y), ImVec2(farBRScreen.x, farBRScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(farBRScreen.x, farBRScreen.y), ImVec2(farBLScreen.x, farBLScreen.y), cameraColor, 2.0f);
			dl->AddLine(ImVec2(farBLScreen.x, farBLScreen.y), ImVec2(farTLScreen.x, farTLScreen.y), cameraColor, 2.0f);
			
			// Draw camera body (small rectangle at origin)
			float bodySize = 0.15f;
			glm::vec3 bodyTL = origin + up * bodySize - right * bodySize;
			glm::vec3 bodyTR = origin + up * bodySize + right * bodySize;
			glm::vec3 bodyBL = origin - up * bodySize - right * bodySize;
			glm::vec3 bodyBR = origin - up * bodySize + right * bodySize;
			
			glm::vec2 bodyTLScreen = project(bodyTL);
			glm::vec2 bodyTRScreen = project(bodyTR);
			glm::vec2 bodyBLScreen = project(bodyBL);
			glm::vec2 bodyBRScreen = project(bodyBR);
			
			dl->AddQuadFilled(
				ImVec2(bodyTLScreen.x, bodyTLScreen.y),
				ImVec2(bodyTRScreen.x, bodyTRScreen.y),
				ImVec2(bodyBRScreen.x, bodyBRScreen.y),
				ImVec2(bodyBLScreen.x, bodyBLScreen.y),
				IM_COL32(0, 200, 255, 150)
			);
		}
	}

}

