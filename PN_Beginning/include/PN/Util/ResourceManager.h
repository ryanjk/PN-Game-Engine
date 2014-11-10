#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "PN/Window/WindowManager.h"

#include "PN/Util/PString.h"

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"
#include "PN/Render/Material.h"

#include <map>
#include <set>

using ImageMap = std::map < HashValue, pn::Image > ;
using MeshMap = std::map < HashValue, pn::Mesh > ;
using ShaderMap = std::map < HashValue, GLuint > ;
using MaterialMap = std::map < HashValue, pn::Material > ;

namespace pn {
	class ResourceManager {
	public:
		ResourceManager();

		void startUp();
		void shutdown();

		void load(const PString& filename);

		const Image& getImage(const PString& filename);
		const Image& getImage(const HashValue& key);

		const Mesh& getMesh(const PString& filename);
		const Mesh& getMesh(const HashValue& key);

		GLuint getVertexShader(const PString& filename);
		GLuint getVertexShader(const HashValue& key);

		GLuint getFragmentShader(const PString& filename);
		GLuint getFragmentShader(const HashValue& key);

		const pn::Material& getMaterial(const PString& filename);
		const pn::Material& getMaterial(const HashValue& key);

		void removeAll();
		void remove(const PString& filename);

	private:
		ImageMap m_imageMap;
		MeshMap m_meshMap;
		ShaderMap m_vshaderMap;
		ShaderMap m_fshaderMap;
		MaterialMap m_materialMap;

		std::set<std::string> m_used_resource_filenames;

		std::string readShaderFile(const char* filename);
	};
}

#endif