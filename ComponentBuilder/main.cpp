#include <iostream>
#include <fstream>
#include <string>

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "json/json.h"

const std::string component_include_dir = "../PN_Beginning/include/PN/ECS/Component/";
const std::string component_source_dir = "../PN_Beginning/src/PN/ECS/Component/";

char* toupper(const char* string) {
	const int size = strlen(string) + 1;
	char* return_string = (char*) malloc(size);
	
	int i;
	for (i = 0; i < size; i++) {
		return_string[i] = toupper(string[i]);
	}

	return return_string;
}
std::string lower_first(std::string string) {
	string[0] = tolower(string[0]);
	return string;
}

int buildComponentHeaderAndSource(std::ifstream& component_file, Json::Reader& reader) {
	Json::Value root;
	bool success = reader.parse(component_file, root);
	if (!success) {
		std::cout << reader.getFormattedErrorMessages() << std::endl;
		component_file.close();
		return -1;
	}

	component_file.close();

	auto component_name = root["name"].asString();
	auto class_name = component_name + "Component";

	std::cout << "Building header file for " + class_name + " . . ." << std::endl;

	// Begin header file
	FILE* header = fopen((component_include_dir + component_name + "Component.h").c_str(), "w");

	char* header_g = toupper(component_name.c_str());
	std::string header_guard = std::string(header_g) + "_COMPONENT_H";

	// Header guard
	fputs("#ifndef ", header);
	fputs(header_guard.c_str(), header);
	fputs("\n#define ", header);
	fputs(header_guard.c_str(), header);

	// Includes
	fputs("\n\n#include \"PN/ECS/Component/IComponent.h\"\n\n", header);
	fputs("#include \"PN/Util/Math.h\"\n", header);
	fputs("#include \"PN/Util/PString.h\"", header);

	// Namespace and class declaration
	fputs("\n\nnamespace pn {\n", header);
	fputs("\tclass ", header);
	fputs(class_name.c_str(), header);
	fputs(" : public pn::IComponent {\n\t", header);

	// Begin public methods
	fputs("public:\n\t\t", header);

	// Simple factory
	fputs(("static std::shared_ptr<" + class_name + "> make(const ComponentData& data, pn::ResourceManager& resources);\n\t\t").c_str(), header);

	// Get component's type enum
	fputs("ComponentType getType() const override;\n\n\t\t", header);

	// Constructor
	fputs((class_name + "();\n\n\t").c_str(), header);

	// Getters and setters
	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();
		fputs(("\t" + dataType + " get" + dataName + "() const;\n\t\t").c_str(), header);
		fputs(("void set" + dataName + "(" + dataType + " " + lower_first(dataName) + ");\n\n\t").c_str(), header);
	}

	// Begin private methods
	fputs("private:\n\t", header);

	// Private data members and their default values
	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();
		fputs(("\t" + dataType + " m_" + lower_first(dataName) + " = ").c_str(), header);

		auto dataDefault = root["data"][dataName]["default"];
		if (dataDefault.isArray()) {
			if (dataDefault.size() == 3) {
				fputs(("{ " +
					dataDefault[0].asString() + ", " +
					dataDefault[1].asString() + ", " +
					dataDefault[2].asString() +
					" };\n\t").c_str(), header);
			}
			else if (dataDefault.size() == 4) {
				fputs(("{ " +
					dataDefault[0].asString() + ", " +
					dataDefault[1].asString() + ", " +
					dataDefault[2].asString() + ", " +
					dataDefault[3].asString() +
					" };\n\t").c_str(), header);
			}
			else if (dataDefault.size() == 16) {
				fputs(("{\n\t\t\t" +
					dataDefault[0].asString() + ", " +
					dataDefault[1].asString() + ", " +
					dataDefault[2].asString() + ", " +
					dataDefault[3].asString() + ", \n\t\t\t" +
					dataDefault[4].asString() + ", " +
					dataDefault[5].asString() + ", " +
					dataDefault[6].asString() + ", " +
					dataDefault[7].asString() + ", \n\t\t\t" +
					dataDefault[8].asString() + ", " +
					dataDefault[9].asString() + ", " +
					dataDefault[10].asString() + ", " +
					dataDefault[11].asString() + ", \n\t\t\t" +
					dataDefault[12].asString() + ", " +
					dataDefault[13].asString() + ", " +
					dataDefault[14].asString() + ", " +
					dataDefault[15].asString() +
					"\n\t\t\t};\n\t").c_str(), header);
			}
		}

		else if (dataDefault.isString()) {
			fputs(("\"" + dataDefault.asString() + "\";\n\t").c_str(), header);
		}

		else {
			fputs((dataDefault.asString() + ";\n\t").c_str(), header);
		}
	}

	// End header guard
	fputs("};\n}\n\n#endif", header);

	// End header file
	fclose(header);

	std::cout << "Building source file for " + class_name + " . . . " << std::endl;

	// Begin source file
	FILE* source = fopen((component_source_dir + component_name + "Component.cpp").c_str(), "w");

	// Includes
	fputs(("#include \"PN/ECS/Component/" + class_name + ".h\"\n\n").c_str(), source);

	// Define simple factory
	fputs(("std::shared_ptr<pn::" + class_name + "> " + "pn::" + class_name + "::make(const ComponentData& data, pn::ResourceManager& resources) {\n\t").c_str(), source);
	fputs(("std::shared_ptr<" + class_name + "> component = std::make_shared<" + class_name + ">();\n").c_str(), source);

	// For each data member, if defined by component implementer, replace the default value
	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();
		auto var_name = lower_first(dataName) + "Data";

		fputs(("\n\tauto " + lower_first(dataName) + "Data = data[\"" + lower_first(dataName) + "\"];\n").c_str(), source);
		fputs(("\tif (!" + var_name + ".isNull()) {\n").c_str(), source);
		fputs(("\t\tcomponent->m_" + lower_first(dataName) + " = ").c_str(), source);

		if (dataType == "vec3") {
			fputs(("vec3(" +
				var_name + "[0].asFloat(), " +
				var_name + "[1].asFloat(), " +
				var_name + "[2].asFloat()" +
				");\n\t}\n").c_str(), source);
		}

		else if (dataType == "vec4") {
			fputs(("vec4(" +
				var_name + "[0].asFloat(), " +
				var_name + "[1].asFloat(), " +
				var_name + "[2].asFloat(), " +
				var_name + "[3].asFloat()" +
				");\n\t}\n").c_str(), source);
		}

		else if (dataType == "mat4") {
			std::cout << "mat4 not ready" << std::endl;
			return -1;
			fputs(("mat4(" +
				var_name + "[0].asFloat(), " +
				var_name + "[1].asFloat(), " +
				var_name + "[2].asFloat(), " +
				var_name + "[3].asFloat()" +
				");\n\t}\n").c_str(), source);
		}

		else if (dataType == "int") {
			fputs((
				var_name + ".asInt()" +
				";\n\t}\n").c_str(), source);
		}

		else if (dataType == "bool") {
			fputs((
				var_name + ".asBool()" +
				";\n\t}\n").c_str(), source);
		}

		else if (dataType == "float") {
			fputs((
				var_name + ".asFloat()" +
				";\n\t}\n").c_str(), source);
		}

		else if (dataType == "double") {
			fputs((
				var_name + ".asDouble()" +
				";\n\t}\n").c_str(), source);
		}

		else if (dataType == "std::string" || dataType == "pn::PString") {
			fputs((
				var_name + ".asString()" +
				";\n\t}\n").c_str(), source);
		}
	}

	fputs(std::string("\n\treturn component;\n}\n\n").c_str(), source);

	fputs(("pn::" + class_name + "::" + class_name + "() {};\n\n").c_str(), source);

	// Define getType() method, which return type enum
	fputs(("pn::ComponentType pn::" + class_name + "::getType() const {\n\t").c_str(), source);
	fputs(("return pn::ComponentType::" + std::string(toupper(component_name.c_str())) + ";\n}\n\n").c_str(), source);

	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();

		fputs((dataType + " pn::" + class_name + "::get" + dataName + "() const {\n\t").c_str(), source);
		fputs(("return m_" + lower_first(dataName) + ";\n}\n\n").c_str(), source);

		fputs(("void pn::" + class_name + "::set" + dataName + "(" + dataType + " " + lower_first(dataName) + ") {\n\t").c_str(), source);
		fputs(("m_" + lower_first(dataName) + " = " + lower_first(dataName) + ";\n}\n\n").c_str(), source);
	}

	// End source file
	fclose(source);
}

int main(int argc, const char* argv[]) {
	if (argc < 2) {
		std::cerr << "Must input at least one file to parse" << std::endl;
		return -1;
	}

	Json::Reader reader;
	std::ifstream component_file;

	for (int i = 1; i < argc; i++) {
		component_file.open(std::string(".\\CDF\\") + std::string(argv[i]));
		if (!component_file.is_open()) {
			std::cout << "Could not locate " + std::string(argv[i]) << std::endl;
			return -1;
		}

		int success = buildComponentHeaderAndSource(component_file, reader);

		if (success == -1) {
			return -1;
		}

	}

	return 0;
}