# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/")
ifndef GBDK_HOME
	GBDK_HOME = ../
endif

LCC = $(GBDK_HOME)bin/lcc -Wa-l -Wl-m -Wl-j

BUILD_DIR = ../build
BINS = $(BUILD_DIR)/atomas.gb

# Add sound source files
SRCS = atomas.c sound.c display.c game.c sintables.c gamebg.c gameover.c
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif

all: $(BUILD_DIR) $(BINS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ -p\/mkdir\/ | grep -v make >> compile.bat

# Compile and link single file in one pass
$(BUILD_DIR)/%.gb: $(OBJS)
	$(LCC) $(LCCFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(LCC) $(LCCFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)
