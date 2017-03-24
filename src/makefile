CFLAGS=-Wall -std=c++11
OPTIMIZATION=-O5
LDFLAGS=-lgmp
DEBUGFLAG=

TEST_PROGRAM_APC=test_program_apc
BUILD_DIR=build

LIB_FILE=libarbitraryprecisioncalculation.a
LIB_LINK=$(subst lib, -l, $(LIB_FILE:.a=))
LIB_DIR=lib

SOURCES=$(filter-out testing/arbitrary_precision_calculation_tests.cpp, $(wildcard */*.cpp))

OBJECTS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
H_FILES=$(SOURCES:.cpp=.h)


all: $(TEST_PROGRAM_APC)
	
$(TEST_PROGRAM_APC): $(LIB_DIR)/$(LIB_FILE) $(BUILD_DIR)/testing/arbitrary_precision_calculation_tests.o $(LIB_DIR)/$(LIB_FILE)
	$(CXX) -L./$(LIB_DIR) $(BUILD_DIR)/testing/arbitrary_precision_calculation_tests.o $(LIB_LINK) $(LDFLAGS) -o $@

$(LIB_DIR)/$(LIB_FILE): $(OBJECTS)
	mkdir -p $(LIB_DIR) && \
    ar rcs $(LIB_DIR)/$(LIB_FILE) $(OBJECTS)

test: $(TEST_PROGRAM_APC)
	./$(TEST_PROGRAM_APC)

$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(addprefix $(BUILD_DIR)/, $(dir $*))
	$(CXX) -I . -MT $@ -MMD -MP -MF $(BUILD_DIR)/$*.temp.d $(CFLAGS) $(DEBUGFLAG) $(OPTIMIZATION) -c -o $(BUILD_DIR)/$*.o $*.cpp
	mv -f $(BUILD_DIR)/$*.temp.d $(BUILD_DIR)/$*.d

$(BUILD_DIR)/%.d: ;

.PRECIOUS: $(BUILD_DIR)/%.d

-include $(OBJECTS:.o=.d)

clean:
	rm -rf $(BUILD_DIR) $(TEST_PROGRAM_APC) $(LIB_DIR)

debug:
	make all "OPTIMIZATION=-O0" "DEBUGFLAG=-g"

debugtest:
	make test "OPTIMIZATION=-O0" "DEBUGFLAG=-g"

