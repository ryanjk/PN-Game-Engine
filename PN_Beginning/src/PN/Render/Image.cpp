#include "PN/Render/Image.h"

pn::Image::Image() {}

pn::Image::Image(PixelContainer&& pixels, Dimension width, Dimension height) 
	: m_pixels(std::move(pixels)), m_width(width), m_height(height)
{}

pn::Image::Image(pn::Image&& image) 
	: m_pixels(std::move(image.m_pixels)), m_width(std::move(image.m_width)), m_height(std::move(image.m_height))
{}

pn::Image& pn::Image::operator=(pn::Image&& image) {
	m_pixels = std::move(image.m_pixels);
	m_width = image.m_width;
	m_height = image.m_height;

	return *this;
}

const PixelContainer& pn::Image::getPixels() const {
	return m_pixels;
}

Dimension pn::Image::getWidth() const {
	return m_width;
}

Dimension pn::Image::getHeight() const {
	return m_height;
}