CFLAGS=-Wall -std=c++11
OPTIMIZATION=-O5
LDFLAGS=-lgmp
DEBUGFLAG=

TEST_PROGRAM_APC=test_program_apc
BUILD_DIR=build

SOURCES=$(filter-out testing/arbitrary_precision_calculation_tests.cpp, $(wildcard */*.cpp))

OBJECTS=$(addprefix $(BUILD_DIR)/, $(SOURCES:.cpp=.o))
H_FILES=$(SOURCES:.cpp=.h)


all: $(TEST_PROGRAM_APC)
	
$(TEST_PROGRAM_APC): $(OBJECTS) $(BUILD_DIR)/testing/arbitrary_precision_calculation_tests.o testing/arbitrary_precision_calculation_tests.cpp
	$(CXX) $(OBJECTS) $(BUILD_DIR)/testing/arbitrary_precision_calculation_tests.o $(LDFLAGS) -o $@


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
	rm -rf $(BUILD_DIR) $(TEST_PROGRAM_APC)

debug:
	make all "OPTIMIZATION=-O0" "DEBUGFLAG=-g"

debugtest:
	make test "OPTIMIZATION=-O0" "DEBUGFLAG=-g"

