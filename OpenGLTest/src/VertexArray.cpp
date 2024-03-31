#include "VertexArray.h"
#include "GLHandleError.h"

VertexArray::VertexArray()
{
	GL_CALL(glGenVertexArrays(1, &m_Renderer_ID));
}

VertexArray::~VertexArray()
{
	GL_CALL(glDeleteVertexArrays(1, &m_Renderer_ID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	/* Bind the vertex array */
	Bind();

	/* Bind the vertex buffer */
	vb.Bind();

	/* Set up the layout */
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];

		GL_CALL(glEnableVertexAttribArray(i));
		GL_CALL(glVertexAttribPointer(
			i,
			element.count,
			element.type,
			element.isNormalized,
			layout.GetStride(),
			(const void*)offset
		));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GL_CALL(glBindVertexArray(m_Renderer_ID));
}

void VertexArray::Unbind() const
{
	GL_CALL(glBindVertexArray(0));
}
