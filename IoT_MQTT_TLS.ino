/*
   Mosquitto,TLS,ESP32
   Project URL: https://github.com/XXXXXXXXXXXXX

   Created on: Oct 06, 2017
   Author : Jorge Francisco B. Melo
   e-mail : jorgefrancisco.melo@gmail.com
   Country: Brazil

   Modified on: Oct 22, 2017
   Author : Jorge Francisco B. Melo
   e-mail : jorgefrancisco.melo@gmail.com
   Country: Brazil

   Project IOT

   Copyright (c) 2017 Jorge Francisco B. Melo
*/

//-----------------------------------------------------------------------------------------------------------------

#include <WiFiClientSecure.h>
#include <MqttClient.h>
//-----------------------------------------------------------------------------------------------------------------

// Define CA certificate:

const char ca_cert[] =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDqDCCApCgAwIBAgIJAP5vkh20sJHZMA0GCSqGSIb3DQEBCwUAMGkxCzAJBgNV\n"
  "BAYTAkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBX\n"
  "aWRnaXRzIFB0eSBMdGQxCjAIBgNVBAsMATExFjAUBgNVBAMMDTE5Mi4xNjguMC4x\n"
  "MDUwHhcNMTcxMDA4MTUzNDE4WhcNMjcxMDA2MTUzNDE4WjBpMQswCQYDVQQGEwJB\n"
  "VTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0\n"
  "cyBQdHkgTHRkMQowCAYDVQQLDAExMRYwFAYDVQQDDA0xOTIuMTY4LjAuMTA1MIIB\n"
  "IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyObGfkvw24qJJDkIArmHhEYW\n"
  "wQp5I5xIGFtMoybQ9b+t5RzB0qzlMAOaCx/yBnrr9YFnA3svthR8KQZnnFeufRHT\n"
  "BT98AQ74CytxPRKlRLSS5iB7a1Jwc+WKpOdb2cx2HH2r7InnNnBU7BZdq/LTwNK/\n"
  "pcquHX960PnzW4icMQbgedT+A7x+t0UdtITMOWhViIVay84+8IksISC9Pk8A7Jxi\n"
  "IrFbw48blEq7fuyh2hrflIJg8Htllv6ObBS4KQZhykk0nvtm0ZoPYRD07ksaoov2\n"
  "zq/DcwH1BGRkiO+cWTuy79I/X8sKncdStUKphnZuWAGmTFoe0lmuMTupAcSJgQID\n"
  "AQABo1MwUTAdBgNVHQ4EFgQUv0NOxG7w0ImrjVR8ZABrXFUFb4swHwYDVR0jBBgw\n"
  "FoAUv0NOxG7w0ImrjVR8ZABrXFUFb4swDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG\n"
  "9w0BAQsFAAOCAQEAGPQNyJVtHeM8CknBAYI+q3JMMlVrRprS2GdaomSqKzvp9FOq\n"
  "G6u7kq/ysvM+V31CWvckKmjV4dsf62QFW3emvtxhHuVUk9WhNLMv+49+8C8tWmnq\n"
  "Fm2syK4E7USqL2Vmi5/uRcbW2mXMEaJqz3Uq93230kZFP0lGORcmCj0wdK/itTuM\n"
  "Z0hX3fHML9TsInBUWcmaMm9yYXxQOcpDfHAIfEmqZoCTE6zuntddDNGfCUJFPNwY\n"
  "NplObd63KhuJRpU9JcDf1YvF/S5IySzSDFslqMNlCkhqIKWs/ueufKbC1+Peb8Tt\n"
  "+izhufWNcm8341xkelnNcwZOesXBrSF4F7RvXQ==\n"
  "-----END CERTIFICATE-----\n";
//-----------------------------------------------------------------------------------------------------------------

// Defines MQTT:

#define MQTT_LOG_ENABLED 1
#define MQTT_ID "TEST-ID"
#define SUBSCRIBE_TOPIC "Table"
#define PUBLISH_TOPIC "Table"

static MqttClient *mqtt = NULL;
const char* mqttBrokerAddress = "192.168.0.105";
const int mqttBrokerPort = 8883;
//-----------------------------------------------------------------------------------------------------------------

// Defines HW:

#define HW_UART_SPEED 115200L
//-----------------------------------------------------------------------------------------------------------------

// Defines WiFi:

const char* ssid     = "XXXXXX";     // your network SSID
const char* password = "XXXXXXX"; // your network password

WiFiClientSecure client;
//-----------------------------------------------------------------------------------------------------------------

// Utility functions:

#define withLn true
#define withoutLn false
#define LOG_PRINTFLN(fmt, ...)  logFln(fmt,withLn, ##__VA_ARGS__)
#define LOG_PRINTF(fmt, ...)  logFln(fmt,withoutLn, ##__VA_ARGS__)
#define LOG_SIZE_MAX 128

void logFln(const char *fmt, bool lnOption, ...) {
  char buf[LOG_SIZE_MAX];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, LOG_SIZE_MAX, fmt, ap);
  va_end(ap);

  if (lnOption) {
    Serial.println(buf);
  } else {
    Serial.print(buf);
  }
}
//-----------------------------------------------------------------------------------------------------------------

class System: public MqttClient::System {
  public:

    unsigned long millis() const {
      return ::millis();
    }

    void yield(void) {
      ::yield();
    }
};
//-----------------------------------------------------------------------------------------------------------------

void connectTCPMQTTBroker(void) {
  if (!client.connected()) {
    LOG_PRINTFLN("");
    LOG_PRINTFLN("Begin connectTCPMQTTBroker");

    LOG_PRINTFLN("Establishing TCP connection with the MQTT broker...");

    unsigned countTries = 0;

    // Set SSL/TLS certificate
    client.setCACert(ca_cert);

    client.connect(mqttBrokerAddress, mqttBrokerPort);

    if (!client.connected()) {
      LOG_PRINTF("TCP connection failed! ");
    } else {
      LOG_PRINTF("Established TCP connection with the MQTT broker! ");
    }

    LOG_PRINTF("MQTT Broker address: %s. ", mqttBrokerAddress);
    LOG_PRINTF("MQTT Broker port: %i. ", mqttBrokerPort);

    LOG_PRINTFLN("");
    LOG_PRINTFLN("End connectTCPMQTTBroker");
    LOG_PRINTFLN("");
  }
}
//-----------------------------------------------------------------------------------------------------------------

void disconnectTCPMQTTBroker(void) {
  if (client.connected()) {
    LOG_PRINTFLN("");
    LOG_PRINTFLN("Begin disconnectTCPMQTTBroker");

    LOG_PRINTF("Stopping TCP connection with the MQTT broker...");

    client.stop();
    client.flush();

    while (client.connected()) {
      LOG_PRINTF(".");
      delay(100);
    }

    LOG_PRINTFLN("");
    LOG_PRINTFLN("End connectTCPMQTTBroker");
    LOG_PRINTFLN("");
  }
}
//-----------------------------------------------------------------------------------------------------------------

bool isConnectedTCPMQTTBroker(void) {
  return client.connected();
}
//-----------------------------------------------------------------------------------------------------------------

void connectServiceMQTTBroker(void) {
  if (!mqtt->isConnected()) {
    LOG_PRINTFLN("");
    LOG_PRINTFLN("Begin connectServiceMQTTBroker");

    LOG_PRINTFLN("Establishing service connection with the MQTT broker...");

    // Start new MQTT connection
    MqttClient::ConnectResult connectResult;

    MQTTPacket_connectData options = MQTTPacket_connectData_initializer;

    options.MQTTVersion = 4;
    options.clientID.cstring = (char*)MQTT_ID;
    options.cleansession = true;
    options.keepAliveInterval = 15; // 15 seconds

    MqttClient::Error::type rc = mqtt->connect(options, connectResult);

    if (rc != MqttClient::Error::SUCCESS) {
      LOG_PRINTF("Service connection failed! ");
      LOG_PRINTF("This client ID: %s. ", options.clientID.cstring);
      LOG_PRINTF("Connection error: %i. ", rc);
    } else {
      LOG_PRINTF("Established service connection with the MQTT broker! ");
      LOG_PRINTF("This client ID: %s. ", options.clientID.cstring);
    }

    LOG_PRINTFLN("");
    LOG_PRINTFLN("End connectServiceMQTTBroker");
    LOG_PRINTFLN("");
  }
}
//-----------------------------------------------------------------------------------------------------------------

bool isConnectedServiceMQTTBroker(void) {
  return mqtt->isConnected();
}
//-----------------------------------------------------------------------------------------------------------------

char* statusWiFiString(int i) {
  char* statusName = "WL_CONNECTED";

  switch (i) {
    case WL_CONNECTED: statusName = "WL_CONNECTED";
      break;

    case WL_NO_SHIELD: statusName = "WL_NO_SHIELD";
      break;

    case WL_IDLE_STATUS: statusName = "WL_IDLE_STATUS";
      break;

    case WL_NO_SSID_AVAIL: statusName = "WL_NO_SSID_AVAIL";
      break;

    case WL_SCAN_COMPLETED: statusName = "WL_SCAN_COMPLETED";
      break;

    case WL_CONNECT_FAILED: statusName = "WL_CONNECT_FAILED";
      break;

    case WL_CONNECTION_LOST: statusName = "WL_CONNECTION_LOST";
      break;

    case WL_DISCONNECTED: statusName = "WL_DISCONNECTED";
      break;

    default: statusName = "WL_UNKNOWN";
      break;
  }

  return statusName;
}
//-----------------------------------------------------------------------------------------------------------------

void connectWiFi(void) {
  if (WiFi.status() != WL_CONNECTED) {
    LOG_PRINTFLN("");
    LOG_PRINTFLN("Begin connectWiFi");

    LOG_PRINTF("Establishing WiFi connection with the Router...");

    unsigned countTries = 0;

    WiFi.mode(WIFI_STA);
    //WiFi.setHostname("ESP_" MQTT_ID);
    WiFi.begin(ssid, password);

    // Waiting for WiFi connect
    while ((WiFi.status() != WL_CONNECTED) &&
           (countTries < 100)) {
      LOG_PRINTF(".");
      delay(100);

      ++countTries;
    }

    LOG_PRINTFLN("");

    if (WiFi.status() != WL_CONNECTED) {
      LOG_PRINTF("WiFi connection failed! ");
    } else {
      LOG_PRINTF("Established WiFi connection with the Router! ");
      LOG_PRINTF("IP: %s. ", WiFi.localIP().toString().c_str());
    }

    LOG_PRINTF("SSID: %s. ", ssid);
    LOG_PRINTF("Status: %s. ", statusWiFiString(WiFi.status()));

    LOG_PRINTFLN("");
    LOG_PRINTFLN("End connectWiFi");
    LOG_PRINTFLN("");
  }
}
//-----------------------------------------------------------------------------------------------------------------

bool isConnectedWiFi(void) {
  return (WiFi.status() == WL_CONNECTED);
}
//-----------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(HW_UART_SPEED);

  //WiFi.printDiag(Serial);

  connectWiFi();

  connectTCPMQTTBroker();

  // Setup MqttClient
  MqttClient::System *mqttSystem = new System;
  MqttClient::Logger *mqttLogger = new MqttClient::LoggerImpl<HardwareSerial>(Serial);
  MqttClient::Network * mqttNetwork = new MqttClient::NetworkClientImpl<WiFiClient>(client, *mqttSystem);
  //// Make 128 bytes send buffer
  MqttClient::Buffer *mqttSendBuffer = new MqttClient::ArrayBuffer<128>();
  //// Make 128 bytes receive buffer
  MqttClient::Buffer *mqttRecvBuffer = new MqttClient::ArrayBuffer<128>();
  //// Allow up to 2 subscriptions simultaneously
  MqttClient::MessageHandlers *mqttMessageHandlers = new MqttClient::MessageHandlersImpl<2>();
  //// Configure client options
  MqttClient::Options mqttOptions;
  ////// Set command timeout to 10 seconds
  mqttOptions.commandTimeoutMs = 10000;
  //// Make client object
  mqtt = new MqttClient(
    mqttOptions, *mqttLogger, *mqttSystem, *mqttNetwork, *mqttSendBuffer,
    *mqttRecvBuffer, *mqttMessageHandlers
  );
}
//-----------------------------------------------------------------------------------------------------------------

void loop() {
  if (!isConnectedWiFi()) {
    connectWiFi();
    disconnectTCPMQTTBroker();
  } else if (!isConnectedTCPMQTTBroker()) {
    connectTCPMQTTBroker();
  } else if (!isConnectedServiceMQTTBroker()) {
    connectServiceMQTTBroker();
  } else {
    const char* buf = "Hello";

    MqttClient::Message message;

    message.qos = MqttClient::QOS0;
    message.retained = false;
    message.dup = false;
    message.payload = (void*) buf;
    message.payloadLen = strlen(buf) + 1;

    mqtt->publish(PUBLISH_TOPIC, message);

    // Idle for 5 seconds
    mqtt->yield(5000L);
  }
}
