#include "textbox.h"

using namespace TextBox;

istream tb::cin(&tb::box);

istream::istream(Box *box)
{
	this->box = box;
	return;
}

istream& istream::lock()
{
	try
	{
		box->lock();
	}
	catch(std::runtime_error ex)
	{
		if(exceptions() & std::ios_base::badbit)
			throw std::ios_base::failure(ex.what());
		else
			setstate(std::ios_base::badbit);
	}

	return *this;
}

istream& istream::unlock()
{
	try
	{
		box->unlock();
	}
	catch(std::runtime_error ex)
	{
		if(exceptions() & std::ios_base::badbit)
			throw std::ios_base::failure(ex.what());
		else
			setstate(std::ios_base::badbit);
	}

	return *this;
}

Sequence istream::sendDCS(std::string seqstr, std::string err, int retsiz)
{
	Sequence ret;

	try
	{
		ret = box->sendDCS(seqstr, err, retsiz);
	}
	catch(std::runtime_error ex)
	{
		if(exceptions() & std::ios_base::badbit)
			throw std::ios_base::failure(ex.what());
		else
			setstate(std::ios_base::badbit);
	}

	return ret;
}

istream& istream::editline(char *str, std::streamsize siz)
{
	std::string seq("\021EDIT\022");
	std::streamsize i = 0;

	if(!str || siz < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to edit a line : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	for(; i < siz; i++)
	{
		if(str[i] == DC1 || str[i] == DC2 || str[i] == DC3)
		{
			if(exceptions() & std::ios_base::failbit)
				throw std::ios_base::failure("TW: failed to edit a line : string contains device control characters");
			else
				setstate(std::ios_base::failbit);
		}
	}

	seq += std::string(str) + "\023";

	sendDCS(seq, "TW: failed to edit line : ", 3);

	getline(str, siz);

	return *this;
}

istream& istream::move(coord pos)
{
	std::string seq("\021MOVE\022IN\022");

	if(pos.x < 0 || pos.y < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to move istream : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	seq += std::to_string(pos.x) + "\022";
	seq += std::to_string(pos.y) + "\023";

	sendDCS(seq, "TW: failed to move istream : ", 3);

	return *this;
}

coord istream::find()
{
	std::string seq = "\021FIND\022IN\023";
	coord pos(0, 0);
	Sequence ret;
	std::size_t endx = 0, endy = 0;

	ret = sendDCS(seq, "TW: failed to find istream : ", 5);

	pos.x = std::stoll(ret[3], &endx, 10);
	pos.y = std::stoll(ret[4], &endy, 10);

	if(endx != ret[3].size() || endy != ret[3].size() || pos.x < 0 || pos.y < 0)
	{
		if(exceptions() & std::ios_base::badbit)
			throw std::ios_base::failure("TW: failed to find TextBox:istream : invalid coordinate");
		else
			setstate(std::ios_base::badbit);
	}

	return pos;
}
