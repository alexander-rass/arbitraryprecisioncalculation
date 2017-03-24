EXECUTABLE=high_precision_pso
SOURCE_DIRECTORY=src
BINARY_DIRECTORY=bin

.PHONY: all
all:
	$(MAKE) -C $(SOURCE_DIRECTORY)/

.PHONY: test
test: 
	$(MAKE) test -C $(SOURCE_DIRECTORY)


clean:
	$(MAKE) clean -C $(SOURCE_DIRECTORY)

.PHONY: doc

doc:
	$(MAKE) cleandoc && \
	doxygen doc/Doxyfile > /dev/null

cleandoc:
	rm -rf doc/html/*

