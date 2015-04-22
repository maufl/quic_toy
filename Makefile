CC=g++
CFLAGS=--std=c++11 -fpermissive -c
INC=-I . -I ../libquic/src
LDFLAGS=-L ../libquic/build -l quic -L ../libquic/build/boringssl/ssl -l ssl -L ../libquic/build/boringssl/crypto -l crypto -l pthread
SOURCES=$(wildcard *.cc) $(wildcard net/tools/quic/*.cc) $(wildcard net/tools/epoll_server/*.cc)
OBJECTS=$(SOURCES:.cc=.o)

EXECUTABLE=quic_cat_client

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

print-%:
	@echo $* = $($*)

clean:
	rm $(OBJECTS)
