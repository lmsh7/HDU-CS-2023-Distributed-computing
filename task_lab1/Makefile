PROG = lab
CC    = clang
CFLAGS  += -Wall --pedantic -std=c99 -I$(shell pwd) -I../labs_headers
LDFLAGS += -L. -L../ -ldistributedmodel

SRCS := $(PROG).c
OBJS := $(SRCS:.c=.o)

.PHONY : all
all: build $(PROG)

$(PROG): $(OBJS)

.PHONY : clean
clean:
	-rm -f  *.o \
        *.log \
        $(PROG)

build: $(SRCS)
	tar czf $(PROG)1.tar.gz $^
