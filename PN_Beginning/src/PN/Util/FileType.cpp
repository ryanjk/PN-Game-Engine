#include "PN/Util/FileType.h"

#include <string>
#include <iostream>

pn::FileTypeEnum pn::FileType::toFileTypeEnum(const PString& filepath) {
	auto s = filepath.getText();
	auto size = s.size();

	auto ext_pos = s.find_last_of(".");
	auto extension = s.substr(ext_pos + 1, size - ext_pos);
	
	if (extension == "png") {
		return pn::FileTypeEnum::PNG;
	}
	else if (extension == "obj") {
		return pn::FileTypeEnum::OBJ;
	}
	else if (extension == "vglsl") {
		return pn::FileTypeEnum::VERTEX_SHADER;
	}
	else if (extension == "fglsl") {
		return pn::FileTypeEnum::FRAGMENT_SHADER;
	}
	else if (extension == "sp") {
		return pn::FileTypeEnum::MATERIAL;
	}
	else {
		return pn::FileTypeEnum::NONE;
	}

}

std::ostream& operator<<(std::ostream& ostream, pn::FileTypeEnum filetype) {
	switch (filetype) {
	case pn::FileTypeEnum::PNG:
		return ostream << "PNG";
	case pn::FileTypeEnum::OBJ:
		return ostream << "OBJ";
	case pn::FileTypeEnum::VERTEX_SHADER:
		return ostream << "VERTEX_SHADER";
	case pn::FileTypeEnum::FRAGMENT_SHADER:
		return ostream << "FRAGMENT_SHADER";
	case pn::FileTypeEnum::MATERIAL:
		return ostream << "MATERIAL";
	case pn::FileTypeEnum::NONE:
		return ostream << "NONE";
	default:
		return ostream << "NO FILE TYPE";
	}
}