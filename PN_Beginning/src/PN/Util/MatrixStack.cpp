#include "PN/Util/MatrixStack.h"

#include <iostream>

pn::MatrixStack::MatrixStack() : m_stack() {}

void pn::MatrixStack::push(const mat4& matrix) {
	m_stack.push_back(matrix);
}

void pn::MatrixStack::pop() {
	m_stack.pop_back();
}

mat4 pn::MatrixStack::multiply() {
	mat4 result;
	for (std::size_t i = 0; i < m_stack.size(); i++) {
		result = result * m_stack[i];
	}
	return result;
}

mat4 pn::MatrixStack::operator*(const mat4& matrix) {
	mat4 result;
	for (std::size_t i = 0; i < m_stack.size(); i++) {
		result = result * m_stack[i];
	}
	return result * matrix;
}

bool pn::MatrixStack::isEmpty() const {
	return m_stack.empty();
}