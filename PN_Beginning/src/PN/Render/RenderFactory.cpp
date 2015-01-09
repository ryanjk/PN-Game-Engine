#include "PN/Render/RenderFactory.h"

#include "PN/Physics/BoundingContainer/BoundingAABB.h"
#include "PN/Physics/BoundingContainer/BoundingOBB.h"
#include "PN/Physics/BoundingContainer/BoundingSphere.h"

#include "LodePNG/lodepng.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using PixelFormat = unsigned char;
using PixelContainer = std::vector < PixelFormat >;
using Dimension = unsigned int;

pn::Image pn::RenderFactory::makeFromPNG(const char* filename) {
	PixelContainer pixels;
	Dimension width, height;

	PixelContainer buffer;

	lodepng::load_file(buffer, filename);

	lodepng::State state;

	auto error = lodepng::decode(pixels, width, height, state, buffer);
	if (error) {
		std::cout << "LodePNG: Could not load file '" << filename << "' : " << lodepng_error_text(error) << std::endl;
	}

	Image image;

	glGenTextures(1, &image.m_tbo);
	glBindTexture(GL_TEXTURE_2D, image.m_tbo);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenSamplers(1, &image.m_sampler);
	glSamplerParameteri(image.m_sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameteri(image.m_sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(image.m_sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	return image;
}

pn::Mesh pn::RenderFactory::loadMeshFromObj(const char* filename) {
	/*
	Modified from 2010 Wouter Lindenhof (http://limegarden.net)
	*/
	
	static const std::string TOKEN_VERTEX_POS = "v";
	static const std::string TOKEN_VERTEX_NOR = "vn";
	static const std::string TOKEN_VERTEX_TEX = "vt";
	static const std::string TOKEN_FACE = "f";
	static const std::string BOUNDING_AABB = "AABB";
	static const std::string BOUNDING_OBB = "OBB";
	static const std::string BOUNDING_SPHERE = "SPHERE";
	
	std::vector<GLfloat> temp_vertices;
	temp_vertices.reserve(50000 * 3);

	std::vector<GLfloat> temp_normals;
	temp_normals.reserve(50000 * 3);

	std::vector<GLfloat> temp_texes;
	temp_texes.reserve(3 * 2);

	std::vector<GLuint> v_indices;
	v_indices.reserve(300000);

	std::vector<GLuint> vn_indices;
	vn_indices.reserve(300000);

	std::vector<GLuint> vt_indices;
	vt_indices.reserve(300000);

	std::shared_ptr<pn::BoundingContainer> bounding_container_ptr = nullptr;

	std::ifstream filestream;
	filestream.open(filename);

	if (!filestream) {
		std::cout << "OBJ Loader: Could not open file " << filename << std::endl;
	}

	std::string line_stream;
	while (std::getline(filestream, line_stream)){
		std::stringstream str_stream(line_stream);
		std::string type_str;
		str_stream >> type_str;
		if (type_str == TOKEN_VERTEX_POS){
			GLfloat pos_x, pos_y, pos_z;
			str_stream >> pos_x >> pos_y >> pos_z;
			temp_vertices.push_back(pos_x);
			temp_vertices.push_back(pos_y);
			temp_vertices.push_back(pos_z);
		}
		else if (type_str == TOKEN_VERTEX_TEX){
			GLfloat tex_x, tex_y;
			str_stream >> tex_x >> tex_y;
			temp_texes.push_back(tex_x);
			temp_texes.push_back(tex_y);
		}
		else if (type_str == TOKEN_VERTEX_NOR){
			GLfloat nor_x, nor_y, nor_z;
			str_stream >> nor_x >> nor_y >> nor_z;
			temp_normals.push_back(nor_x);
			temp_normals.push_back(nor_y);
			temp_normals.push_back(nor_z);
		}
		else if (type_str == TOKEN_FACE){
			GLuint i1 = 0, i2 = 0, i3 = 0, ni1 = 0, ni2 = 0, ni3 = 0;

				GLuint ti1, ti2, ti3;
				sscanf_s(line_stream.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&i1,
					&ti1,
					&ni1,
					&i2,
					&ti2,
					&ni2,
					&i3,
					&ti3,
					&ni3);

				v_indices.push_back(i1 - 1);
				v_indices.push_back(i2 - 1);
				v_indices.push_back(i3 - 1);

				vn_indices.push_back(ni1 - 1);
				vn_indices.push_back(ni2 - 1);
				vn_indices.push_back(ni3 - 1);

				vt_indices.push_back(ti1 - 1);
				vt_indices.push_back(ti2 - 1);
				vt_indices.push_back(ti3 - 1);

		}
		else if (type_str == BOUNDING_AABB) {
			float bb_length, bb_height, bb_width;
			str_stream >> bb_length >> bb_height >> bb_width;
			bounding_container_ptr = std::make_shared<pn::BoundingAABB>(bb_length, bb_width, bb_height);
		}
		else if (type_str == BOUNDING_OBB) {
			float bb_length, bb_height, bb_width;
			str_stream >> bb_length >> bb_height >> bb_width;
			bounding_container_ptr = std::make_shared<pn::BoundingOBB>(bb_length, bb_width, bb_height);
		}
		else if (type_str == BOUNDING_SPHERE) {
			float bs_radius;
			str_stream >> bs_radius;
			bounding_container_ptr = std::make_shared<pn::BoundingSphere>(bs_radius);
		}
	}
	
	const unsigned int v_size = 3 * v_indices.size();
	const unsigned int vn_size = 3 * vn_indices.size();
	const unsigned int vt_size = 2 * vt_indices.size();

	std::vector<GLfloat> vertices;

	
	vertices.reserve(v_size + vn_size + vt_size);

	unsigned int current_vertex = 0;
	for (auto i : v_indices) {
		vertices.push_back(temp_vertices[i * 3]);
		vertices.push_back(temp_vertices[i * 3 + 1]);
		vertices.push_back(temp_vertices[i * 3 + 2]);
	}

	for (auto i : vn_indices) {
		vertices.push_back(temp_normals[i * 3]);
		vertices.push_back(temp_normals[i * 3 + 1]);
		vertices.push_back(temp_normals[i * 3 + 2]);
	}

	for (auto i : vt_indices) {
		vertices.push_back(temp_texes[i * 2]);
		vertices.push_back(temp_texes[i * 2 + 1]);
	}

	// Explicit closing of the file 
	filestream.close();

	pn::Mesh mesh;

	glGenVertexArrays(1, &mesh.m_vao);
	glBindVertexArray(mesh.m_vao);

	glGenBuffers(1, &mesh.m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	int offset = v_size * sizeof(GLfloat);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (char*)(offset));
	offset += vn_size * sizeof(GLfloat);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (char*)(offset));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mesh.m_num_vertices = v_size / 3;
	mesh.setBoundingContainer(bounding_container_ptr);

	return mesh;
}