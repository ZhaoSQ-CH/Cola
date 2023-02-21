#include "image_rev_send.h"
#include "image_debug.h"

#include "esp_camera.h"

ESP32DMASPI::Slave slave;

uint8_t *spi_slave_rx_buf;
uint8_t *spi_slave_tx_buf;

void image_spi_dma_init(void) {

#if IMAGE_DEBUG_ENABLE
  printf("image spi dma start ...\n\r");
#endif

//   // to use DMA buffer, use these methods to allocate buffer
//   spi_slave_tx_buf = slave.allocDMABuffer(BUFFER_SIZE);
//   spi_slave_rx_buf = slave.allocDMABuffer(BUFFER_SIZE);

  slave.setDataMode(SPI_MODE3);
  slave.setMaxTransferSize(BUFFER_SIZE);
  slave.setDMAChannel(1); // 1 or 2 only
  slave.setQueueSize(1);  // transaction queue size

  // begin() after setting
  // HSPI = CS: 15, CLK: 14, MOSI: 13, MISO: 12
  // VSPI = CS: 5, CLK: 18, MOSI: 23, MISO: 19
  slave.begin(HSPI);

#if IMAGE_DEBUG_ENABLE
  printf("image spi dma finish ...\n\r");
#endif
}

// void image_rev_send_data(esp_32_spi_data esp_32_spi_data_t) {
//   // set buffer (reply to master) data here
//   // if there is no transaction in queue, add transaction
//   if (slave.remained() == 0)
//     slave.queue(spi_slave_rx_buf, spi_slave_tx_buf, BUFFER_SIZE);

//   // if transaction has completed from master,
//   // available() returns size of results of transaction,
//   // and buffer is automatically updated
//   while (slave.available()) {
// #if IMAGE_DEBUG_ENABLE
//     printf("image spi dma send start ...\n\r");
// #endif

// #if IMAGE_DEBUG_ENABLE
//     uint16_t frame_head[2] = {0x01, 0xFE};
//     uint16_t frame_end[2] = {0xFE, 0x01};
//     Serial.write(frame_head[0]);
//     Serial.write(frame_head[1]);

//     // do something here with received data
//     for (size_t i = 0; i < IMAGE_SIZE; ++i) {
//       Serial.write(spi_slave_rx_buf[i]);
//     }
//     Serial.write(frame_end[0]);
//     Serial.write(frame_end[1]);
// #endif
//     slave.pop();
//   }
// }
