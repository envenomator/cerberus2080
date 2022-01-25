# project config
PROJECTNAME	:= test
STARTADDRESS:= 0x0205
LINKCONFIG	:= cerberus.cfg
SERIALPORT	:= /dev/ttyUSB0

# build config
TARGET_BIN	:= $(PROJECTNAME).bin
BUILD_DIR	:= ./build
SRC_DIRS	:= ./src
AS_BINARY	:= ca65
LNK_BINARY	:= cl65
HEXDUMP		:= xxd -c8 -g1 -o $(STARTADDRESS)
SEND_SCRIPT	:= send.py
ASFLAGS		:=
SRCS		:= $(shell find $(SRC_DIRS) -name '*.s')
OBJS		:= $(subst $(SRC_DIRS),$(BUILD_DIR),$(patsubst %.s,%.o,$(SRCS)))

# Default make target - build to binary file
# Link all objects
$(BUILD_DIR)/$(TARGET_BIN): $(OBJS)
	$(LNK_BINARY) $(OBJS) -C $(LINKCONFIG) -o $@

# Build step for assembly source - assemble each file to an object
$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.s
	@mkdir -p $(BUILD_DIR)
	$(AS_BINARY) $(ASFLAGS) $< -o $@

# Build target and upload immediately
all: $(BUILD_DIR)/$(TARGET_BIN) upload

# send to serial interface
upload:
	$(SEND_SCRIPT) $(TARGET_BIN) $(SERIALPORT)

hexdump: $(BUILD_DIR)/$(TARGET_BIN)
	$(HEXDUMP) $(BUILD_DIR)/$(TARGET_BIN)
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
