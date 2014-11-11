lib := lib
tlkt := toolkit

.PHONY : all clean $(tlkt) $(lib) 

all : $(lib) $(tlkt)

$(tlkt) : $(lib)
	@$(MAKE) -C $@ all

$(lib) :
	@$(MAKE) -C $@ all

clean :
	@for d in $(lib) $(tlkt); \
		do \
		$(MAKE) -C $$d clean; \
		done
