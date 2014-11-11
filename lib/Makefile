sources := $(wildcard *.c)

.PHONY : all clean

all : libcnrt.a

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources))
endif

COMPILE.c += -std=c99 -g -Wall -Wunused 

%.d : %.c
	@$(CC) -M $(CPPFLAGS) $< > $@.$$$$;                      \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;     \
	rm -f $@.$$$$

libcnrt.a : $(subst .c,.o,$(sources))
	@$(AR) $(ARFLAGS) $@ $?

clean :
	@$(RM) *.d *.o libcnrt.a
