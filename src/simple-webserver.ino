#include <Husarnet.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp32cam.h>

#define HTTP_PORT 8080

#if __has_include("credentials.h")

// For local development (rename credenials-template.h and type your WiFi and
// Husarnet credentials there)
#include "credentials.h"

#else

// For GitHub Actions OTA deploment

// WiFi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// Husarnet credentials
const char *hostName = HUSARNET_HOSTNAME;
const char *husarnetJoinCode = HUSARNET_JOINCODE;  // find at app.husarnet.com
const char *dashboardURL = "default";

#endif

WebServer server(HTTP_PORT);

void handleMjpeg() {
  // Configure stream
  static struct esp32cam::CameraClass::StreamMjpegConfig mjcfg;
  mjcfg.frameTimeout = 1000;  // ms
  mjcfg.minInterval = 100;    // ms
  mjcfg.maxFrames =
      -1;  // -1 means - send frames until error occurs or client disconnects

  // Actually stream
  auto client = server.client();

  auto startTime = millis();

  int res = esp32cam::Camera.streamMjpeg(client, mjcfg);
  // int res = esp32cam::Camera.streamMjpeg(client);
  if (res <= 0) {
    Serial1.printf("Stream error: %d\n", res);
    return;
  }

  auto duration = millis() - startTime;
  Serial1.printf("Stream end %d frames, on average %0.2f FPS\n", res,
                 1000.0 * res / duration);
}

void setup(void) {
  // ===============================================
  // Wi-Fi and Husarnet VPN configuration
  // ===============================================

  // remap default Serial (used by Husarnet logs)
  Serial.begin(115200, SERIAL_8N1, 16, 17);  // from P3 & P1 to P16 & P17
  Serial1.begin(115200, SERIAL_8N1, 3,
                1);  // remap Serial1 from P9 & P10 to P3 & P1

  Serial1.println("\r\n**************************************");
  Serial1.println("ESP32 IP camera example");
  Serial1.println("**************************************\r\n");

  // Init Wi-Fi
  Serial1.printf("📻 1. Connecting to: %s Wi-Fi network ", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    static int cnt = 0;
    delay(500);
    Serial1.print(".");
    cnt++;
    if (cnt > 10) {
      ESP.restart();
    }
  }

  Serial1.println(" done\r\n");

  // Init Husarnet P2P VPN service
  Serial1.printf("⌛ 2. Waiting for Husarnet to be ready ");

  Husarnet.selfHostedSetup(dashboardURL);
  Husarnet.join(husarnetJoinCode, hostName);
  Husarnet.start();

  // Before Husarnet is ready peer list contains:
  // master (0000:0000:0000:0000:0000:0000:0000:0001)
  const uint8_t addr_comp[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
  bool husarnetReady = 0;
  while (husarnetReady == 0) {
    Serial1.print(".");
    for (auto const &host : Husarnet.listPeers()) {
      if (host.first == addr_comp) {
        ;
      } else {
        husarnetReady = 1;
      }
    }
    delay(1000);
  }

  Serial1.println(" done\r\n");

  // ===============================================
  // PLACE YOUR APPLICATION CODE BELOW
  // ===============================================

  // Configure camera
  // Tested on M5CAMERA X
  esp32cam::Config cfg;

  cfg.setPins(esp32cam::pins::M5CameraLED);
  cfg.setResolution(esp32cam::Resolution::find(320, 240));
  cfg.setBufferCount(2);
  cfg.setJpeg(80);

  bool ok = esp32cam::Camera.begin(cfg);
  if (!ok) {
    Serial1.println("Camera initialization failed");
  }

  // Setup the stream webserver
  server.on("/stream", handleMjpeg);
  server.begin();

  Serial1.println("🚀 HTTP server with a live video stream started\r\n");
  Serial1.printf("Visit:\r\nhttp://%s:%d/stream\r\n\r\n", hostName, HTTP_PORT);

  Serial1.printf("Known hosts:\r\n");
  for (auto const &host : Husarnet.listPeers()) {
    Serial1.printf("%s (%s)\r\n", host.second.c_str(),
                   host.first.toString().c_str());
  }
}

void loop(void) {
  server.handleClient();
  delay(10);
}