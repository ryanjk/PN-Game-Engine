#ifndef PN_IMAGE_H
#define PN_IMAGE_H

#include <vector>

using PixelFormat = unsigned char;
using PixelContainer = std::vector < PixelFormat > ;
using Dimension = unsigned int;

namespace pn {
	class Image {
	public:
		Image();
		Image(PixelContainer&& pixels, Dimension width, Dimension height);
		Image(const Image& image) = default;
		Image(Image&& image) _NOEXCEPT;

		Image& operator=(Image&& image) _NOEXCEPT;

		const PixelContainer& getPixels() const;
		Dimension getWidth() const;
		Dimension getHeight() const;

	private:
		PixelContainer m_pixels;
		Dimension m_width;
		Dimension m_height;
	};
}

#endif