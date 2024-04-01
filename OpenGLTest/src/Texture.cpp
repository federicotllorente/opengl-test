#include "Texture.h"
#include "vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& filepath)
	: m_RendererID(0), m_Filepath(filepath), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1); // Flip vertically since (0; 0) is the bottom left corner for OpenGL
	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4); // 4 is 'desired channels' (RGBA)

	/* Generate and bind a new texture */
	GL_CALL(glGenTextures(1, &m_RendererID));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	/* Set parameters ('settings') for the texture */
	
	// Minification filter - for areas that are smaller than the texture size
	GL_CALL(glTexParameteri(
		GL_TEXTURE_2D, // Target
		GL_TEXTURE_MIN_FILTER, // Parameter name
		GL_LINEAR // Parameter value
	));

	// Magnification filter - for areas that are larger than the texture size
	GL_CALL(glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MAG_FILTER,
		GL_LINEAR
	));

	// Wrap params
	GL_CALL(glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, // Horizontal wrap
		GL_CLAMP_TO_EDGE
	));
	GL_CALL(glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_WRAP_S, // Vertical wrap
		GL_CLAMP_TO_EDGE
	));

	/* Send to OpenGL our data */
	GL_CALL(glTexImage2D(
		GL_TEXTURE_2D, // Target
		0, // Level
		GL_RGBA8, // Internal format - how OpenGL will store a texture data
		m_Width,
		m_Height,
		0, // Border
		GL_RGBA, // Format - the format of the data we're providing to OpenGL
		GL_UNSIGNED_BYTE, // Type
		m_LocalBuffer // Pointer to 'pixels' = Our data :)
	));

	/* Unbind texture */
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
	
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
	GL_CALL(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
