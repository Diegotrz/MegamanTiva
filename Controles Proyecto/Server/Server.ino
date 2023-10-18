/*
  Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
  Ported to Arduino ESP32 by Evandro Copercini
  updated by chegewara and MoThunderz
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "noteCode.h"

// Initialize all pointers
BLEServer* pServer = NULL;                        // Pointer to the server
BLECharacteristic* pCharacteristic_1 = NULL;      // Pointer to Characteristic 1
BLECharacteristic* pCharacteristic_2 = NULL;      // Pointer to Characteristic 2
BLEDescriptor *pDescr_1;                          // Pointer to Descriptor of Characteristic 1
BLE2902 *pBLE2902_1;                              // Pointer to BLE2902 of Characteristic 1
BLE2902 *pBLE2902_2;                              // Pointer to BLE2902 of Characteristic 2

// Some variables to keep track on device connected
bool deviceConnected = false;
bool oldDeviceConnected = false;

//Control buttons
#define shootbtn 15
#define jumpbtn 2
#define rightbtn 4
#define leftbtn 5

// Pin del Buzzer
// #define buzzerpin 4
int pre;

//OST
//Menú (Password Theme)
int m1[] = {
    NOTE_FS5, NOTE_GS5, NOTE_FS5, NOTE_CS6,
    NOTE_DS6, NOTE_CS6, NOTE_FS6,
    NOTE_F6, NOTE_B5, NOTE_CS6, NOTE_DS6,
    NOTE_FS5, NOTE_GS5, NOTE_FS5, NOTE_CS6, 
    NOTE_DS6, NOTE_CS6, NOTE_FS6,
    NOTE_F6, NOTE_B5, NOTE_CS6, NOTE_DS6,
    NOTE_FS5, NOTE_GS5, NOTE_FS5,
    NOTE_F5, NOTE_FS5, NOTE_F5, 
    NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5, 
    NOTE_CS5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_FS5, NOTE_F5,
    NOTE_GS5, NOTE_AS5, NOTE_GS5, NOTE_FS5,
    NOTE_AS5, NOTE_B5, NOTE_CS6, NOTE_GS5
};
int d1[] = {
    1, 2, 15, 1,
    2, 15, 1,
    2, 15, 1, 2,
    1, 2, 15, 1,
    2, 15, 1,
    2, 15, 1, 2,
    1, 2, 15,
    1, 2, 15,
    1, 4, 4, 4, //21, 4, 4, 4
    1, 2, 1, 2, 15, 1, //24, 7, 24, 7, 1, 21,
    4, 2, 15, 1, //4, 4, 4, 32
    4, 4, 4, 1
};

//Battle Theme
int m2[] = {
    NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_C3,NOTE_C3,
    NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_D3,NOTE_C3,NOTE_C3,
    NOTE_D5,NOTE_E5,NOTE_F5,NOTE_A5,NOTE_C6,NOTE_AS5,NOTE_G5,NOTE_AS5,NOTE_A5,NOTE_D5,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_A4,NOTE_AS4,NOTE_C5,NOTE_D6,
    NOTE_C6,NOTE_AS5,NOTE_A5,NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_C5,
    NOTE_D5,NOTE_E5,NOTE_F5,NOTE_A5,NOTE_C6,NOTE_AS5,NOTE_G5,NOTE_AS5,NOTE_A5,NOTE_D5,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_C5,
    NOTE_D5,NOTE_D5,NOTE_F5,NOTE_F5,NOTE_AS5,NOTE_AS5,NOTE_DS6,NOTE_DS6,
    NOTE_D5,NOTE_D5,1,NOTE_DS5,NOTE_FS5,NOTE_FS5,1,
    NOTE_DS5,NOTE_D5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_DS5,
    NOTE_D5,NOTE_D5,1,NOTE_DS5,NOTE_FS5,NOTE_FS5,1,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_DS5,NOTE_FS5,NOTE_AS5,
    NOTE_D5,NOTE_D5,1,NOTE_DS5,NOTE_FS5,NOTE_FS5,1,
    NOTE_DS5,NOTE_D5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_DS5,
    NOTE_D5,NOTE_D5,1,NOTE_DS5,NOTE_FS5,NOTE_FS5,1,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_DS5,NOTE_FS5,NOTE_AS5,
    NOTE_D5,NOTE_E5,NOTE_F5,NOTE_A5,NOTE_C6,NOTE_AS5,NOTE_G5,NOTE_AS5,NOTE_A5,NOTE_D5,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_A4,NOTE_AS4,NOTE_C5,NOTE_D6,
    NOTE_C6,NOTE_AS5,NOTE_A5,NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_C5,
    NOTE_D5,NOTE_E5,NOTE_F5,NOTE_A5,NOTE_C6,NOTE_AS5,NOTE_G5,NOTE_AS5,NOTE_A5,NOTE_D5,
    NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,NOTE_AS4,NOTE_C5,
    NOTE_D5,NOTE_D5,NOTE_F5,NOTE_F5,NOTE_AS5,NOTE_AS5,NOTE_DS6,NOTE_DS6,
    NOTE_DS5,NOTE_D5,NOTE_CS5,NOTE_D5,
    NOTE_A5,NOTE_A5,NOTE_A5,NOTE_A5,NOTE_A5,NOTE_A5,NOTE_A5,NOTE_A5,
    NOTE_DS5,NOTE_D5,NOTE_CS5,NOTE_D5,
    NOTE_DS5,NOTE_D5,NOTE_F5,NOTE_DS5,NOTE_D5,NOTE_A4,
    NOTE_F5,NOTE_DS5,NOTE_G5,NOTE_F5,NOTE_DS5,NOTE_AS4,
    NOTE_G5,NOTE_F5,NOTE_A5,NOTE_G5,NOTE_AS5,NOTE_A5,NOTE_C6,NOTE_AS5,
    NOTE_D6,1,NOTE_D6,1,NOTE_D6,1,NOTE_D6,1,
    NOTE_F6,NOTE_DS6,NOTE_D6,1,NOTE_D6,1,NOTE_CS6
};
int d2[] = {
    8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,4,3,
    8,8,8,4,3,8,8,8,8,
    16,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,4,3,
    4,8,8,2,8,8,8,
    8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,
    8,4,8,4,8,8,8,
    8,8,8,8,8,8,8,
    8,8,16,16,8,4,8,8,8,
    8,8,8,8,8,8,8,
    8,4,8,4,8,8,8,
    8,8,8,8,8,8,8,
    8,8,16,16,8,4,8,8,8,
    8,8,8,8,8,8,8,8,4,3,
    8,8,8,4,3,8,8,8,8,
    16,16,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,4,3,
    4,8,8,2,8,8,8,
    8,8,8,8,8,8,8,8,
    2,1,2,1,
    4,4,4,4,4,4,4,4,
    2,1,2,1,
    8,8,8,8,8,4,
    4,8,8,8,5,4,
    8,8,8,8,8,8,8,8,
    4,4,4,4,4,4,4,4,
    3,3,4,4,4,4,1
};

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/
// UUIDs used in this example:
#define SERVICE_UUID          "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_1 "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_2 "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"

// Callback function that is called whenever a client is connected or disconnected
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

// Callback function that is called whenever a client is sending a WRITE to the server
class CharacteristicCallBack: public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) override {
    std::string pChar_value_stdstr = pChar -> getValue();
    String pChar_value_string = String(pChar_value_stdstr.c_str());
    int pChar_value_int = pChar_value_string.toInt();
    if(pChar_value_int == 1){
      digitalWrite(shootbtn, HIGH);
      Serial.println("El boton presionado es Shoot");
    }
    else if(pChar_value_int == 2){
      digitalWrite(jumpbtn, HIGH);
      Serial.println("El boton presionado es Jump");
    }
    else if(pChar_value_int == 3){
      digitalWrite(rightbtn, HIGH);
      Serial.println("El boton presionado es Right");
    }
    else if(pChar_value_int == 4){
      digitalWrite(leftbtn, HIGH);
      Serial.println("El boton presionado es Left");
    }
    digitalWrite(jumpbtn, LOW);
    digitalWrite(rightbtn, LOW);
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(shootbtn, OUTPUT);
  pinMode(jumpbtn, OUTPUT);
  pinMode(leftbtn, OUTPUT);
  pinMode(rightbtn, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic_1 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_1,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );  

  pCharacteristic_2 = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_2,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE
                    );  

  // add callback functions here:
  pCharacteristic_2->setCallbacks(new CharacteristicCallBack());
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");

  //Menú
  //  for(int note = 0; note<47; note++){
  //   pre=1000;
  //   int t = pre/d1[note];
  //   tone(buzzerpin, m1[note], t);
  //   int pause = t*1.3;
  //   delay(pause);
  //   noTone(buzzerpin);
  //  }
  //Battle Theme
  // for(int note = 0; note<231; note++){
  //   pre = 1000;
  //   int t = pre/d2[note];
  //   tone(PC_4, m2[note], t);
  //   int pause = t*1.3;
  //   delay(pause);
  //   noTone(PC_4);
  // }
}

void loop() {
    // The code below keeps the connection status uptodate:
    // Disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // Connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
