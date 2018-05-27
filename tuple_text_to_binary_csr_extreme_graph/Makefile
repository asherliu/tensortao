SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
		done

clean: $(SUBDIRS)
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
		done

.PHONY: all $(SUBDIRS)


