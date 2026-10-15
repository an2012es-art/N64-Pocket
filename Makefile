TARGET := N64_Pocket
BUILD  := build
SOURCES := source
INCLUDES :=

ARCH := -mthumb -mthumb-interwork

CFLAGS := -g -Wall -O2 -ffunction-sections -fdata-sections
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $@).map

LIBS := -lnds9

include $(DEVKITPRO)/libnds/build_rules
