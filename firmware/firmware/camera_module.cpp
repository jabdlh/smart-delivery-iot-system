#include "camera_module.h"
#include "globals.h"
#include <mbedtls/base64.h>
#include <esp_camera.h>
#include <Arduino.h>

// Setup camera
void setupCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = Y2_GPIO_NUM;
  config.pin_d1       = Y3_GPIO_NUM;
  config.pin_d2       = Y4_GPIO_NUM;
  config.pin_d3       = Y5_GPIO_NUM;
  config.pin_d4       = Y6_GPIO_NUM;
  config.pin_d5       = Y7_GPIO_NUM;
  config.pin_d6       = Y8_GPIO_NUM;
  config.pin_d7       = Y9_GPIO_NUM;
  config.pin_xclk     = XCLK_GPIO_NUM;
  config.pin_pclk     = PCLK_GPIO_NUM;
  config.pin_vsync    = VSYNC_GPIO_NUM;
  config.pin_href     = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn     = PWDN_GPIO_NUM;
  config.pin_reset    = RESET_GPIO_NUM;
  config.xclk_freq_hz = 10000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size   = FRAMESIZE_SVGA; // increase to QVGA after tests
  config.jpeg_quality = 10;
  config.fb_count     = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed 0x%x\n", err);
    while (true) delay(1000);
  }
  Serial.println("Camera initialized");

  // Flip vertically if upside down
  sensor_t * s = esp_camera_sensor_get();
  s->set_vflip(s, 1);   // 0 = normal, 1 = flipped vertically

  // Mirror horizontally if needed
  s->set_hmirror(s, 0); // 0 = normal, 1 = mirrored

  // 1. Lower the overall exposure/brightness to reduce glare
  // Value can range from -2 to 2. -1 is a good starting point to compensate for bright overhead light.
  s->set_ae_level(s, -2);
  
  // 2. Reduce the Gain Ceiling (Max signal amplification)
  // This limits the camera's ability to boost brightness, controlling highlight blowout.
  // Value: 2 (4x gain) is much lower than the default of 4 (16x gain) or 5 (32x gain).
  s->set_gainceiling(s, (gainceiling_t)2);
  
  // 3. Disable Auto White Balance (AWB) if colors look weird due to bright light
  // If the reflection is causing a blue or yellow tint, turn this off and set manual balance.
  // s->set_whitebal(s, 0); // 0 = Disabled, 1 = Enabled (Default)
  // s->set_awb_gain(s, 1); // Only effective if AWB is enabled.

  // 3. NEW: Increase Brightness to compensate for overall scene darkness (Value: -2 to 2)
  // Set to 1 (or even 2) to lift the shadows and mid-tones.
  s->set_brightness(s, -1);
}

//not used in this project
// Function to turn off the camera
void turnOffCamera() {
  // Deinitialize the camera to free up resources
  esp_camera_deinit();
  Serial.println("Camera turned off");
}

void captureAndPublish() {
  // Flush old frame by grabbing and discarding one
  camera_fb_t* temp = esp_camera_fb_get();
  if (temp) esp_camera_fb_return(temp);
  delay(600); // allow sensor to refresh

  // Now grab fresh frame
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  size_t jpegLen = fb->len;
  size_t b64Len = 4 * ((jpegLen + 2) / 3);
  Serial.printf("JPEG %u bytes -> base64 %u bytes\n", (unsigned)jpegLen, (unsigned)b64Len);

  uint8_t* out = (uint8_t*)malloc(b64Len + 1);
  bool singleOk = false;
  if (out) {
    size_t produced = 0;
    int rc = mbedtls_base64_encode(out, b64Len + 1, &produced, fb->buf, jpegLen);
    if (rc == 0) {
      Serial.printf("Base64 encoded %u bytes\n", (unsigned)produced);
      singleOk = publishImageSingle(out, produced);
    } else {
      Serial.printf("Base64 encode failed rc=%d\n", rc);
    }
    free(out);
  } else {
    Serial.println("base64 buffer malloc failed");
  }

  if (!singleOk) {
    Serial.println("Falling back to chunked publish");
    publishImageChunked(fb->buf, fb->len);
  }

  esp_camera_fb_return(fb);
}

// single publish + immediate EOF marker
bool publishImageSingle(const uint8_t* buf, size_t len) {
  #if defined(PubSubClient_h)
 mqttClient.setBufferSize(8192);
  #endif

  bool ok =mqttClient.publish(TOPIC_IMAGE, buf, len, false);
  Serial.printf("Single publish ok=%d, published %u bytes,mqttClient.state=%d\n", ok?1:0, (unsigned)len,mqttClient.state());

  if (ok) {
    delay(400); // tiny pause to allow broker to process main publish
   mqttClient.publish(TOPIC_IMAGE, (uint8_t*)"EOF", 3, false);
    Serial.println("Sent EOF after single publish");
  }
  return ok;
}

// Chunked image publishing
void publishImageChunked(const uint8_t* data, size_t len) {
  const size_t outChunkChars = 1024;
  size_t adjustedOutChunk = outChunkChars - (outChunkChars % 4);
  const size_t inChunkBytes = (adjustedOutChunk / 4) * 3;

  char* temp = (char*)malloc(adjustedOutChunk + 1);
  if (!temp) {
    Serial.println("Chunk temp malloc failed");
    return;
  }

  size_t offset = 0;
  while (offset < len) {
    size_t thisIn = (len - offset >= inChunkBytes) ? inChunkBytes : (len - offset);
    size_t produced = 0;
    int rc = mbedtls_base64_encode((unsigned char*)temp, adjustedOutChunk + 1, &produced, data + offset, thisIn);
    if (rc != 0) {
      Serial.printf("Chunk base64 encode rc=%d\n", rc);
      break;
    }
   mqttClient.publish(TOPIC_IMAGE, (uint8_t*)temp, produced, false);
    Serial.printf("Published chunk offset=%u in=%u b64=%u\n", (unsigned)offset, (unsigned)thisIn, (unsigned)produced);
    offset += thisIn;
    delay(10);
  }

 mqttClient.publish(TOPIC_IMAGE, (uint8_t*)"EOF", 3, false);  // Send EOF marker after all chunks are sent
  Serial.println("Chunked publish complete (EOF sent)");
  free(temp);
}
