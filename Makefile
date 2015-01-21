.PHONY: all clean first
all: 

## variables and functions #######################################
RM := rm -rf
CC := gcc-4.9
bin_folder := bin
findsource = $(wildcard $(1)/*.c)
findobj = $(patsubst %.c,$(bin_folder)/%.o,$(1))
app_sources := $(wildcard app/*/*.c)
app_outpout := $(basename $(app_sources))
##################################################################


## librui.a ######################################################
lib_folder := lib
lib_sources := $(call findsource,$(lib_folder))
lib_objects := $(call findobj,$(lib_sources))
lib_archive := $(bin_folder)/$(lib_folder)/librui.a
$(lib_archive) : $(lib_objects)
	@$(AR) $(ARFLAGS) $@ $?
##################################################################


## automatic prerequisites #######################################
include $(patsubst %.c,$(bin_folder)/%.d,$(lib_sources))
$(bin_folder)/%.d: %.c
	@mkdir -p $(dir $@)
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
##################################################################


## .o compiled from .c ##########################################
CPPFLAGS += -I$(lib_folder)
%.o : %.c
$(bin_folder)/%.o : %.c
	@mkdir -p $(dir $@)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
##################################################################

## x compiled from x.o and libs ##################################
apps := $(patsubst %.c,$(bin_folder)/%,$(wildcard app/*/*.c))
$(apps) : % : %.o $(lib_archive)
	@mkdir -p $(dir $@)
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
##################################################################

## clean #########################################################
clean:
	$(RM) $(bin_folder)
##################################################################

all: $(apps)
