/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara
   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8
   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.
   A connect handler associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <M5Stack.h>
#include <BLE2902.h>
#include <BLEAddress.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_E1_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a1"
#define CHAR_E2_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a2"
#define CHAR_E3_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a3"
#define CHAR_E4_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a4"
#define CHAR_E5_UUID "63bac001-bee5-4148-b8c7-220305bada25"
#define CHAR_E6_UUID "b2465de1-6a70-4ca8-acf6-cd1cd0fa5d46"

BLEServer*     pServer = NULL;
BLECharacteristic* pE1 = NULL;
BLECharacteristic* pE2 = NULL;
BLECharacteristic* pE3 = NULL;
BLECharacteristic* pE4 = NULL;
BLECharacteristic* pE5 = NULL;
BLECharacteristic* pE6 = NULL;
bool connection = false;
bool rngMode = 0;
uint smallLoopDelay = 10;
int E1, E2, E3, E4, E5, E6;
bool wasConnected = false;
bool prev = false;

int pin1 = G35;
int pin2 = G36;
int pin3 = G25; // 됨
int pin4 = G26;
int pin5 = G12; 
int pin6 = G13;
// int pin7 = G2
// int pin8 = G15;

#include <ESP32AnalogRead.h>
// ESP32AnalogRead adc;
// ESP32AnalogRead adc2;
// ESP32AnalogRead adc3;

void pinSetup() {
    // pinMode(G35, INPUT);
    // pinMode(G36, INPUT);
    // pinMode(G25, INPUT);
    // pinMode(G26, INPUT); // 지우기 애매함
    // pinMode(G34, INPUT);
    // pinMode(G12, INPUT);
    // pinMode(G13, INPUT);
    // pinMode(G0 , INPUT);
    // pinMode(G2 , INPUT);
    // pinMode(G15, INPUT);
    // pinMode(GPIO_NUM_35, INPUT);
    // pinMode(GPIO_NUM_36, INPUT);
    // pinMode(GPIO_NUM_25, INPUT);
    // pinMode(GPIO_NUM_26, INPUT);
    // pinMode(GPIO_NUM_34, INPUT);
    // pinMode(GPIO_NUM_12, INPUT);
    // pinMode(GPIO_NUM_13, INPUT);
    // pinMode(GPIO_NUM_0, INPUT);
    // pinMode(GPIO_NUM_2, INPUT);
    // pinMode(GPIO_NUM_15, INPUT);

    // gpio_pulldown_dis(GPIO_NUM_34);
    // gpio_pullup_dis(  GPIO_NUM_34);
    // gpio_pulldown_dis(GPIO_NUM_25);
    // gpio_pullup_dis(  GPIO_NUM_25);
    // gpio_pulldown_dis(GPIO_NUM_26);
    // gpio_pullup_dis(  GPIO_NUM_26);
    // gpio_pulldown_dis(GPIO_NUM_12);
    // gpio_pullup_dis(  GPIO_NUM_12);
    // gpio_pulldown_dis(GPIO_NUM_13);
    // gpio_pullup_dis(  GPIO_NUM_13);
    // gpio_pulldown_dis(GPIO_NUM_0);
    // gpio_pullup_dis(  GPIO_NUM_0);
    // gpio_pulldown_dis(GPIO_NUM_2);
    // gpio_pullup_dis(  GPIO_NUM_2);
    // gpio_pulldown_dis(GPIO_NUM_15);
    // gpio_pullup_dis(  GPIO_NUM_15);

    // adcAttachPin(pin1);
    // adcAttachPin(pin2);
    // adcAttachPin(pin3);
    // adcAttachPin(pin4);
    // adcAttachPin(pin5);
    // adcAttachPin(pin6);


    M5.Lcd.println();
    M5.Lcd.println("Pins set up.");

    // adc.attach(G34);
    // adc2.attach(G12);
    // adc3.attach(G13);
}


class ConnectionOverseer: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        connection = true;
        BLEDevice::startAdvertising();
    }

    void onDisconnect(BLEServer* pServer) {
        connection = false;
    }
};

// act as a single call interface for all needed changes in loop behavior
void applyMode(bool rng) {
    rngMode = rng;
    if (!rng) {
        M5.Lcd.clear(BLACK);
        M5.Lcd.fillCircle(160, 120, 20, GREEN);
    }
    if (rng) {
        M5.Lcd.clear(BLACK);
        M5.Lcd.fillCircle(160, 120, 24, GREEN);
        M5.Lcd.fillCircle(160, 120, 20, RED);
    }
    delay(1000);
}

int rng(int maxValue) {
    return maxValue * random(100) / 100;
}

void drawBattery() {
    int batt = M5.Power.getBatteryLevel();
    M5.Lcd.drawRect(0, 0, (batt*160)/100, 2, GREEN);
}

void updateAdc() {
    E1 = analogRead(pin1);
    E2 = analogRead(pin2);
    E3 = analogRead(pin3); analogRead(G26);
    E4 = analogRead(pin4);
    E5 = analogRead(pin5);
    E6 = analogRead(pin6);
    
    // ESP32AnalogRead Library - - -
    // E4 = adc.readVoltage();
    // E5 = adc2.readVoltage();
    // E6 = adc3.readVoltage();
}

void updateRng() {
    E1 = rng(4096);
    E2 = rng(4096);
    E3 = rng(4096);
    E4 = rng(4096);
    E5 = rng(4096);
    E6 = rng(4096);
}

void oldVisualizeData() {
    M5.Lcd.fillCircle(160, 120, (E1*120)/4096, YELLOW);
}

int hE1, hE2, hE3, hE4, hE5, hE6;

void visualizeData(String mode) {
    hE1 = (E1*200)/4096;
    hE2 = (E2*200)/4096;
    hE3 = (E3*200)/4096;
    hE4 = (E4*200)/4096;
    hE5 = (E5*200)/4096;
    hE6 = (E6*200)/4096;

    M5.Lcd.clear(BLACK);

    M5.Lcd.drawString("E1", 40 , 230);
    M5.Lcd.drawString("E2", 80 , 230);
    M5.Lcd.drawString("E3", 120, 230);
    M5.Lcd.drawString("E4", 160, 230);
    M5.Lcd.drawString("E5", 200, 230);
    M5.Lcd.drawString("E6", 240, 230);

    M5.Lcd.fillRect(40 , 220-hE1, 30, hE1, GREEN);
    M5.Lcd.fillRect(80 , 220-hE2, 30, hE2, GREEN);
    M5.Lcd.fillRect(120, 220-hE3, 30, hE3, GREEN);
    M5.Lcd.fillRect(160, 220-hE4, 30, hE4, GREEN);
    M5.Lcd.fillRect(200, 220-hE5, 30, hE5, GREEN);
    M5.Lcd.fillRect(240, 220-hE6, 30, hE6, GREEN);

    M5.Lcd.drawString(mode, 5, 5);
}

void pushData() {
    pE1->setValue(E1);  pE1->notify();
    pE2->setValue(E2);  pE2->notify();
    pE3->setValue(E3);  pE3->notify();
    pE4->setValue(E4);  pE4->notify();
    pE5->setValue(E5);  pE5->notify();
    pE6->setValue(E6);  pE6->notify();
}

void setup() {
    // pinSetup(); 

    // M5
    M5.begin(true, false, false, false);
    M5.Power.begin();


    // ADC
    delay(1000);
    pinSetup(); // so it doesn't intefere w/ boot


    // bluetooth
    BLEDevice::init("M5Fire");

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ConnectionOverseer());

    BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 30, 0);

    pE1 = pService->createCharacteristic(
                        CHAR_E1_UUID,
                        BLECharacteristic::PROPERTY_READ   |
                        BLECharacteristic::PROPERTY_WRITE  |
                        BLECharacteristic::PROPERTY_NOTIFY |
                        BLECharacteristic::PROPERTY_INDICATE
                    );
    pE2 = pService->createCharacteristic(
                        CHAR_E2_UUID,
                        BLECharacteristic::PROPERTY_READ    |
                        BLECharacteristic::PROPERTY_WRITE   |
                        BLECharacteristic::PROPERTY_NOTIFY  |
                        BLECharacteristic::PROPERTY_INDICATE
                    );
    pE3 = pService->createCharacteristic(
                        CHAR_E3_UUID,
                        BLECharacteristic::PROPERTY_READ    |
                        BLECharacteristic::PROPERTY_WRITE   |
                        BLECharacteristic::PROPERTY_NOTIFY  |
                        BLECharacteristic::PROPERTY_INDICATE
                    );
    pE4 = pService->createCharacteristic(
                        CHAR_E4_UUID,
                        BLECharacteristic::PROPERTY_READ    |
                        BLECharacteristic::PROPERTY_WRITE   |
                        BLECharacteristic::PROPERTY_NOTIFY  |
                        BLECharacteristic::PROPERTY_INDICATE
                    );
    pE5 = pService->createCharacteristic(
                        CHAR_E5_UUID,
                        BLECharacteristic::PROPERTY_READ    |
                        BLECharacteristic::PROPERTY_WRITE   |
                        BLECharacteristic::PROPERTY_NOTIFY  |
                        BLECharacteristic::PROPERTY_INDICATE
                    );
    pE6 = pService->createCharacteristic(
                        CHAR_E6_UUID,
                        BLECharacteristic::PROPERTY_READ    |
                        BLECharacteristic::PROPERTY_WRITE   |
                        BLECharacteristic::PROPERTY_NOTIFY  |
                        BLECharacteristic::PROPERTY_INDICATE
                    );

    pE1->addDescriptor(new BLE2902());
    pE2->addDescriptor(new BLE2902());
    pE3->addDescriptor(new BLE2902());
    pE4->addDescriptor(new BLE2902());
    pE5->addDescriptor(new BLE2902());
    pE6->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x00);  // 아이폰때문에 넣는거랬나?
    // pAdvertising->setMinPreferred(0x0);  // 아이폰때문에 넣는거랬나?
    BLEDevice::startAdvertising();


    // homescreen
    M5.Lcd.setBrightness(20);
    drawBattery();
    
    M5.Lcd.fillCircle(160, 120, 5, BLUE);

    BLEAddress BDA = BLEDevice::getAddress();
    const char *sBDA = BDA.toString().c_str();
    M5.Lcd.drawString(sBDA, 5, 5);
}

void loop() {

    // connection
    if (!connection && !wasConnected) { // before first connection
        if (prev) {
            updateAdc();
            visualizeData("ADC Mode");
            pushData();
            delay(smallLoopDelay);
        }
        else { delay(1000); }
    }
    if (connection && !wasConnected) { // initiating connection
        wasConnected = true;
        delay(1000);
        applyMode(false);
    }
    if (connection) { // during connection
        if (!rngMode) {
            updateAdc();
            visualizeData("ADC Mode");
            pushData();
            delay(smallLoopDelay);
        }
        else if (rngMode) {
            updateRng();
            visualizeData("RNG Mode");
            pushData();
            delay(smallLoopDelay);
        }

    }
    
    M5.update();
    if(M5.BtnA.wasPressed()) { applyMode(!rngMode); }
    if(M5.BtnB.wasPressed()) { prev = !prev; delay(1000); }

    // initiating disconnection
    if (!connection && wasConnected) {
        wasConnected = false;
        
        M5.Lcd.clear(BLACK);
        M5.Lcd.fillCircle(160, 120, 5, BLUE);
        drawBattery();

        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising

    }
}

// PERIPHERALS mapping spreadsheet: may need to re-find one for M5Fire specifically!