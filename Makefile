TARGETS=dumpansicodes

CORE=libansicnv.a

libansicnv.a: ansibaudrateconverter.o
	rm -fv $@
	ar r $@ $<

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *.a *.la

dumpansicodes: dumpansicodes.o libansicnv.a
	g++ -o $@ $<

dumpansicodes.o: dumpansicodes.cpp
	g++ -c -o $@ $<

ansibaudrateconverter.o: ansibaudrateconverter.cpp
	g++ -c -o $@ $<

