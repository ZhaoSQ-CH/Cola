#include "image_rev_send.h"
#include "image_debug.h"
#include "html_index.h"
#include "wifi_config.h"
#include "server.h"

void setup() {
#if (IMAGE_DEBUG_ENABLE || WIFI_DEBUG_ENABLE)
    image_debug_init();
#endif
    image_spi_dma_init();

    wifi_init();

    StartServer();
}

void loop(){
}
