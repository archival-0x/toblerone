TARGET    	= toblerone
MAIN_SRCS 	= $(wildcard *.c)
MAIN_OBJS 	= $(MAIN_SRCS:.c=.o)

# install bin path
PREFIX 		= /usr/local

# compiler flags
CC			= gcc
CFLAGS  	= -Wall
LDFLAGS 	= -lX11 -lImlib2 -lcurl


all: $(MAIN_OBJS)
	@$(CC) $(CFLAGS) -o $(TARGET) $(MAIN_OBJS) $(LDFLAGS)


%.o: %.c
	@$(CC) $(CFLAGS) -c -o $@ $<


install: $(TARGET)
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $< $(DESTDIR)$(PREFIX)/bin/$(TARGET)


.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)


clean:
	@rm -f *.o $(TARGET)
