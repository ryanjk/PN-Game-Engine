#ifndef FILE_TYPE_H
#define FILE_TYPE_H

#include "PN/Util/PString.h"

namespace pn {
	enum class FileTypeEnum {
		PNG, 
		OBJ,
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		MATERIAL,
		NONE
	};
	class FileType {
	public:
		static FileTypeEnum toFileTypeEnum(const PString& filepath);
	};
}

std::ostream& operator<<(std::ostream& ostream, pn::FileTypeEnum filetype);

#endif