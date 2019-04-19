deps_config := \
	/Users/nathanreeves/esp2/esp-idf/components/app_trace/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/aws_iot/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/bt/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/driver/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/esp32/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/esp_adc_cal/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/esp_event/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/esp_http_client/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/esp_http_server/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/ethernet/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/fatfs/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/freemodbus/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/freertos/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/heap/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/libsodium/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/log/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/lwip/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/mbedtls/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/mdns/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/mqtt/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/nvs_flash/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/openssl/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/pthread/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/spi_flash/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/spiffs/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/tcpip_adapter/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/unity/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/vfs/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/wear_levelling/Kconfig \
	/Users/nathanreeves/esp2/esp-idf/components/app_update/Kconfig.projbuild \
	/Users/nathanreeves/esp2/esp-idf/components/bootloader/Kconfig.projbuild \
	/Users/nathanreeves/esp2/esp-idf/components/esptool_py/Kconfig.projbuild \
	/Users/nathanreeves/esp2/esp-idf/components/partition_table/Kconfig.projbuild \
	/Users/nathanreeves/esp2/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
