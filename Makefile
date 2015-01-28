.PHONY: all clean help
#no PHONY targets: $(lib_archive), $(bin_folder)/%.d, $(bin_folder)/%.o, $(apps)
all:

## variables and functions #######################################
RM := rm -rf
CC := gcc-4.9
CFLAGS += -Wall -g
CPPFLAGS += -I$(lib_folder)
bin_folder := bin
findsource = $(wildcard $(1)/*.c)
findobj = $(patsubst %.c,$(bin_folder)/%.o,$(1))
##################################################################


## librui.a ######################################################
lib_folder := lib
lib_sources := $(call findsource,$(lib_folder))
lib_objects := $(call findobj,$(lib_sources))
lib_archive := $(bin_folder)/$(lib_folder)/librui.a
$(lib_archive) : $(lib_objects)
	$(AR) $(ARFLAGS) $@ $?
##################################################################


## app ###########################################################
app_folder := app
app_sources := $(wildcard $(app_folder)/*/*.c)
apps := $(patsubst %.c,$(bin_folder)/%,$(app_sources))
##################################################################


## all ###########################################################
all_sources := $(lib_sources) $(app_sources)
all_objects := $(lib_objects) $(lib_archive) $(apps)
all_folders_in_bin := $(sort $(dir $(all_objects)))
##################################################################


## make sure directory is existed#################################
makesuredir := $(shell mkdir -p $(all_folders_in_bin))
##################################################################


## automatic prerequisites (working for both lib and app) ########
autodependence := $(patsubst %.c,$(bin_folder)/%.d,$(all_sources))
ifneq "$(MAKECMDGOALS)" "clean"
	include $(autodependence)
endif
$(bin_folder)/%.d: %.c
	set -e; rm -f $@; \
		$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
		sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
		rm -f $@.$$$$
##################################################################


## .o compiled from .c  (working for both lib and app) ###########
$(bin_folder)/%.o : %.c 
	$(COMPILE.c) $(OUTPUT_OPTION) $<
##################################################################


## x compiled from x.o and libs ##################################
LDLIBS += -lm
% : %.o $(lib_archive)
	$(LINK.o) $^ $(LDLIBS) -o $@
	@cp -v $@ $(subst /,-,$@)
##################################################################


## clean #########################################################
clean :
	@$(RM) $(bin_folder)
	@$(RM) bin-*
##################################################################


## help #########################################################
help :
#$(MAKE) --print-data-base --question no-such-target | grep -v -e '^no-such-target' -e '^makefile' | awk '/^[^.%][-A-Za-z0-9_]*:/ { print substr($$1, 1, length($$1)-1) }' | sort | pr --omit-pagination --width=80 --columns=4
##################################################################

all: $(apps)
