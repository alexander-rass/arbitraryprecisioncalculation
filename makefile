EXECUTABLE=high_precision_pso
SOURCE_DIRECTORY=src
LIB_DIRECTORY=lib

.PHONY: all
all:
	$(MAKE) -C $(SOURCE_DIRECTORY)/ && \
    mkdir -p $(LIB_DIRECTORY) && \
    cp $(SOURCE_DIRECTORY)/$(LIB_DIRECTORY)/* $(LIB_DIRECTORY)

.PHONY: test
test: 
	$(MAKE) test -C $(SOURCE_DIRECTORY)


clean:
	$(MAKE) clean -C $(SOURCE_DIRECTORY) && \
    rm -r $(LIB_DIRECTORY)

.PHONY: doc

doc:
	$(MAKE) cleandoc && \
	doxygen doc/Doxyfile > /dev/null

cleandoc:
	rm -rf doc/html/*

