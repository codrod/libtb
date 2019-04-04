#include "textbox.h"

using namespace TextBox;

int Box::counter = 0;
Box tb::box;

Box::Box() : cin(this), cout(this), cerr(this)
{
	std::string ret;

	mutex = new Mutex();
	output = new OutputPipe();

	lock();

		try
		{
			putDCS("\021SETUP\023");

				input = new InputPipe();

			ret = getDCS();
		}
		catch(std::runtime_error ex)
		{
			unlock();
			throw ex;
		}

	unlock();

	if(ret != "\021RET\022SETUP\0221\023")
		throw std::runtime_error("TextBox:ERROR: failed to setup box");

	if(!counter)
	{
		tb::cin.rdbuf(std::cin.rdbuf());
		tb::cout.rdbuf(std::cout.rdbuf());

		tb::cin.tie(&tb::cout);
	}

	counter++;

	return;
}

Box::~Box()
{
	delete output;
	delete input;
	delete mutex;

	return;
}

std::string Box::getDCS()
{
	char c = 0;
	bool started = false;
	std::string str;
	size_t ret = 0;

	while(!started || c != DC3)
	{
		//std::cerr<<"c = "<<c<<"\n";
		//read(in, &c, 1);
		input->read(&c, 1);
		//while((ret = read(in, &c, 1)) == -1 && (errno == EAGAIN || errno == EWOULDBLOCK));

		//if(!ret || errno) //premature EOF
			//throw std::runtime_error("TW: failed to read DCS");

		if(started)
		{
			str += c;
		}
		else if(c == DC1)
		{
			started = true;
			str += c;
		}
	}

	return str;
}

Box& Box::putDCS(std::string seq)
{
	ssize_t ret = 0;
	size_t nbytes = 0;

	//ensure pending output has been sent before sending DCS
	//This may cause a race condition
	try
	{
		tb::cout.flush();
	}
	catch(std::ios_base::failure ex)
	{
		throw std::runtime_error(ex.what());
	}

	output->write(seq.data(), seq.size());

	return *this;
}

Box& Box::lock()
{
	try
	{
		mutex->lock();
	}
	catch(std::runtime_error ex)
	{
		throw std::runtime_error(std::string("TextBox:ERROR: failed to lock the box : ") + ex.what());
	}

	return *this;
}

Box& Box::unlock()
{
	try
	{
		mutex->unlock();
	}
	catch(std::runtime_error ex)
	{
		throw std::runtime_error(std::string("TextBox:ERROR: failed to unlock the box : ") + ex.what());
	}

	return *this;
}

Sequence Box::sendDCS(std::string seqstr, std::string err, int retsiz)
{
	std::string retstr;
	Sequence seq(seqstr);

	lock();

		retstr = putDCS(seqstr).getDCS();

	unlock();

	Sequence retseq(retstr);

	if(retseq.size() < retsiz || retseq[0] != "RET" || retseq[1] != seq[0] || retseq[2] != "1")
	{
		if(retseq.size() < 3 || retseq[0] != "RET" || retseq[1] != seq[0] || retseq[2] != "0")
			throw std::runtime_error(err.append("irrelevant DCS response "));
		else if(retseq.size() >= 4)
			throw std::runtime_error(err.append(retseq[3]));
		else
			throw std::runtime_error(err);
	}

	return retseq;
}
