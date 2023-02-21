#pragma once
#ifndef IMAGESPI_SLAVE_H
#define IMAGESPI_SLAVE_H

#include <ESP32DMASPISlave.h>

#define IMAGE_SIZE_GRAYSCALE_QVGA       320*240
#define IMAGE_SIZE_GRAYSCALE_QQVGA      160*120  
#define IMAGE_SIZE_GRAYSCALE_QQQVGA     80*60 
#define IMAGE_SIZE                      IMAGE_SIZE_GRAYSCALE_QQQVGA

typedef struct esp_32_spi_data {
    size_t len;
    uint8_t *buf;
}esp_32_spi_data;

static const uint32_t BUFFER_SIZE = 20*1024; // IMAGE_SIZE;
extern ESP32DMASPI::Slave slave;

void image_spi_dma_init(void);
void image_rev_send_data(esp_32_spi_data esp_32_spi_data_t);
#endif  // IMAGESPI_SLAVE_H