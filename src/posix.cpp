#include "textbox.h"

using namespace TextBox;

char* GetEnv(const char *name)
{
	return getenv(name);
}

Mutex::Mutex()
{
	std::string path("/tw_lock_");
	char *pid = NULL;

	if(!(pid = GetEnv("TW_PID")))
		throw std::runtime_error("cant get TW_PID");

	path += pid;

	if(!(sem = sem_open(path.data(), 0)))
		throw std::runtime_error("cant open named semaphore");

	return;
}

Mutex::~Mutex()
{
	sem_close(sem);

	return;
}

Mutex& Mutex::lock()
{
	if(sem_wait(sem) == -1)
		throw std::runtime_error(strerror(errno));

	return *this;
}

Mutex& Mutex::unlock()
{
	while(sem_post(sem) == -1)
		if(errno != EINTR)
			throw std::runtime_error(strerror(errno));

	return *this;
}

OutputPipe::OutputPipe()
{
	std::string path("/tmp/tw_out_");
	char *pid = NULL;

	if(!(pid = GetEnv("TW_PID")))
		throw std::runtime_error("TW: cant get TW_PID");

	path += pid;

	if((fd = open(path.data(), O_WRONLY)) == -1)
		throw std::runtime_error("TW: failed to open output FIFO pipe");

	return;
}

OutputPipe::~OutputPipe()
{
	return;
}

OutputPipe& OutputPipe::write(const char *str, std::streamsize size)
{
	ssize_t ret = 0;
	size_t nbytes = 0;

	for(; nbytes != size; nbytes += ret)
		if((ret = ::write(fd, &str[nbytes], size - nbytes)) == -1)
			throw std::runtime_error(std::string("TW: failed to write DCS : ").append(strerror(errno)));

	return *this;
}

InputPipe::InputPipe()
{
	std::string path("/tmp/tw_in_");
	char *pid = NULL;

	if(!(pid = GetEnv("TW_PID")))
		throw std::runtime_error("TW: cant get TW_PID");

	path += pid;

	if((fd = open(path.data(), O_RDONLY)) == -1)
		throw std::runtime_error("TW: failed to open input FIFO pipe");

	return;
}

InputPipe::~InputPipe()
{
	return;
}

InputPipe& InputPipe::read(char *str, std::streamsize siz)
{
	::read(fd, str, siz);
	//while((ret = read(in, &c, 1)) == -1 && (errno == EAGAIN || errno == EWOULDBLOCK));

	//if(!ret || errno) //premature EOF
		//throw std::runtime_error("TW: failed to read DCS");

	return *this;
}
