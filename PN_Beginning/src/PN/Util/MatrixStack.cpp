#include "PN/Util/MatrixStack.h"

#include <iostream>

pn::MatrixStack::MatrixStack() : m_stack() {}

void pn::MatrixStack::push(mat4 matrix) {
	m_stack.push_back(matrix);
}

void pn::MatrixStack::pop() {
	m_stack.pop_back();
}

mat4 pn::MatrixStack::multiply() {
	mat4 result = mat4();
	for (int i = m_stack.size() - 1; i >= 0; i--) {
		result = m_stack[i] * result;
	}
	return result;
}

mat4 pn::MatrixStack::operator*(const mat4& matrix) {
	mat4 result = matrix;
	for (std::size_t i = m_stack.size() - 1; i >= 0; i--) {
		result = m_stack[i] * result;
	}
	return result;
}

bool pn::MatrixStack::isEmpty() const {
	return m_stack.empty();
}