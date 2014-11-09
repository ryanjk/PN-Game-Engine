#include "PN/Util/ResourceManager.h"
#include "PN/Util/FileType.h"
#include "PN/Util/PString.h"

#include "PN/Render/RenderFactory.h"

#include "json/json.h"

#include <iostream>
#include <fstream>
#include <mutex>
#include <cassert>

std::mutex imageMutex;
std::mutex meshMutex;

static const pn::PString IMAGE_FOLDER_PATH = "resource/image/";
static const pn::PString MESH_FOLDER_PATH = "resource/mesh/";
static const pn::PString SHADER_FOLDER_PATH = "resource/shader/";
static const pn::PString SHADER_PROGRAM_FOLDER_PATH = "resource/shaderProgram/";

pn::ResourceManager::ResourceManager() {}

void pn::ResourceManager::startUp() {}

void pn::ResourceManager::shutdown() {}

/*
For each resource to load, only load if not already in the map
*/
void pn::ResourceManager::load(const PString& filename) {

	auto filetype = pn::FileType::toFileTypeEnum(filename);

	switch (filetype) {
	case pn::FileTypeEnum::PNG: {
		if (m_imageMap.find(filename.getHash()) == m_imageMap.end()) {
			Image image = pn::RenderFactory::makeFromPNG((IMAGE_FOLDER_PATH + filename).c_str());
			m_imageMap.insert({ filename.getHash(), std::move(image) });
			m_used_resource_filenames.insert(filename.getText());
		}
	}
		break;
	case pn::FileTypeEnum::OBJ: {
		if (m_meshMap.find(filename.getHash()) == m_meshMap.end()) {
			Mesh mesh = pn::RenderFactory::loadMeshFromObj((MESH_FOLDER_PATH + filename).c_str());
			m_meshMap.insert({ filename.getHash(), std::move(mesh) });
			m_used_resource_filenames.insert(filename.getText());
		}
	}
		break;
	case pn::FileTypeEnum::VERTEX_SHADER: {
		if (m_vshaderMap.find(filename.getHash()) == m_vshaderMap.end()) {
			GLuint shader = glCreateShader(GL_VERTEX_SHADER);

			std::string shaderText = readShaderFile((SHADER_FOLDER_PATH + filename).c_str());
			const char* shader_cStr = shaderText.c_str();

			GLint result = GL_FALSE;
			int logLength;

			glShaderSource(shader, 1, &shader_cStr, NULL);
			glCompileShader(shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			bool isError = logLength > 1;
			if (isError) {
				std::vector<char> vertShaderError(logLength);
				glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
				std::cout << &vertShaderError[0] << std::endl;
			}
			m_vshaderMap.insert({ filename.getHash(), shader });
		}
	}
		break;
	case pn::FileTypeEnum::FRAGMENT_SHADER: {
		if (m_fshaderMap.find(filename.getHash()) == m_fshaderMap.end()) {
			GLuint shader = glCreateShader(GL_FRAGMENT_SHADER);

			std::string shaderText = readShaderFile((SHADER_FOLDER_PATH + filename).c_str());
			const char* shader_cStr = shaderText.c_str();

			GLint result = GL_FALSE;
			int logLength;

			glShaderSource(shader, 1, &shader_cStr, NULL);
			glCompileShader(shader);

			glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
			bool isError = logLength > 1;
			if (isError) {
				std::vector<char> vertShaderError(logLength);
				glGetShaderInfoLog(shader, logLength, NULL, &vertShaderError[0]);
				std::cout << &vertShaderError[0] << std::endl;
			}
			m_fshaderMap.insert({ filename.getHash(), shader });
		}
	}
		break;
	case pn::FileTypeEnum::SHADER_PROGRAM: {
		if (m_shaderProgramMap.find(filename.getHash()) == m_shaderProgramMap.end()) {
			m_used_resource_filenames.insert(filename.getText());

			Json::Reader reader;
			Json::Value root;

			std::ifstream program_file;
			program_file.open((SHADER_PROGRAM_FOLDER_PATH + filename).c_str());

			if (!program_file.is_open()) {
				std::cout << "ShaderLoader: Could not locate shader program file: " << filename << std::endl;
				getchar();
				exit(-1);
			}

			bool success = reader.parse(program_file, root);
			if (!success) {
				std::cout << "ShaderLoader: Could not parse shader program file: " << reader.getFormattedErrorMessages() << std::endl;
				getchar();
				exit(-1);
			}

			program_file.close();

			auto vertex_shader = root["vertex"];
			if (vertex_shader.isNull()) {
				std::cout << "ShaderLoader: Could not find vertex shader" << std::endl;
				getchar();
				exit(-1);
			}
			load(vertex_shader.asString());

			auto fragment_shader = root["fragment"];
			if (fragment_shader.isNull()) {
				std::cout << "ShaderLoader: Could not find fragment shader" << std::endl;
				getchar();
				exit(-1);
			}
			load(fragment_shader.asString());

			auto material_id = root["material_id"];
			if (material_id.isNull()) {
				std::cout << "ShaderLoader: Material does not have ID" << std::endl;
				getchar();
				exit(-1);
			}
			int id_number = material_id.asInt();

			// get shader gl objects to link into program
			GLuint vertex_shader_object = getVertexShader(vertex_shader.asString());
			GLuint fragment_shader_object = getFragmentShader(fragment_shader.asString());

			// create gl program object
			GLuint program = glCreateProgram();
			glAttachShader(program, vertex_shader_object);
			glAttachShader(program, fragment_shader_object);
			glLinkProgram(program);

			GLint result = GL_FALSE;
			int logLength;

			glGetProgramiv(program, GL_LINK_STATUS, &result);
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
			bool isError = logLength > 1;
			if (isError) {
				std::vector<char> programError(logLength);
				glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
				std::cout << &programError[0] << std::endl;
			}

			// create game shader program object
			pn::ShaderProgram shader_program(program, filename, vertex_shader.asString(), fragment_shader.asString(), id_number);

			GLint num_active_uniforms;
			glGetProgramInterfaceiv(program, GL_UNIFORM, GL_ACTIVE_RESOURCES, &num_active_uniforms);

			std::vector<GLchar> nameData(256);
			std::vector<GLenum> properties = { GL_NAME_LENGTH };
			std::vector<GLint> values(properties.size());
			for (int uniform = 0; uniform < num_active_uniforms; uniform++) {
				glGetProgramResourceiv(program, GL_UNIFORM, uniform, properties.size(), &properties[0], values.size(), NULL, &values[0]);

				nameData.resize(values[0]);

				glGetProgramResourceName(program, GL_UNIFORM, uniform, nameData.size(), NULL, &nameData[0]);

				std::string name((char*)&nameData[0], nameData.size() - 1);
				shader_program.addUniform(name);
			}
			std::cout << "inserting shaderprogram " << filename << " with gluint " << shader_program.getGLProgramObject() << std::endl;
			m_shaderProgramMap.insert({ filename.getHash(), shader_program });
		}
	}
		break;
	default:
		std::cout << "ResourceManager: Couldn't load " << filename.getText() << " -- No identifiable file type" << std::endl;
		break;
	}

}

const pn::Image& pn::ResourceManager::getImage(const PString& filename) {
	return getImage(filename.getHash());
}

const pn::Image& pn::ResourceManager::getImage(const HashValue& key) {
	auto image_itr = m_imageMap.find(key);
	assert(image_itr != m_imageMap.end());
	return image_itr->second;
}

const pn::Mesh& pn::ResourceManager::getMesh(const PString& filename) {
	return getMesh(filename.getHash());
}

const pn::Mesh& pn::ResourceManager::getMesh(const HashValue& key) {
	auto mesh_itr = m_meshMap.find(key);
	assert(mesh_itr != m_meshMap.end());
	return mesh_itr->second;
}

GLuint pn::ResourceManager::getVertexShader(const PString& filename) {
	return getVertexShader(filename.getHash());
}

GLuint pn::ResourceManager::getVertexShader(const HashValue& key) {
	auto vshader_itr = m_vshaderMap.find(key);
	assert(vshader_itr != m_vshaderMap.end());
	return vshader_itr->second;
}

GLuint pn::ResourceManager::getFragmentShader(const PString& filename) {
	return getFragmentShader(filename.getHash());
}

GLuint pn::ResourceManager::getFragmentShader(const HashValue& key) {
	auto fshader_itr = m_fshaderMap.find(key);
	assert(fshader_itr != m_fshaderMap.end());
	return fshader_itr->second;
}

const pn::ShaderProgram& pn::ResourceManager::getShaderProgram(const PString& filename) {
	return getShaderProgram(filename.getHash());
}

const pn::ShaderProgram& pn::ResourceManager::getShaderProgram(const HashValue& key) {
	auto shader_program_itr = m_shaderProgramMap.find(key);
	assert(shader_program_itr != m_shaderProgramMap.end());
	return shader_program_itr->second;
}

void pn::ResourceManager::remove(const PString& filename) {
	auto filetype = pn::FileType::toFileTypeEnum(filename);

	switch (filetype) {
	case pn::FileTypeEnum::PNG:
		m_imageMap.erase(filename.getHash());
		break;
	case pn::FileTypeEnum::OBJ:
		m_meshMap.erase(filename.getHash());
		break;
	case pn::FileTypeEnum::VERTEX_SHADER: {
		GLuint vshader = getVertexShader(filename);
		glDeleteShader(vshader);
		m_vshaderMap.erase(filename.getHash());
	}
		break;
	case pn::FileTypeEnum::FRAGMENT_SHADER: {
		GLuint fshader = getFragmentShader(filename);
		glDeleteShader(fshader);
		m_fshaderMap.erase(filename.getHash());
	}
		break;
	case pn::FileTypeEnum::SHADER_PROGRAM: {
		pn::ShaderProgram shaderProgram = getShaderProgram(filename);
		remove(shaderProgram.getVertexShaderFilename());
		remove(shaderProgram.getFragmentShaderFilename());
		glDeleteProgram(shaderProgram.getGLProgramObject());
		m_shaderProgramMap.erase(filename.getHash());
	}
		break;
	default:
		break;
	}
}

void pn::ResourceManager::removeAll() {
	for (auto& file : m_used_resource_filenames) {
		remove(file);
	}
}

std::string pn::ResourceManager::readShaderFile(const char* filename) {
	/*
	From Nex at:
	http://www.nexcius.net/2012/11/20/how-to-load-a-glsl-shader-in-opengl-using-c/
	*/

	std::string content;
	std::ifstream fileStream(filename, std::ios::in);

	if (!fileStream.is_open()) {
		std::cerr << "ShaderLoader: Could not read file " << filename << ". File does not exist." << std::endl;
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}
