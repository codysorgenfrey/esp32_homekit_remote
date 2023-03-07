#include <HomekitRemoteDevice.h>
#include <HomekitRemoteDeviceServerSide.h>
#include <WebSocketsClient.h>
#include <WebSocketsServer.h>

class TestClass : public HomekitRemoteDevice {
public:
  TestClass(WebSocketsClient *ws) : HomekitRemoteDevice(ws, "Test") {}
  void handleHKRCommand(const char *command, const JsonVariant &payload) {
    Serial.printf("handleHKRCommand: %s", command);
  }
  void handleHKRError(HKR_ERROR error) {
    Serial.printf("handleHKRError: %i", error);
  }
};

class TestClass2 : public HomekitRemoteDeviceServerSide {
public:
  TestClass2(WebSocketsServer *ws) : HomekitRemoteDeviceServerSide(ws, "Test") {}
  void handleHKRCommand(const char *command, const JsonVariant &payload) {
    Serial.printf("handleHKRCommand: %s", command);
  }
  void handleHKRError(HKR_ERROR error) {
    Serial.printf("handleHKRError: %i", error);
  }
};

void setup() {
  // put your setup code here, to run once:
  new TestClass(new WebSocketsClient());
  new TestClass2(new WebSocketsServer(81));
}

void loop() {
  // put your main code here, to run repeatedly:
}