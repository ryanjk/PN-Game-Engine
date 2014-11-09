#ifndef PN_MATRIX_STACK
#define PN_MATRIX_STACK

#include "PN/Util/Math.h"

namespace pn {
	class MatrixStack {
	public:
		MatrixStack();
		void push(const mat4& matrix);
		void pop();

		bool isEmpty() const;

		mat4 multiply();
		mat4 operator*(const mat4& matrix);

	private:
		std::vector<mat4> m_stack;
	};
}

#endif