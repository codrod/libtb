#include "textbox.h"

using namespace TextBox;

Sequence::Sequence(std::string str)
{
	parse(str);

	return;
}

Sequence::Sequence()
{
	return;
}

Sequence& Sequence::parse(std::string str)
{
	std::string err("TW: failed to parse string into Sequence : ");
	std::string element;
	size_t i = 1; //skip DC1

	if(*str.begin() != DC1)
		throw std::runtime_error(err.append("string does not start with DC1"));

	if(*str.rbegin() != DC3)
		throw std::runtime_error(err.append("string does not end with DC3"));

	for(; i + 1 < str.size(); i++)
	{
		if(str[i] == DC1 || str[i] == DC3)
			throw std::runtime_error(err.append("found DC1 or DC3 in middle of string"));

		if(str[i] == DC2)
		{
			if(!element.size())
				throw std::runtime_error(err.append("found empty argument in string"));

			push_back(element);
			element.clear();
		}
		else element.push_back(str[i]);
	}

	if(!element.size())
		throw std::runtime_error(err.append("found empty argument in string"));

	push_back(element);

	return *this;
}
