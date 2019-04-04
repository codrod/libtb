/*! \file textbox.h
    \brief Header file for libtb
*/

/*! \mainpage Overview
	TextBox Library or 'libtb' is a C++ API used to interface with TextBoxes. Visit
	the [gtb](https://codrod.github.io/gtb/index.html) repository to learn more
	about TextBoxes.
*/

/*
	finalize error handling in Box::get and Box::put

	clean up error messages and double check error handling
*/

#if !TEXTBOX_VERSION

	//C
	#include <stdlib.h>
	#include <errno.h>
	#include <string.h>

	//C++
	#include <iostream>
	#include <string>
	#include <vector>

	#include "textbox_posix.h"

	#define TEXTBOX_VERSION 1000000L

	namespace TextBox
	{
		const char DC1 = '\021';
		const char DC2 = '\022';
		const char DC3 = '\023';

		class coord
		{
			public:
				long long int x;
				long long int y;

				coord(long long int x, long long int y);
		};

		class Sequence : public std::vector<std::string>
		{
			public:
				Sequence(std::string seq);
				Sequence();

				Sequence& parse(std::string seq);
		};

		//Forward reference to Box
		class Box;

		class ostream : public std::ostream
		{
			public:
				ostream(Box *box);

				ostream& move(coord pos);
				coord find();
				ostream& scroll(coord pos);

				ostream& clear(coord start, coord finish);
				ostream& clear(); //clear whole screen (including scrolled output)

				ostream& write(const char *str, std::streamsize siz);
				ostream& write(const char *str);
				ostream& put(char c);

				ostream& insert(const char *str, std::streamsize siz);
				ostream& insert(const char *str);
				ostream& insert(char c);

			private:
				Box *box;

				ostream& lock();
				ostream& unlock();
				Sequence sendDCS(std::string seqstr, std::string err, int retsiz);

		};

		class istream : public std::istream
		{
			public:
				istream(Box *box);
				istream& move(coord pos);
				coord find();

				istream& editline(char *str, std::streamsize siz);

			private:
				Box *box;

				istream& lock();
				istream& unlock();
				Sequence sendDCS(std::string seqstr, std::string err, int retsiz);
		};

		class Box
		{
			public:
				Mutex *mutex;
				OutputPipe *output;
				InputPipe *input;

				istream cin;
				ostream cout;
				ostream cerr;

				Box& lock();
				Box& unlock();

				std::string getDCS();
				Box& putDCS(std::string seq);
				Sequence sendDCS(std::string seqstr, std::string err, int retsiz);

				Box();
				~Box();

			private:
				static int counter;
		};

		void move_cursor(coord pos); //moves outpos and inpos at once
	}

	namespace tb
	{
		extern TextBox::Box box;

		extern TextBox::istream cin;
		extern TextBox::ostream cout;
		extern TextBox::ostream cerr;
		//extern TextBox::ostream clog;
	}

#endif
