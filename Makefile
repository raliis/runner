all: polar

CC       ?= gcc
CFLAGS   ?= -Wall -g

CXX      ?= g++
CXXFLAGS ?= -Wall -g

COBJS     = hid.o polar.o protocol.o parse_data.o database.o
CPPOBJS   = 
OBJS      = $(COBJS) $(CPPOBJS)
LIBS      = -lusb-1.0 -lpq -lpthread
INCLUDES ?= `pkg-config libusb-1.0 --cflags` -I/usr/include/postgresql/


polar: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LIBS) -o .bin/polar

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $(INCLUDES) $< -o $@

$(CPPOBJS): %.o: %.c
	$(CXX) $(CXXFLAGS) -c $(INCLUDES) $< -o $@

clean:
	rm -f $(OBJS) ./bin/polar

.PHONY: clean
 