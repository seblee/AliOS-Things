NAME := board_gokit3_soc

$(NAME)_MBINS_TYPE := kernel
$(NAME)_VERSION    := 1.0.1
$(NAME)_SUMMARY    := configuration for board gokit3_soc
MODULE             := 1062
HOST_ARCH          := xtensa
HOST_MCU_FAMILY    := mcu_esp8266
SUPPORT_MBINS      := no

# todo: remove these after rhino/lwip ready
osal ?= rhino

$(NAME)_COMPONENTS += $(HOST_MCU_FAMILY) kernel_init 

CONFIG_SYSINFO_PRODUCT_MODEL := ALI_AOS_ESP8266
CONFIG_SYSINFO_DEVICE_NAME   := ESP8266

GLOBAL_CFLAGS += -DSYSINFO_PRODUCT_MODEL=\"$(CONFIG_SYSINFO_PRODUCT_MODEL)\"
GLOBAL_CFLAGS += -DSYSINFO_DEVICE_NAME=\"$(CONFIG_SYSINFO_DEVICE_NAME)\"

#for activation handle
GLOBAL_CFLAGS += -DBOARD_ESP8266

GLOBAL_INCLUDES += .         
GLOBAL_INCLUDES += driver/  
$(NAME)_SOURCES := board.c
$(NAME)_SOURCES += driver/hal_rgb_led.c 
$(NAME)_SOURCES += driver/hal_temp_hum.c
$(NAME)_SOURCES += driver/hal_infrared.c
$(NAME)_SOURCES += driver/hal_motor.c

GLOBAL_DEFINES += LOCAL_PORT_ENHANCED_RAND WITH_VENDOR_LWIP

ifeq ($(APP), yts)
GLOBAL_DEFINES += CLI_CONFIG_STACK_SIZE=4096
endif

