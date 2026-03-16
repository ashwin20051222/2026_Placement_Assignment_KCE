CC ?= gcc
CPPFLAGS ?= -D_POSIX_C_SOURCE=200809L
CFLAGS ?= -std=c11 -Wall -Wextra -pedantic
LDLIBS ?= -pthread

SRCS := $(sort $(wildcard *.c))
PROGRAMS := $(SRCS:.c=)

.PHONY: all clean list

all: $(PROGRAMS)

list:
	@printf '%s\n' $(PROGRAMS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(PROGRAMS) a.out
