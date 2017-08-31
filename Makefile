TARGETS=dumpansicodes

CORE=libansicnv.a

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *.a *.la

libansicnv.a: ansibaudrateconverter.o wallclocktime.o ansicodeparser.o cp437x08.o cp437x14.o cp437x16.o
	rm -fv $@
	ar r $@ $^

dumpansicodes: dumpansicodes.o libansicnv.a
	g++ -o $@ $^

dumpansicodes.o: dumpansicodes.cpp
	g++ -c -o $@ $^

wallclocktime.o: wallclocktime.cpp
	g++ -c -o $@ $^

ansicodeparser.o: ansicodeparser.cpp
	g++ -c -o $@ $^

ansibaudrateconverter.o: ansibaudrateconverter.cpp
	g++ -c -o $@ $^

cp437x08.o: cp437x08.cpp
	g++ -c -o $@ $^

cp437x14.o: cp437x14.cpp
	g++ -c -o $@ $^

cp437x16.o: cp437x16.cpp
	g++ -c -o $@ $^

