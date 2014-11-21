#include "PN/Render/Image.h"

#include <iostream>

pn::Image::Image() {}

pn::Image::~Image() {

}

const GLuint& pn::Image::getTBO() const {
	return m_tbo;
}

const GLuint& pn::Image::getSamplerObject() const {
	return m_sampler;
}