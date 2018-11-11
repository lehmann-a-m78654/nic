# blah: Record.h logappend.cpp logread.cpp
# 	g++ -std=c++11 logappend.o -o logappend 
# 	g++ -std=c++11 logread.o -o logread 
# logappend: logappend.o
# 	g++ -std=c++11 logappend.o -o logappend 
# logread: logread.o
# 	g++ -std=c++11 logread.o -o logread 
all:
	g++ -std=c++11 -g -o logappend logappend.cpp
	g++ -std=c++11 -g -o logread logread.cpp
logappend: 
	g++ -std=c++11 -g -o logappend logappend.cpp
logread:  
	g++ -std=c++11 -g -o logread logread.cpp
clean:
	rm logread logappend

coverage: logappend.o logread.o
	g++ -std=c++11 logappend.o -o logappend -fprofile-arcs -ftest-coverage
	g++ -std=c++11 logread.o -o logread -fprofile-arcs -ftest-coverage