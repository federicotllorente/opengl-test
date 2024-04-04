#pragma once

#include "Test.h"
#include "Renderer.h"
#include "AppWindow.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace test
{
	class TestSquare : public Test
	{
	public:
		TestSquare(float size, glm::vec2 initialCoord);
		~TestSquare();

		void OnUpdate(float deltaTime) override;
		void OnRender(Renderer renderer);
		void OnImGuiRender(ImGuiIO& io);

	private:
		float m_Size;
		unsigned int m_Indices[6] = {
			0, 1, 2,
			2, 3, 0
		};

		Shader m_Shader = Shader("res/shaders/BasicWithTexture.shader");
		VertexArray m_VertexArray;
		IndexBuffer m_IndexBuffer = IndexBuffer(m_Indices, 6);

		int m_ActiveTexture = 1; // Save the state to switch from one to another - 0: cat - 1: logo
		Texture m_CatTexture = Texture("res/textures/cat.png");
		Texture m_LogoTexture = Texture("res/textures/opengl-logo.png");

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		
		glm::vec3 m_TranslationA;
		glm::vec3 m_TranslationB;
	};
}
