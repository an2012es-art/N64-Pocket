# Standard devkitPro/libnds project Makefile
.SUFFIXES:
.PHONY: all clean

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM is not set. Use the devkitPro MSYS2 terminal.")
endif


TARGET := N64_Pocket
BUILD := build
SOURCES := source
INCLUDES :=
DATA :=
GRAPHICS :=

ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -O2 -ffunction-sections $(ARCH) $(INCLUDE)
CXXFLAGS := $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS := -g $(ARCH)
LDFLAGS := -specs=ds_arm9.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS := -lnds9
LIBDIRS :=

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.bin)))

export OFILES := $(addsuffix .o,$(BINFILES)) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(LIBNDS)/include
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

.PHONY: clean
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nds $(TARGET).elf $(TARGET).map

else
DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).nds: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)
endif

include $(DEVKITARM)/ds_rules
