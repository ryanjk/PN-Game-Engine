#ifndef PSTRING_H
#define PSTRING_H

#include <iostream>
#include <string>

using HashValue = unsigned int;

namespace pn {
	class PString {
	public:
		PString(const std::string& string);
		PString(const PString& pstring);
		PString(const char* string);

		const std::string& getText() const;
		const char* c_str() const;

		HashValue getHash() const;

		bool equals(const std::string& string) const;
		bool equals(const PString& pstring) const;
		bool equals(const char* string) const;

		PString operator+(const PString& pstring) const;

	private:
		std::string m_string;
		HashValue m_hash;

		HashValue hash(const char* string, unsigned int seed = 0) const;
	};
}

std::ostream& operator<<(std::ostream& stream, const pn::PString& pstring);

#endif