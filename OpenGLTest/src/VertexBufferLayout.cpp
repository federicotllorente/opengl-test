#include "VertexBufferLayout.h"

void VertexBufferLayout::Push(unsigned int type, unsigned int count)
{
	bool isNormalized = type == GL_UNSIGNED_BYTE ? GL_TRUE : GL_FALSE;
	m_Elements.push_back({ type, count, isNormalized });
	m_Stride += count * VertexBufferElement::GetSizeOfType(type);
}
