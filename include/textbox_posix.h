/*! \file textbox_posix.h
    \brief POSIX middleware for libtb
*/

#if !TEXTBOX_POSIX_VER

	#define TEXTBOX_POSIX_VER 1000000L

	//POSIX C
	#include <unistd.h>
	#include <semaphore.h>
	#include <sys/stat.h>
	#include <fcntl.h>

	namespace TextBox
	{
		char* GetEnv(const char *name);

		class Mutex
		{
			public:
				Mutex& lock();
				Mutex& unlock();

				Mutex();
				~Mutex();

			private:
				sem_t *sem;
		};

		class OutputPipe
		{
			public:
				OutputPipe();
				~OutputPipe();

				OutputPipe& write(const char *str, std::streamsize);

			private:
				int fd;
		};

		class InputPipe
		{
			public:
				InputPipe();
				~InputPipe();

				InputPipe& read(char *str, std::streamsize);

			private:
				int fd;
		};
	}

#endif
