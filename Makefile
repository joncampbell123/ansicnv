TARGETS=dumpansicodes

CORE=libansicnv.a

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *.a *.la

libansicnv.a: ansibaudrateconverter.o wallclocktime.o
	rm -fv $@
	ar r $@ $^

dumpansicodes: dumpansicodes.o libansicnv.a
	g++ -o $@ $^

dumpansicodes.o: dumpansicodes.cpp
	g++ -c -o $@ $^

wallclocktime.o: wallclocktime.cpp
	g++ -c -o $@ $^

ansibaudrateconverter.o: ansibaudrateconverter.cpp
	g++ -c -o $@ $^

