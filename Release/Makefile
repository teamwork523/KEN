# See README.txt for license.

# NOTE:  THIS FILE IS EXPERIMENTAL

# The recommended way to compile Ken programs is to include all .c
# files on a single compile command line.  See "compile.csh".

CC  = gcc  # tested on version 4.6.1
CPP = g++

WARNINGFLAGS = \
    -Wall -W -Wextra -Wundef -Wshadow -Wunreachable-code \
    -Wredundant-decls -Wunused-macros -Wcast-qual -Wcast-align \
    -Wwrite-strings -Wmissing-field-initializers -Wendif-labels \
    -Winit-self -Wlogical-op -Wmissing-declarations -Wpacked \
    -Wstack-protector -Wformat=2 -Wswitch-default -Wswitch-enum \
    -Wunused -Wstrict-overflow=5 -Wpointer-arith -Wnormalized=nfc \
    -Wlong-long

OTHERFLAGS = \
    -D_FORTIFY_SOURCE=2 -ftrapv -fstack-protector-all \
    -fstrict-overflow -g3 -D_XOPEN_SOURCE_EXTENDED

CFLAGS = $(WARNINGFLAGS) $(OTHERFLAGS) \
    -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition \
    -Wbad-function-cast -Wc++-compat -Wjump-misses-init \
    -Wnested-externs

CCFLAGS = $(WARNINGFLAGS) $(OTHERFLAGS) \
    -Weffc++

LIBKEN = libken.so
GETSET = app00 app01 app02  # these use get/set to find heap entry points
KENVAR = app03              # these use kenvar (key/val) interface
CPPAPP = app04              # these use C++ STL

ALL = $(LIBKEN) $(GETSET) $(KENVAR) $(CPPAPP) app04cli
REMINDER = 'remember to "setenv LD_LIBRARY_PATH ." before running'

all : $(ALL)

$(LIBKEN) :
	@echo ''
	$(CC) $(CFLAGS) -fPIC -shared -o $@ ken.c kenext.c kencom.c kenpat.c kencrc.c
	@echo ''

$(GETSET) : app% : $(LIBKEN) app%.c
	@echo ''
	$(CC) $(CFLAGS) -L. -o $@ $@.c          -lken
	@echo '' ; echo $(REMINDER) ; echo ''

$(KENVAR) : app% : $(LIBKEN) app%.c kenvar.c
	@echo ''
	$(CC) $(CFLAGS) -L. -o $@ $@.c kenvar.c -lken
	@echo '' ; echo $(REMINDER) ; echo ''

$(CPPAPP) : app% : $(LIBKEN) app%.cc
	@echo ''
	$(CPP) $(CCFLAGS) -L. -o $@ $@.cc -lken
	@echo '' ; echo $(REMINDER) ; echo ''

app04cli : $(LIBKEN)
	@echo ''
	ln -s app04.cc $@.c
	$(CC) $(CFLAGS) -DAPP04_CLIENT -L. -o $@ $@.c -lken
	rm -f $@.c
	@echo ''

clean :
	rm -f $(ALL) app04cli.c

# see GNU Make Manual sections 14.4 and 14.5 regarding conventions
install : $(LIBKEN)
ifeq ($(prefix),)
	@echo '' ; echo 'ERROR: need environment var "prefix" for' $@ ; echo ''
else
	mv $(LIBKEN) $(DESTDIR)$(prefix)
endif

