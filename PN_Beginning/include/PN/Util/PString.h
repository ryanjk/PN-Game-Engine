#ifndef PSTRING_H
#define PSTRING_H

#include <iostream>
#include <string>

using HashValue = unsigned int;

namespace pn {
	class PString {
	public:
		PString();
		PString(const std::string& string);
		PString(const PString& pstring);
		PString(const char* string);

		const std::string& getText() const;
		const char* c_str() const;

		HashValue getHash() const;

		PString operator+(const PString& pstring) const;

	private:
		std::string m_string;
		HashValue m_hash;

		HashValue hash(const char* string, unsigned int seed = 0) const;
	};
}

std::ostream& operator<<(std::ostream& stream, const pn::PString& pstring);

#endif