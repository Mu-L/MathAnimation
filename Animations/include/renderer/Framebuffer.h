#ifndef MATH_ANIM_FRAMEBUFFER_H
#define MATH_ANIM_FRAMEBUFFER_H
#include "core.h"

#include "renderer/Texture.h"

namespace MathAnim
{
	enum class ByteFormat;
	struct Pixel
	{
		uint8 r;
		uint8 g;
		uint8 b;
	};

	struct Framebuffer
	{
		uint32 fbo;
		int32 width;
		int32 height;

		// Depth/Stencil attachment (optional)
		uint32 rbo;
		ByteFormat depthStencilFormat;
		bool includeDepthStencil;

		// Color attachments
		std::vector<Texture> colorAttachments;
		Texture depthStencilBuffer;

		void bind() const;
		void unbind() const;
		void clearColorAttachmentUint32(int colorAttachment, uint32 clearColor[4]) const;
		// Clear the RG values of the color attachment using the HIGH LOW 
		// values of a uint64 value
		void clearColorAttachmentUint64(int colorAttachment, uint64 clearColor) const;
		void clearColorAttachmentRgb(int colorAttachment, const glm::vec3& clearColor) const;
		void clearColorAttachmentRgb(int colorAttachment, const Vec3& clearColor) const;
		void clearColorAttachmentRgba(int colorAttachment, const Vec4& clearColor) const;
		void clearDepthStencil() const;

		uint32 readPixelUint32(int colorAttachment, int x, int y) const;
		uint64 readPixelUint64(int colorAttachment, int x, int y) const;
		Pixel* readAllPixelsRgb8(int colorAttachment, bool flipVerticallyOnLoad = false) const;
		void freePixels(Pixel* pixels) const;
		const Texture& getColorAttachment(int index) const;

		void regenerate();
		void destroy(bool clearColorAttachmentSpecs = true);
	};

	class FramebufferBuilder
	{
	public:
		FramebufferBuilder(uint32 width, uint32 height);
		Framebuffer generate();

		FramebufferBuilder& includeDepthStencil();
		FramebufferBuilder& addColorAttachment(const Texture& textureSpec); // TODO: The order the attachments are added will be the index they get (change this in the future too...?)

	private:
		Framebuffer framebuffer;
	};
}

#endif