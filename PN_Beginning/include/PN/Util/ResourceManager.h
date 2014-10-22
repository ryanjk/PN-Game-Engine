#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER

#include "PN/Window/WindowManager.h"

#include "PN/Util/PString.h"

#include "PN/Render/Image.h"
#include "PN/Render/Mesh.h"

#include <map>

using ImageMap = std::map < HashValue, pn::Image > ;
using MeshMap = std::map < HashValue, pn::Mesh > ;
using ShaderMap = std::map < HashValue, GLuint > ;

namespace pn {
	class ResourceManager {
	public:
		static ResourceManager g_resourceManager;

		void startUp();
		void shutdown();

		void load(const PString& filename);

		template<class T>
		T& get(const PString& filename);

		Image& getImage(const PString& filename);
		Image& getImage(const HashValue& key);

		Mesh& getMesh(const PString& filename);
		Mesh& getMesh(const HashValue& key);

		GLuint& getVertexShader(const PString& filename);
		GLuint& getVertexShader(const HashValue& key);

		GLuint& getFragmentShader(const PString& filename);
		GLuint& getFragmentShader(const HashValue& key);

		void remove(const PString& filename);

	private:
		ResourceManager();

		ImageMap m_imageMap;
		MeshMap m_meshMap;
		ShaderMap m_vshaderMap;
		ShaderMap m_fshaderMap;
	};
}

#endif