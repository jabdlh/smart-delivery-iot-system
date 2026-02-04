#include "globals.h"
#include <DHT.h>
#include "esp_camera.h"
#include "motor_control.h" 
#include <Arduino.h>
// MQTT Topics
const char*TOPIC_TEMPERATURE = "esp32/temperature";  // Define the sensor topic
const char* TOPIC_IMAGE = "esp32/image";
const char* TOPIC_CONTROL = "esp32/control";
const char* TOPIC_WEIGHT = "esp32/weight";
const char* TOPIC_STATUS = "esp32/status";
// Wi-Fi credentials
/*const char* WIFI_SSID = "Jabdlh";
const char* WIFI_PASS = "0552700489";

const char* WIFI_SSID = "ip";
const char* WIFI_PASS = "1234567891";

const char* WIFI_SSID = "ip";
const char* WIFI_PASS = "1234567891";

const char* WIFI_SSID = "IPHONE13";
const char* WIFI_PASS = "0987654321";



const char* WIFI_SSID = "Jabdlh";
const char* WIFI_PASS = "0552700489";

const char* WIFI_SSID = "M";
const char* WIFI_PASS = "muaz12345";

*/

const char* WIFI_SSID = "JABDLH 1951";
const char* WIFI_PASS = "81Ga351>";

// MQTT credentials
const char* MQTT_SERVER = "put your MQTT server info here";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "MQTT user";
const char* MQTT_PASS = "MQTT password";
const char* CLIENT_ID = "Your Client ID";

// Camera pin definitions for the WROVER model

const int PWDN_GPIO_NUM = -1;  // Power down pin
const int RESET_GPIO_NUM = -1;  // Reset pin
const int XCLK_GPIO_NUM = 21;
const int SIOD_GPIO_NUM = 26;
const int SIOC_GPIO_NUM = 27;
const int Y9_GPIO_NUM = 35;
const int Y8_GPIO_NUM = 34;
const int Y7_GPIO_NUM = 39;
const int Y6_GPIO_NUM = 36;
const int Y5_GPIO_NUM = 19;
const int Y4_GPIO_NUM = 18;
const int Y3_GPIO_NUM = 5;
const int Y2_GPIO_NUM = 4;
const int VSYNC_GPIO_NUM = 25;
const int HREF_GPIO_NUM = 23;
const int PCLK_GPIO_NUM = 22;




//components Pins

  //const int CONVEYORPIN;
 //const int LOCKPIN = 12;

  const int LED_PIN = 2;

//pinMode(LED_PIN, OUTPUT);
  
// Declare the Wi-Fi client and MQTT client globally
WiFiClientSecure tlsClient;  // Secure Wi-Fi client
PubSubClient mqttClient(tlsClient);  // MQTT client using the secure Wi-Fi client

// Define the CA certificate (Let's Encrypt Root CA for HiveMQ Cloud)
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----

)EOF";


