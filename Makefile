CC=g++
CFLAGS=--std=c++11 -fpermissive -c -g
INC=-I . -I ../libquic/src
LDFLAGS=-L ../libquic/build -l quic -L ../libquic/build/boringssl/ssl -l ssl -L ../libquic/build/boringssl/crypto -l crypto -l pthread
SRCFILES=$(wildcard net/tools/quic/*.cc) $(wildcard net/tools/epoll_server/*.cc)
OBJFILES=$(SRCFILES:.cc=.o)

all: quic_cat_client quic_cat_server

quic_cat_client: $(OBJFILES) quic_cat_client.o
	$(CC) $(OBJFILES) $@.o -o $@ $(LDFLAGS)

quic_cat_server: $(OBJFILES) quic_cat_server.o
	$(CC) $(OBJFILES) $@.o -o $@ $(LDFLAGS)

.cc.o:
	$(CC) $(CFLAGS) $(INC) $< -o $@

print-%:
	@echo $* = $($*)

clean:
	rm $(OBJFILES) quic_cat_client.o quic_cat_server.o
