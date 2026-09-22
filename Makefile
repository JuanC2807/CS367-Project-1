#-----------------------------------------------------------------------------
# Makefile
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
# Choose a compiler and its options
#--------------------------------------------------------------------------
CC   = gcc -std=gnu99	# Use gcc for Zeus (gnu99 needed for Linux WCONTINUED)
OPTS = -Og -Wall -Werror -Wmaybe-uninitialized -Wno-error=unused-variable -Wno-error=unused-function -pthread
DEBUG = -g # -g for GDB debugging

#--------------------------------------------------------------------
# Build Environment
#--------------------------------------------------------------------
SRCDIR=./src
OBJDIR=./obj
INCDIR=./inc
LIBDIR=./lib
BINDIR=.

#--------------------------------------------------------------------
# Build Files
#--------------------------------------------------------------------
SRCS=$(wildcard $(SRCDIR)/strawhat*.c)
HDRS=$(wildcard $(INCDIR)/*.h)

#--------------------------------------------------------------------
# Compiler Options
#--------------------------------------------------------------------
INCLUDE=$(addprefix -I,$(INCDIR))
LIBRARY=$(addprefix -L,$(OBJDIR))
SRCOBJS=${SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o}
INCS = $(wildcard $(INCDIR)/*.h)
OBJS=$(OBJDIR)/strawhat.o $(OBJDIR)/strawhat_cs.o $(OBJDIR)/strawhat_shell.o $(OBJDIR)/strawhat_support.o $(OBJDIR)/strawhat_scheduler.o
OTUROBJS=$(OBJDIR)/wlf_sched.o
CFLAGS=$(OPTS) $(INCLUDE) $(LIBRARY) $(DEBUG)

HELPER_TARGETS=$(BINDIR)/slow_countup $(BINDIR)/slow_door $(BINDIR)/slow_bug $(BINDIR)/slow_countdown

#--------------------------------------------------------------------
# Build Recipies for the Executables (binary)
#--------------------------------------------------------------------
TARGET = $(BINDIR)/strawhat 
TARGET_LIB = $(LIBDIR)/strawhat_process.o

all: $(TARGET) helpers
lib: $(TARGET_LIB)

tester: $(TARGET) $(SRCDIR)/test_wlf_sched.c $(OBJDIR)/wlf_sched.o
	${CC} $(CFLAGS) -o $@ $(SRCDIR)/test_wlf_sched.c $(OBJDIR)/wlf_sched.o

helpers: $(HELPER_TARGETS)

$(BINDIR)/slow_countup: $(OBJDIR)/slow_countup.o
	${CC} ${CFLAGS} -o $@ $^

$(BINDIR)/slow_countdown: $(OBJDIR)/slow_countdown.o
	${CC} ${CFLAGS} -o $@ $^

$(BINDIR)/slow_door: $(OBJDIR)/slow_door.o
	${CC} ${CFLAGS} -o $@ $^

$(BINDIR)/slow_bug: $(OBJDIR)/slow_bug.o
	${CC} ${CFLAGS} -o $@ $^  

# Links the object files to create the target binary
$(TARGET): $(OBJS) $(OTUROBJS) $(HDRS) $(INCDIR) $(OBJDIR)/libstrawhat_sd.a
	${CC} ${CFLAGS} -o $@ $(OBJS) $(OTUROBJS) -lstrawhat_sd

#$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c 
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCS)
	${CC} $(CFLAGS) -c -o $@ $<

# Links the object files to create the target binary
$(LIBDIR)/%.o: $(LIBDIR)/%.c $(INCS)
	${CC} $(CFLAGS) -c -o $@ $<
	ar -crs $(LIBDIR)/libstrawhat_sd.a $@; mv $(LIBDIR)/libstrawhat_sd.a $(OBJDIR)/libstrawhat_sd.a

#--------------------------------------------------------------------
# Cleans the binaries
#--------------------------------------------------------------------
clean:
	rm -f $(OBJS) $(SRCOBJS) $(TARGET) $(HELPER_TARGETS) tester $(OBJDIR)/*.o $(LIBDIR)/*.o
