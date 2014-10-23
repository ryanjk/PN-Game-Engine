#include "PN/Util/ResourceManager.h"
#include "PN/Util/FileType.h"
#include "PN/Util/ShaderLoader.h"

#include "PN/Render/RenderFactory.h"

#include <iostream>
#include <mutex>

std::mutex imageMutex;
std::mutex meshMutex;

static const pn::PString IMAGE_FOLDER_PATH = "resource/image/";
static const pn::PString MESH_FOLDER_PATH = "resource/mesh/";
static const pn::PString SHADER_FOLDER_PATH = "resource/shader/";

pn::ResourceManager pn::ResourceManager::g_resourceManager;

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
			std::lock_guard<std::mutex> imageLock(imageMutex);
			m_imageMap.insert({ filename.getHash(), std::move(image) });
		}
	}
		break;
	case pn::FileTypeEnum::OBJ: {
		if (m_meshMap.find(filename.getHash()) == m_meshMap.end()) {
			Mesh mesh = pn::RenderFactory::loadMeshFromObj((MESH_FOLDER_PATH + filename).c_str());
			std::lock_guard<std::mutex> meshLock(meshMutex);
			m_meshMap.insert({ filename.getHash(), std::move(mesh) });
		}
	}
		break;
	case pn::FileTypeEnum::VERTEX_SHADER: {
		if (m_vshaderMap.find(filename.getHash()) == m_vshaderMap.end()) {
			GLuint shader = pn::ShaderLoader::loadShader((SHADER_FOLDER_PATH + filename).c_str(), GL_VERTEX_SHADER);
			m_vshaderMap.insert({ filename.getHash(), shader });
		}
	}
		break;
	case pn::FileTypeEnum::FRAGMENT_SHADER: {
		if (m_fshaderMap.find(filename.getHash()) == m_fshaderMap.end()) {
			GLuint shader = pn::ShaderLoader::loadShader((SHADER_FOLDER_PATH + filename).c_str(), GL_FRAGMENT_SHADER);
			m_fshaderMap.insert({ filename.getHash(), shader });
		}
	}
		break;
	default:
		std::cout << "ResourceManager: Couldn't load " << filename.getText() << " -- No identifiable file type" << std::endl;
		break;
	}

}

template<class T>
T& pn::ResourceManager::get(const PString& filename) {
	auto filetype = pn::FileType::toFileTypeEnum(filename);
	auto key = filename.getHash();

	switch (filetype) {
	case pn::FileTypeEnum::PNG: {
		auto image_itr = m_imageMap.find(key);
		if (image_itr != m_imageMap.end()) {
			return image_itr->second;
		}
	}
		break;
	case pn::FileTypeEnum::OBJ: {
		auto mesh_itr = m_meshMap.find(key);
		if (mesh_itr != m_meshMap.end()) {
			return mesh_itr->second;
		}
	}
		break;
	case pn::FileTypeEnum::VERTEX_SHADER: {
		auto shader_itr = m_vshaderMap.find(key);
		if (shader_itr != m_vshaderMap.end()) {
			return shader_itr->second;
		}
	}
		break;
	case pn::FileTypeEnum::FRAGMENT_SHADER: {
		auto shader_itr = m_fshaderMap.find(key);
		if (shader_itr != m_fshaderMap.end()) {
			return shader_itr->second;
		}
	}
		break;
	default:
		return;
		break;
	}
}

pn::Image& pn::ResourceManager::getImage(const PString& filename) {
	return getImage(filename.getHash());
}

pn::Image& pn::ResourceManager::getImage(const HashValue& key) {
	auto image_itr = m_imageMap.find(key);
	if (image_itr != m_imageMap.end()) {
		return image_itr->second;
	}
}

pn::Mesh& pn::ResourceManager::getMesh(const PString& filename) {
	return getMesh(filename.getHash());
}

pn::Mesh& pn::ResourceManager::getMesh(const HashValue& key) {
	auto mesh_itr = m_meshMap.find(key);
	if (mesh_itr != m_meshMap.end()) {
		return mesh_itr->second;
	}
}

GLuint& pn::ResourceManager::getVertexShader(const PString& filename) {
	return getVertexShader(filename.getHash());
}

GLuint& pn::ResourceManager::getVertexShader(const HashValue& key) {
	auto shader_itr = m_vshaderMap.find(key);
	if (shader_itr != m_vshaderMap.end()) {
		return shader_itr->second;
	}
}

GLuint& pn::ResourceManager::getFragmentShader(const PString& filename) {
	return getFragmentShader(filename.getHash());
}

GLuint& pn::ResourceManager::getFragmentShader(const HashValue& key) {
	auto shader_itr = m_fshaderMap.find(key);
	if (shader_itr != m_fshaderMap.end()) {
		return shader_itr->second;
	}
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
	default:
		break;
	}

}
