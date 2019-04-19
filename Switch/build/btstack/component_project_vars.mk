# Automatically generated build file. Do not edit.
COMPONENT_INCLUDES += $(IDF_PATH)/components/btstack/3rd-party/bluedroid/decoder/include $(IDF_PATH)/components/btstack/3rd-party/bluedroid/encoder/include $(IDF_PATH)/components/btstack/3rd-party/hxcmod-player $(IDF_PATH)/components/btstack/3rd-party/hxcmod-player/mods $(IDF_PATH)/components/btstack/3rd-party/md5 $(IDF_PATH)/components/btstack/3rd-party/yxml $(IDF_PATH)/components/btstack/src/classic $(IDF_PATH)/components/btstack/src/ble/gatt-service $(IDF_PATH)/components/btstack/src/ble $(IDF_PATH)/components/btstack/src/classic $(IDF_PATH)/components/btstack/src $(IDF_PATH)/components/btstack/platform/embedded $(IDF_PATH)/components/btstack/platform/freertos $(IDF_PATH)/components/btstack/include
COMPONENT_LDFLAGS += -L$(BUILD_DIR_BASE)/btstack -lbtstack
COMPONENT_LINKER_DEPS += 
COMPONENT_SUBMODULES += 
COMPONENT_LIBRARIES += btstack
COMPONENT_LDFRAGMENTS += 
component-btstack-build: component-micro-ecc-build
