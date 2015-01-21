all: 

.PHONY: all clean

bin_folder := bin
findsource = $(wildcard $(1)/*.c)
findobj = $(patsubst %.c,$(bin_folder)/%.o,$(1))


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
$(bin_folder)/%.o : %.c
	@mkdir -p $(dir $@)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
##################################################################


## clean #########################################################
clean:
	$(RM) -rf $(bin_folder)
##################################################################


##################################################################
app_source := $(wildcard */*/*.c)
app_bnr_source := $(call findsource,$(app_bnr))
app_bnr_obj := $(call findobj,$(app_bnr_source))
app_bnr : $(app_bnr_obj) librui.a 
	$(LINK.o) $^ -o $@
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@
##################################################################

headers := -I $(lib_dir)

all: $(lib_archive)

biprecommd := app/bipartite_network_recommender




all: $(biprecommd)

$(player): $(libraries)

$(lib_io): $(lib_sc)

$(lib_cn): $(lib_io)


