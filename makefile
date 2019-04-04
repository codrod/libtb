CXX = g++
CXXFLAGS = --std=c++11 -Iinclude `pkg-config --cflags gtkmm-3.0`
LDFLAGS = -Llib `pkg-config --libs gtkmm-3.0`
LDLIBS = -lpthread

AR = ar
ARFLAGS = crv

INCLUDES = include/textbox.h include/textbox_posix.h
OBJS = src/box.o src/coord.o src/istream.o src/ostream.o src/posix.o src/sequence.o

#TESTS =
#EXAMPLES =

%.o: %.c $(INCLUDES)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS) $(LDLIBS)

#%.ct: %.c lib/libtb.a
#	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LDLIBS)

lib/libtb.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $?

#test: $(TESTS)
#	rm -f test.sh
#	echo "#!/bin/sh" >> test.sh
#	ls test/*.ct -1 >> test.sh
#	chmod u+x test.sh

.PHONY: docs
docs:
	doxygen

#examples: $(EXAMPLES);

clean:
	rm -fr src/*.o lib/* docs/* #test/*.ct examples/*.ct test.sh
