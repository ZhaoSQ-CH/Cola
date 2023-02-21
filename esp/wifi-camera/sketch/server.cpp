#include "server.h"

#include "Arduino.h"
#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "fb_gfx.h"
#include "img_converters.h"
#include "soc/rtc_cntl_reg.h" // disable brownout problems
#include "soc/soc.h"          // disable brownout problems
#include <WiFi.h>

#include "html_index.h"
#include "image_rev_send.h"
#include "image_debug.h"

#define PART_BOUNDARY "123456789000000000000987654321"

// uint8_t *spi_slave_tx_buf;
// uint8_t *spi_slave_rx_buf;
uint8_t spi_slave_dma_tx_buf[BUFFER_SIZE / 100];
uint8_t spi_slave_dma_rx_buf[BUFFER_SIZE];

httpd_handle_t httpd_index = NULL;
httpd_handle_t stream_httpd = NULL;

static const char *_STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  return httpd_resp_send(req, (const char *)INDEX_HTML, strlen(INDEX_HTML));
}

esp_err_t img_jpg_get(uint8_t *buff, uint32_t buff_len, uint32_t *len,
                      uint8_t **_jpg_buf) {
  esp_err_t res = ESP_OK;
  uint32_t i = 0;
  static uint8_t _data_len = 0;
  static uint8_t state = 0;
  uint8_t sum = 0;

  for (; i < 5; i++) {
    if (buff[i] == 0x85 && buff[++i] == 0xFF && buff[++i] == 0xFF &&
        buff[++i] == 0xFF && buff[++i] == 0xFF) { // 同步数据
      Serial.printf("MJPG find head of frame: ok \n\r");
      *len = ((buff[++i] << 8) | buff[++i]);
      *_jpg_buf = &(buff[i + 1]);
      if (buff[++i] == 0xFF && buff[++i] == 0xD8 && buff[++i] == 0xFF) {
        Serial.printf("MJPG find head of photo: ok\n\r");
        res = ESP_OK;
        goto END;
      } 
    //   else {
    //     Serial.printf("MJPG find head of photo: failed \n\r");
    //     res = ESP_FAIL;
    //     //goto END;
    //   }
    } else {
      Serial.printf("MJPG find head of frame: failed \n\r");
      res = ESP_FAIL;
      //goto END;
      // return ESP_FAIL;
    }
  }
END:
  return res;
}

static esp_err_t stream_handler(httpd_req_t *req) {
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;

  uint8_t *_jpg_buf_NEXT = NULL;
  size_t _jpg_buf_len_Next = 0;
  char *part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  while (true) {
    // set buffer (reply to master) data here
    // if there is no transaction in queue, add transaction
    if (slave.remained() == 0)
      slave.queue(spi_slave_dma_rx_buf, spi_slave_dma_tx_buf, BUFFER_SIZE - 1);

    // if transaction has completed from master,
    // available() returns size of results of transaction,
    // and buffer is automatically updated
    while (slave.available()) {
      // do something here with received data
      res = img_jpg_get(spi_slave_dma_rx_buf, BUFFER_SIZE - 1, &_jpg_buf_len,
                  &_jpg_buf);
      
    //   if(res == ESP_OK) {
    //       _jpg_buf_NEXT = _jpg_buf;
    //       _jpg_buf_len_Next = _jpg_buf_len;
    //   }

    //   if(res == ESP_FAIL){
    //       _jpg_buf = _jpg_buf_NEXT;
    //       _jpg_buf_len = _jpg_buf_len_Next;
    //       res = ESP_OK;
    //   }

      Serial.printf("MJPG: %uB\n\r", (uint32_t)(_jpg_buf_len));
      if (res == ESP_OK) {
        size_t hlen =
            snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
        res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
      }
      if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
      }
      if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY,
                                    strlen(_STREAM_BOUNDARY));
      }
      _jpg_buf = NULL;
      slave.pop();
    }
  }
  return res;
}

static esp_err_t cmd_handler(httpd_req_t *req) {
  char *buf;
  size_t buf_len;
  char variable[32] = {
      0,
  };

  buf_len = httpd_req_get_url_query_len(req) + 1;
  if (buf_len > 1) {
    buf = (char *)malloc(buf_len);
    if (!buf) {
      httpd_resp_send_500(req);
      return ESP_FAIL;
    }
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      if (httpd_query_key_value(buf, "go", variable, sizeof(variable)) ==
          ESP_OK) {
      } else {
        free(buf);
        httpd_resp_send_404(req);
        return ESP_FAIL;
      }
    } else {
      free(buf);
      httpd_resp_send_404(req);
      return ESP_FAIL;
    }
    free(buf);
  } else {
    httpd_resp_send_404(req);
    return ESP_FAIL;
  }

  int res = 0;

  if (!strcmp(variable, "forward")) {
    Serial.println("Forward");
  } else if (!strcmp(variable, "left")) {
    Serial.println("Left");
  } else if (!strcmp(variable, "right")) {
    Serial.println("Right");
  } else if (!strcmp(variable, "backward")) {
    Serial.println("Backward");
  } else if (!strcmp(variable, "stop")) {
    Serial.println("Stop");
  } else {
    res = -1;
  }

  if (res) {
    return httpd_resp_send_500(req);
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, NULL, 0);
}

void StartServer(void) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;
  httpd_uri_t index_url = {.uri = "/",
                           .method = HTTP_GET,
                           .handler = index_handler,
                           .user_ctx = NULL};

  httpd_uri_t stream_uri = {.uri = "/stream",
                            .method = HTTP_GET,
                            .handler = stream_handler,
                            .user_ctx = NULL};

  httpd_uri_t cmd_uri = {.uri = "/action",
                         .method = HTTP_GET,
                         .handler = cmd_handler,
                         .user_ctx = NULL};

  if (httpd_start(&httpd_index, &config) == ESP_OK) {
    httpd_register_uri_handler(httpd_index, &index_url);
    httpd_register_uri_handler(httpd_index, &cmd_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}
