blah: Record.o logappend.o logread.o
	g++ -std=c++11 Record.o logappend.o -o logappend -lssl -lcrypto
	g++ -std=c++11 Record.o logread.o -o logread -lssl -lcrypto
logappend: Record.o logappend.o
	g++ -std=c++11 Record.o logappend.o -o logappend -lssl -lcrypto
logread: Record.o logread.o
	g++ -std=c++11 Record.o logread.o -o logread -lssl -lcrypto
Record.o: Record.h State.h Record.cpp
	g++ -std=c++11 -c Record.cpp 
logappend.o: Record.h logappend.cpp
	g++ -std=c++11 -c logappend.cpp
logread.o: logread.cpp Record.h
	g++ -std=c++11 -c logread.cpp
clean:
	rm *.o logappend
	rm *.o logread
all:
	make logread
	make logappend
coverage: Record.o logappend.o logread.o
	g++ -std=c++11 Record.o logappend.o -o logappend -lssl -lcrypto -fprofile-arcs -ftest-coverage
	g++ -std=c++11 Record.o logread.o -o logread -lssl -lcrypto -fprofile-arcs -ftest-coverage