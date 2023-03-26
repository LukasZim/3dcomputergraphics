#include "texture.h"
#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <fmt/format.h>
DISABLE_WARNINGS_POP()

Texture::Texture(std::filesystem::path filePath)
{
	// Load image from disk to CPU memory.
	// Image class is defined in <framework/image.h>
	Image cpuTexture{ filePath };
	initialize(cpuTexture);
}

Texture::Texture(Image cpuTexture) {
	initialize(cpuTexture);
}

void Texture::initialize(Image cpuTexture) {
	glCreateTextures(GL_TEXTURE_2D, 1, &m_texture);
	glTextureStorage2D(m_texture, 1, GL_RGB8, cpuTexture.width, cpuTexture.height);

	// Upload pixels into the GPU texture.
	glTextureSubImage2D(m_texture, 0, 0, 0, cpuTexture.width, cpuTexture.height, GL_RGB, GL_FLOAT, cpuTexture.pixels.data());

	// Set behavior for when texture coordinates are outside the [0, 1] range.
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(m_texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set interpolation for texture sampling (GL_NEAREST for no interpolation).
	glTextureParameteri(m_texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(Texture&& other)
	: m_texture(other.m_texture)
{
	other.m_texture = INVALID;
}

Texture::~Texture()
{
	if (m_texture != INVALID)
		glDeleteTextures(1, &m_texture);
}

void Texture::bind(GLint textureSlot)
{
	glActiveTexture(textureSlot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
