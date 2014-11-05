#include "PN/Util/PString.h"

pn::PString::PString() : m_string(), m_hash()
{}

pn::PString::PString(const std::string& string) : m_string(string), m_hash(hash(string.c_str())) 
{}

pn::PString::PString(const PString& pstring) : m_string(pstring.m_string), m_hash(pstring.m_hash)  
{}

pn::PString::PString(const char* string) : m_string(string), m_hash(hash(string))  
{}

const std::string& pn::PString::getText() const {
	return m_string;
}

const char* pn::PString::c_str() const {
	return m_string.c_str();
}

HashValue pn::PString::getHash() const {
	return m_hash;
}

pn::PString pn::PString::operator+(const pn::PString& pstring) const {
	pn::PString result(m_string + pstring.getText());
	return result;
}

std::ostream& operator<<(std::ostream& stream, const pn::PString& pstring) {
	return stream << pstring.getText();
}

HashValue pn::PString::hash(const char* string, unsigned int seed) const  {
	/*
	from George V. Reilly at:
	http://stackoverflow.com/questions/98153/whats-the-best-hashing-algorithm-to-use-on-a-stl-string-when-using-hash-map
	*/

	unsigned hash = seed;
	while (*string) {
		hash = hash * 101 + *string++;
	}
	return hash;
}