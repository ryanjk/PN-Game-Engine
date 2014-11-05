#ifndef PN_MATH_H
#define PN_MATH_H

#define GLM_FORCE_INLINE
#define GLM_FORCE_RADIANS
#define GLM_SWIZZLE

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <vector>

using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;

void print_vec3(vec3 v);
void print_mat4(mat4 m);

glm::fquat make_quat(const vec3& axis, float angle);



#endif