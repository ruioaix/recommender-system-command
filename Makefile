bin_folder := bin

findsource = $(wildcard $(1)/*.c)
findobj = $(patsubst %.c,$(bin_folder)/%.o,$(1))

##################################################################
#lib sc
lib_sc := lib/self_contained
lib_sc_source := $(call findsource,$(lib_sc))
lib_sc_obj := $(call findobj,$(lib_sc_source))
#lib io
lib_io := lib/io
lib_io_source := $(call findsource,$(lib_io))
lib_io_obj := $(call findobj,$(lib_io_source))
#lib cn
lib_cn := lib/complex_network
lib_cn_source := $(call findsource,$(lib_cn))
lib_cn_obj := $(call findobj,$(lib_cn_source));
#all lib
lib_source := $(lib_sc_source) $(lib_io_source) $(lib_cn_source)
lib_obj := $(lib_sc_obj) $(lib_io_obj) $(lib_cn_obj)
#
$(lib_io_obj) :
$(bin_folder)/%.o : %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $^ -o $@
##################################################################

##################################################################
app_bnr := app/bipartite_network_recommender
app_bnr_source := $(call findsource,$(app_bnr))
app_bnr_obj := $(call findobj,$(app_bnr_source))
##################################################################

all:
	@echo $(lib_sc_source)
	@echo $(lib_sc_obj)
	@echo $(lib_source)
	@echo $(lib_obj)

biprecommd := app/bipartite_network_recommender

.PHONY: all $(libraries) $(biprecommd)

all: $(biprecommd)

$(player): $(libraries)

$(lib_io): $(lib_sc)

$(lib_cn): $(lib_io)

$(bin_folder)/%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

#include $(patsubst %.c,bin/%.d,$(lib_source))
