ifeq ($(strip $(DEVKITPRO)),)
$(error "DEVKITPRO is not set. Please install devkitPro.")
endif

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM is not set. Please install devkitARM.")
endif

TARGET := N64_Pocket
BUILD := build
SOURCES := source
DATA :=
INCLUDES :=

ARCH := -mthumb -mthumb-interwork

CFLAGS := -g -Wall -O2 -ffunction-sections -fdata-sections
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS = -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $@).map

LIBS := -lnds9
LIBDIRS := $(LIBNDS)

include $(DEVKITPRO)/libnds/build_rules
