#include "TestSombrero.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#include <vector>

namespace test
{
	TestSombrero::TestSombrero()
		: m_Color { 1.0f, 1.0f, 1.0f, 1.0f }
	{
		std::vector<float> vertices;
		std::vector<unsigned int> lineEndpointIndices;

		const std::size_t vertexCountPerSide = 60;
		
		const std::size_t coordinateCount = vertexCountPerSide * vertexCountPerSide * 3;
		const std::size_t lineEndpointCount = vertexCountPerSide * (vertexCountPerSide - 1) * 4;
		
		const float vertexStep = 2.0 / (vertexCountPerSide - 1);

		vertices.resize(coordinateCount);
		lineEndpointIndices.resize(lineEndpointCount);

		for (std::size_t row = 0; row < vertexCountPerSide; ++row)
		{
			for (std::size_t col = 0; col < vertexCountPerSide; ++col)
			{
				const float x = -1.0f + col * vertexStep;
				const float y = -1.0f + row * vertexStep;
				const float z = m_F(x, y);

				const int vertexStartIndex = (row * vertexCountPerSide + col) * 3;

				vertices[vertexStartIndex + 0] = x;
				vertices[vertexStartIndex + 1] = y;
				vertices[vertexStartIndex + 2] = z;
			}
		}

		std::size_t lineEndpointIndex = 0;

		for (std::size_t row = 0; row < vertexCountPerSide; ++row)
		{
			for (std::size_t col = 0; col < vertexCountPerSide - 1; ++col)
			{
				lineEndpointIndices[lineEndpointIndex++] = row * vertexCountPerSide + col;
				lineEndpointIndices[lineEndpointIndex++] = row * vertexCountPerSide + col + 1;
			}
		}

		for (std::size_t row = 0; row < vertexCountPerSide - 1; ++row)
		{
			for (std::size_t col = 0; col < vertexCountPerSide; ++col)
			{
				lineEndpointIndices[lineEndpointIndex++] = row * vertexCountPerSide + col;
				lineEndpointIndices[lineEndpointIndex++] = (row + 1) * vertexCountPerSide + col;
			}
		}

		/* Create a new vertex buffer */
		VertexBuffer vb(vertices.data(), vertices.size() * sizeof(float));

		/* Create vertex buffer layout */
		VertexBufferLayout layout;
		layout.Push(GL_FLOAT, 3);

		/* Add vertex buffer to VAO */
		m_VertexArray.AddBuffer(vb, layout);

		/* Create IBO */
		m_IndexBuffer = new IndexBuffer(lineEndpointIndices.data(), lineEndpointIndices.size());

		/* MVP matrices */
		//m_ProjectionMatrix = glm::mat4(glm::ortho(0.0f, 900.0f, 0.0f, 900.0f, -1.0f, 1.0f));
		//m_ViewMatrix = glm::mat4(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)));

		m_Shader.Bind();
		m_Shader.SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);

		/* Unbind everything */
		m_VertexArray.Unbind();
		m_Shader.Unbind();
		vb.Unbind();
		m_IndexBuffer->Unbind();
	}

	TestSombrero::~TestSombrero()
	{
		delete m_IndexBuffer;
		m_IndexBuffer = nullptr;
	}

	void TestSombrero::OnUpdate(float deltaTime)
	{
	}

	void TestSombrero::OnRender(Renderer renderer)
	{
		m_Shader.Bind();

		m_Shader.SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		if (m_IsAnimationOn)
		{
			if (m_AngleZ >= 180.0f)
				m_AngleZ = -180.0f;
			
			m_AngleZ += m_Diff;
		}
		
		modelMatrix = glm::rotate(modelMatrix, glm::radians(m_AngleX), glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(m_AngleZ), glm::vec3(0.0f, 0.0f, 1.0f));

		//glm::mat4 mvp = m_ProjectionMatrix * m_ViewMatrix * modelMatrix;
		m_Shader.SetUniformMat4f("u_MVP", modelMatrix);

		renderer.Draw(m_VertexArray, m_IndexBuffer, m_Shader, GL_LINES);
	}

	void test::TestSombrero::OnImGuiRender(ImGuiIO& io)
	{
		ImGui::Begin("Debug");

		ImGui::SliderFloat("Rotation X", &m_AngleX, -90.0f, 90.0f);
		ImGui::SliderFloat("Rotation Z", &m_AngleZ, -180.0f, 180.0f);

		if (ImGui::Button("Start/stop animation"))
		{
			if (m_IsAnimationOn)
				m_IsAnimationOn = false;
			else
				m_IsAnimationOn = true;
		}

		if (ImGui::Button("Reset rotation X"))
			m_AngleX = -60.0f;

		if (ImGui::Button("Reset rotation Z"))
			m_AngleZ = 0.0f;

		ImGui::ColorPicker4("Color", m_Color);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
}
