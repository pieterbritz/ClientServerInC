# Makefile
#all: libmycode.a

MAIN_ABS_DIR = $(abspath ../)
SOURCE_PATH  = $(abspath ../)
DEST_UT      = UnitTests
UT_BUILD_DIR = build_$(DEST_UT)

INCLUDES = -I$(SOURCE_PATH)/include -I.

DEFINES = -DUNITTEST

PREFIX=
BUILD_DIR=$(UT_BUILD_DIR)
C_SOURCES  := v_server.c

C_OBJECTS	= $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
C_OBJS		= $(C_OBJECTS:.cpp=.o)
CODE_CFLAGS	= -g -O0 -coverage $(DEFINES) $(INCLUDES)
CFLAGS		= $(CODE_CFLAGS) -std=c99
LDFLAGS = -Wl,-Map=$(BUILD_DIR)/$(DEST_UT).map,--cref -coverage

$(BUILD_DIR)/libmycode.a: $(C_OBJS)
	ar rUv $@ $(C_OBJS)
	ranlib $@

vpath %.cpp $(sort $(dir $(C_SOURCES)))
vpath %.c $(sort $(dir $(C_SOURCES)))

# Coverage filters
LCOV_REMOVE := $(MAIN_ABS_DIR)/UnitTests\*

NOVAPROVA_CFLAGS= $(shell pkg-config --cflags novaprova)
NOVAPROVA_LIBS= $(shell pkg-config --libs novaprova)
NOVAPROVA_INC = -isystem /usr/local/include/novaprova 

CFLAGS= -g -O0 $(NOVAPROVA_CFLAGS) $(NOVAPROVA_INC) $(INCLUDES) $(DEFINES)

check: testrunner
	./testrunner

UT_SOURCE	=	test_server_open.c
UT_OBJECTS	=	$(addprefix $(BUILD_DIR)/,$(notdir $(UT_SOURCE:.c=.o)))

vpath %.cpp $(sort $(dir $(UT_SOURCE)))


$(BUILD_DIR)/%.o:	%.c Makefile
	@echo "[CC] $<"
	$(CC) -c $(CFLAGS) $(LDFLAGS) $< -o $@

$(BUILD_DIR)/%.o:	%.cpp Makefile
	@echo "[CPP] $<"
	$(CXX) -c $(CFLAGS) $(LDFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir $@

testrunner:  $(UT_OBJECTS) $(BUILD_DIR)/libmycode.a
	@echo "[LD] $<"
	$(LINK.c) -o $@ $(UT_OBJECTS) $(BUILD_DIR)/libmycode.a $(NOVAPROVA_LIBS)

clean:
	$(RM) testrunner $(BUILD_DIR)/*

deps:
	@echo $(C_OBJS)
	
list_src:
	@echo $(shell find ../Src -name '*.c')
	
# Run the coverage tests
lcov:	testrunner
	lcov --directory $(UT_BUILD_DIR) --capture --output-file $(UT_BUILD_DIR)/coverage.inf
	lcov --remove $(UT_BUILD_DIR)/coverage.inf $(LCOV_REMOVE) -o $(UT_BUILD_DIR)/coverage.filtered.inf
	genhtml $(UT_BUILD_DIR)/coverage.filtered.inf --output-directory coverage_html --css-file lcov.css

coverage_xml:	testrunner
	gcovr -x -r . -e UnitTests.\* -o coverage.xml


depend: $(C_SOURCES)
	makedepend $(INCLUDES) $(DEFINES) $^


