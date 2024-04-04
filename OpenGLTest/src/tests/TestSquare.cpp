#include "TestSquare.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

namespace test
{
	test::TestSquare::TestSquare(float size, glm::vec2 initialCoord)
		: m_Size(size),
		m_TranslationA(100, 100, 0),
		m_TranslationB(200, 200, 0)
	{
		float verticesData[] = {
			-(size / 2), -(size / 2), 0.0f, 0.0f,
			size / 2, -(size / 2), 1.0f, 0.0f,
			size / 2, size / 2, 1.0f, 1.0f,
			-(size / 2), size / 2, 0.0f, 1.0f
		};
		
		/* Create a new vertex buffer */
		VertexBuffer vb(verticesData, 4 * 4 * sizeof(float));

		/* Create vertex buffer layout */
		VertexBufferLayout layout;
		layout.Push(GL_FLOAT, 2);
		layout.Push(GL_FLOAT, 2);

		/* Add vertex buffer to VAO */
		m_VertexArray.AddBuffer(vb, layout);

		/* MVP matrices */
		m_ProjectionMatrix = glm::mat4(glm::ortho(0.0f, (float)WindowWidth, 0.0f, (float)WindowHeight, -1.0f, 1.0f)); // Maps what the "camera" sees to NDC (Normalized device coordinate), taking care of aspect ratio and perspective
		m_ViewMatrix = glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))); // Defines position and orientation of the "camera"

		m_Shader.Bind();

		/* Bind it and set a 1-integer uniform to the shader for the texture */
		m_LogoTexture.Bind();
		m_Shader.SetUniform1i("u_Texture", 0);

		/* Unbind everything */
		m_VertexArray.Unbind();
		m_Shader.Unbind();
		vb.Unbind();
		m_IndexBuffer.Unbind();
	}

	test::TestSquare::~TestSquare()
	{
	}

	void test::TestSquare::OnUpdate(float deltaTime)
	{
	}

	void test::TestSquare::OnRender(Renderer renderer)
	{
		m_Shader.Bind();
		// To make these two draw calls more robust, makes sense to bind the shader right before setting the uniform, just if it's not bound yet
		// This validation could be made in a more complex solution for managing shaders, but for this example doesn't make much sense to do it now

		{
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_TranslationA); // Defines position, rotation and scale of the vertices of the model in the world
			glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * modelMatrix;
			m_Shader.SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(m_VertexArray, m_IndexBuffer, m_Shader);
		}

		{
			glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * modelMatrix;
			m_Shader.SetUniformMat4f("u_MVP", mvp);
			renderer.Draw(m_VertexArray, m_IndexBuffer, m_Shader);
		}
	}

	void test::TestSquare::OnImGuiRender(ImGuiIO& io)
	{
		ImGui::Begin("Debug");

		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -(float)WindowWidth, (float)WindowWidth);
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -(float)WindowWidth, (float)WindowWidth);

		if (ImGui::Button("Reset translation A"))
			m_TranslationA = glm::vec3(0, 0, 0);

		if (ImGui::Button("Reset translation B"))
			m_TranslationB = glm::vec3(0, 0, 0);

		if (ImGui::Button("Change texture")) {
			switch (m_ActiveTexture)
			{
			case 0: // cat
			{
				m_LogoTexture.Bind();
				m_Shader.SetUniform1i("u_Texture", 0);
				m_ActiveTexture = 1;
				break;
			}
			case 1: // logo
			{
				m_CatTexture.Bind();
				m_Shader.SetUniform1i("u_Texture", 0);
				m_ActiveTexture = 0;
				break;
			}
			default:
				break;
			}
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
}
