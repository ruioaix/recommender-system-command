sources := $(wildcard *.c)
libobj := $(subst .c,.o,$(sources))
exec := run_check

headers := -I ../lib
libcnrt := ../lib/libcnrt.a
extralibs := -lm -lcheck  -lssl -lcrypto

.PHONY : all clean 

all : $(exec)

$(exec) : $(libobj) $(libcnrt) 
	$(LINK.o) $^ $(extralibs) -o $@

COMPILE.c += $(headers)

ifneq ($(MAKECMDGOALS),clean)
-include $(subst .c,.d,$(sources))
endif

%.d : %.c
	$(CC) -M $(CPPFLAGS) $(headers) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean :
	@$(RM) *.o *.d $(exec) 
