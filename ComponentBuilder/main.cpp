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

int main(int argc, const char* argv[]) {
	if (argc != 2) {
		std::cerr << "Must input single file to parse" << std::endl;
		return -1;
	}

	Json::Reader reader;
	std::ifstream component_file;

	component_file.open(argv[1]);
	if (!component_file.is_open()) {
		std::cout << "Could not locate file" << std::endl;
		return -1;
	}

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

	std::cout << "Building header file . . ." << std::endl;

	FILE* header = fopen((component_include_dir + component_name + "Component.h").c_str(), "w");
	
	char* header_g = toupper(component_name.c_str());
	std::string header_guard = std::string(header_g) + "_COMPONENT_H";

	fputs("#ifndef ", header);
	fputs(header_guard.c_str(), header);
	fputs("\n#define ", header);
	fputs(header_guard.c_str(), header);

	fputs("\n\n#include \"PN/ECS/Component/IComponent.h\"\n\n", header);
	fputs("#include \"PN/Util/Math.h\"\n", header);
	fputs("#include \"PN/Util/PString.h\"", header);

	fputs("\n\nnamespace pn {\n", header);
	fputs("\tclass ", header);
	fputs(class_name.c_str(), header);
	fputs(" : public pn::IComponent {\n\t", header);

	fputs("public:\n\t\t", header);
	fputs(("static " + class_name + " make(const ComponentData& data);\n\t\t").c_str(), header);
	fputs("ComponentType getType() const override;\n\n\t", header);

	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();
		fputs(("\t" + dataType + " get" + dataName + "() const;\n\t\t").c_str(), header);
		fputs(("void set" + dataName + "(" + dataType + " " + lower_first(dataName) + ");\n\n\t").c_str(), header);
	}

	fputs("private:\n\t", header);
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

	fputs("};\n}\n\n#endif", header);

	fclose(header);

	std::cout << "Building source file . . . " << std::endl;

	FILE* source = fopen((component_source_dir + component_name + "Component.cpp").c_str(), "w");

	fputs(("#include \"PN/ECS/Component/" + class_name + ".h\"\n\n").c_str(), source);

	fputs(("pn::" + class_name + " " + "pn::" + class_name + "::make(const ComponentData& data) {\n\t").c_str(), source);
	fputs((class_name + " component;\n").c_str(), source);

	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();
		auto var_name = lower_first(dataName) + "Data";
		
		fputs(("\n\tauto " + lower_first(dataName) + "Data = data[\"" + lower_first(dataName) + "\"];\n").c_str(), source);
		fputs(("\tif (!" + var_name + ".isNull()) {\n").c_str(), source);
		fputs(("\t\tcomponent.m_" + lower_first(dataName) + " = ").c_str(), source);

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

	fputs(("pn::ComponentType pn::" + class_name + "::getType() const {\n\t").c_str(), source);
	fputs(("return pn::ComponentType::" + component_name + ";\n}\n\n").c_str(), source);

	for (auto dataName : root["data"].getMemberNames()) {
		auto dataType = root["data"][dataName]["type"].asString();

		fputs((dataType + " pn::" + class_name + "::get" + dataName + "() const {\n\t").c_str(), source);
		fputs(("return m_" + lower_first(dataName) + ";\n}\n\n").c_str(), source);

		fputs(("void pn::" + class_name + "::set" + dataName + "(" + dataType + " " + lower_first(dataName) + ") {\n\t").c_str(), source);
		fputs(("m_" + lower_first(dataName) + " = " + lower_first(dataName) + ";\n}\n\n").c_str(), source);
	}

	fclose(source);

	return 0;
}