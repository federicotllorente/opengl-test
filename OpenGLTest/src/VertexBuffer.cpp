#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	/* Generate a new buffer */
	GL_CALL(glGenBuffers(1, &m_RendererID));
	/* Bind it */
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	/* Provide data to it */
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	GL_CALL(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::Unbind() const
{
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
