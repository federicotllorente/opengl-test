#pragma once

#include "Test.h"
#include "Renderer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace test
{
	class TestSombrero : public Test
	{
	public:
		TestSombrero();
		~TestSombrero();

		void OnUpdate(float deltaTime) override;
		void OnRender(Renderer renderer);
		void OnImGuiRender(ImGuiIO& io);

	private:
		Shader m_Shader = Shader("res/shaders/Sombrero.shader");
		VertexArray m_VertexArray;
		IndexBuffer* m_IndexBuffer = nullptr;

		float m_AngleX = -60.0f;
		float m_AngleZ = 0.0f;
		float m_Color[4];
		bool m_IsAnimationOn = true;
		float m_Diff = 0.07f;

		static float m_F(float x, float y)
		{
			constexpr float epsilon = 0.0001f;
			constexpr float k = 10.0f;
			if (std::abs(x) < epsilon && std::abs(y) < epsilon)
				return 1.0f;
			else
				return sin(sqrt(pow(x * k, 2) + pow(y * k, 2))) / sqrt(pow(x * k, 2) + pow(y * k, 2)); // sombrero equation
		}

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
	};
}
