##
##  Unix makefile for editline library.
##

##  Set your options:
##	-DANSI_ARROWS		ANSI arrows keys work like emacs.
##	-DHAVE_TCGETATTR	Have tcgetattr(), tcsetattr().
##	-DHAVE_TERMIO		Have "struct termio" and <termio.h>
##	(If neither of above two, we use <sgttyb.h> and BSD ioctl's)
##	-DHIDE			Make static functions static (non debug).
##	-DHIST_SIZE=n		History size.
##	-DUNIQUE_HISTORY	Don't save command if same as last one.
##	-DUSE_DIRENT		Use <dirent.h>, not <sys/dir.h>?
##	-DUSE_TERMCAP		Use the termcap library for terminal size
##				see LDFLAGS, below, if you set this.
##	-DDO_SIGTSTP		Send SIGTSTP on "suspend" key
DEFS	= -DANSI_ARROWS -DHAVE_TCGETATTR -DHIDE -DUSE_DIRENT# -D_GNU_SOURCE

##  Set your C compiler:
WARN	= -Wall -Wshadow -Wpointer-arith -Wcast-qual \
	-Wunused -Wcomment -Wswitch
OPT	= -g
#OPT	= -O
CFLAGS	= $(DEFS) $(WARN) $(OPT)

##  If you have -DUSE_TERMCAP, set this as appropriate:
#LDFLAGS = -ltermlib
#LDFLAGS = -ltermcap

##  Set ranlib as appropriate:
RANLIB	= ranlib
#RANLIB	= echo

##  End of configuration.

SOURCES	= editline.c complete.c sysunix.c
OBJECTS	= editline.o complete.o sysunix.o

all:		libedit.a

testit:		testit.c libedit.a
	$(CC) $(CFLAGS) -o $@ testit.c libedit.a $(LDFLAGS)

clean:
	rm -f *.[oa] testit foo core tags


libedit.a:	$(OBJECTS)
	@rm -f $@
	ar r $@ $(OBJECTS)
	$(RANLIB) $@

$(OBJECTS):	editline.h
