TARGETS=dumpansicodes

all: $(TARGETS)

clean:
	rm -f $(TARGETS) *.o *.a *.la

dumpansicodes: dumpansicodes.o
	g++ -o $@ $<

dumpansicodes.o: dumpansicodes.cpp
	g++ -c -o $@ $<

