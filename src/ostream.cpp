#include "textbox.h"

using namespace TextBox;

ostream tb::cout(&tb::box);
ostream tb::cerr(&tb::box);

ostream::ostream(Box *box)
{
	this->box = box;
	return;
}

ostream& ostream::lock()
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

ostream& ostream::unlock()
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

Sequence ostream::sendDCS(std::string seqstr, std::string err, int retsiz)
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

ostream& ostream::write(const char *str, std::streamsize siz)
{
	std::ostream *super_this = dynamic_cast<std::ostream*>(this);
	std::string ret;

	lock();

		try
		{
			super_this->write(str, siz);
		}
		catch(std::runtime_error ex)
		{
			unlock();
			throw std::ios_base::failure(ex.what());
		}

	unlock();

	return *this;
}

ostream& ostream::write(const char *str)
{
	return write(str, std::string(str).size());
}

ostream& ostream::put(char c)
{
	std::ostream *super_this = dynamic_cast<std::ostream*>(this);

	lock();

		try
		{
			super_this->put(c);
		}
		catch(std::runtime_error ex)
		{
			unlock();
			throw std::ios_base::failure(ex.what());
		}

	unlock();

	return *this;
}

//Think about if a sentry object would help in this situation
ostream& ostream::insert(const char *str, std::streamsize siz)
{
	std::string seq("\021INSERT\022");
	std::streamsize i = 0;

	if(!str || siz < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to insert : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	for(; i < siz; i++)
	{
		if(str[i] == DC1 || str[i] == DC2 || str[i] == DC3)
		{
			if(exceptions() & std::ios_base::failbit)
				throw std::ios_base::failure("TW: failed to insert : string contains device control characters");
			else
				setstate(std::ios_base::failbit);
		}
	}

	seq += std::string(str, siz) + "\023";

	sendDCS(seq, "TW: failed to insert : ", 3);

	return *this;
}

ostream& ostream::insert(const char *str)
{
	return insert(str, std::string(str).size());
}

ostream& ostream::insert(char c)
{
	return insert(&c, 1);
}

ostream& ostream::move(coord pos)
{
	std::string seq("\021MOVE\022OUT\022");

	if(pos.x < 0 || pos.y < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to move ostream : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	seq += std::to_string(pos.x) + "\022";
	seq += std::to_string(pos.y) + "\023";

	sendDCS(seq, "TW: failed to move ostream : ", 3);

	return *this;
}

coord ostream::find()
{
	std::string seq("\021FIND\022OUT\023");
	Sequence ret;
	coord pos(0, 0);
	std::size_t endx = 0, endy = 0;

	ret = sendDCS(seq, "TW: failed to find ostream : ", 5);

	pos.x = std::stoll(ret[3], &endx, 10);
	pos.y = std::stoll(ret[4], &endy, 10);

	if(endx != ret[3].size() || endy != ret[3].size() || pos.x < 0 || pos.y < 0)
	{
		if(exceptions() & std::ios_base::badbit)
			throw std::ios_base::failure("TW: failed to find ostream : invalid coordinate");
		else
			setstate(std::ios_base::badbit);
	}

	return pos;
}

ostream& ostream::scroll(coord pos)
{
	std::string seq("\021SCROLL\022");

	if(pos.x < 0 || pos.y < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to scroll : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	seq += std::to_string(pos.x) + "\022";
	seq += std::to_string(pos.y) + "\023";

	sendDCS(seq, "TW: failed to scroll : ", 3);

	return *this;
}

ostream& ostream::clear(coord start, coord end)
{
	std::string seq("\021CLEAR\022");

	if(start.x < 0 || start.y < 0 || end.x < 0 || end.y < 0)
	{
		if(exceptions() & std::ios_base::failbit)
			throw std::ios_base::failure("TW: failed to clear : invalid argument");
		else
			setstate(std::ios_base::failbit);
	}

	seq += std::to_string(start.x) + "\022";
	seq += std::to_string(start.y) + "\022";
	seq += std::to_string(end.x) + "\022";
	seq += std::to_string(end.y) + "\023";

	sendDCS(seq, "TW: failed to clear : ", 3);

	return *this;
}

ostream& ostream::clear()
{
	std::string seq("\021CLEAR\023");

	sendDCS(seq, "TW: failed to clear : ", 3);

	return *this;
}
