#include "Renderer.h"
#include "GLHandleError.h"

void Renderer::Clear() const
{
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, Shader& shader) const
{
	/* Re-bind shader */
	shader.Bind();

	/* Re-bind VAO */
	va.Bind();

	/* Re-bind index buffer */
	ib.Bind();

	/* Draw */
	GL_CALL(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}
