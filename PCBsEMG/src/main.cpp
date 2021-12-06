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
bool operationMode = 0;
uint smallLoopDelay = 100;
int E1, E2, E3, E4, E5, E6;
bool wasConnected = false;
// BLEAddress BDA;

int pin1 = G35;
int pin2 = G36;
int pin3 = G26;
int pin4 = G34;
int pin5 = G12;
int pin6 = G13;

#include <ESP32AnalogRead.h>
ESP32AnalogRead adc;
ESP32AnalogRead adc2;
ESP32AnalogRead adc3;

void pinSetup() {
    pinMode(G35, INPUT);
    pinMode(G36, INPUT);
    pinMode(G26, INPUT);
    pinMode(G34, INPUT);
    pinMode(G12, INPUT);
    pinMode(G13, INPUT);

    pinMode(GPIO_NUM_35, INPUT);
    pinMode(GPIO_NUM_36, INPUT);
    pinMode(GPIO_NUM_26, INPUT);
    pinMode(GPIO_NUM_34, INPUT);
    pinMode(GPIO_NUM_12, INPUT);
    pinMode(GPIO_NUM_13, INPUT);

    gpio_pulldown_dis(GPIO_NUM_34);
    gpio_pullup_dis(  GPIO_NUM_34);
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
void applyMode(bool mode) {
    operationMode = mode;
    if (!mode) {
        M5.Lcd.clear(BLACK);
        M5.Lcd.fillCircle(160, 120, 10, RED);
    }
    if (mode) {
        M5.Lcd.clear(BLACK);
        M5.Lcd.fillCircle(160, 120, 20, GREEN);
    }
}

float rng(int maxValue) {
    return maxValue * random(100) / 100;
}

void drawBattery() {
    int batt = M5.Power.getBatteryLevel();
    M5.Lcd.drawRect(0, 0, (batt*160)/100, 2, GREEN);
}

void updateData() {
    E1 = analogRead(pin1);
    E2 = analogRead(pin2);
    E3 = analogRead(pin3);
    // E4 = analogRead(pin4);
    // E5 = analogRead(pin5);
    // E6 = analogRead(pin6);
    E4 = adc.readVoltage();
    E5 = adc2.readVoltage();
    E6 = adc3.readVoltage();
}

void oldVisualizeData() {
    M5.Lcd.fillCircle(160, 120, (E1*120)/4096, YELLOW);
}

int hE1, hE2, hE3, hE4, hE5, hE6;

void visualizeData() {
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
    M5.Lcd.fillRect(120, 220-hE4, 30, hE4, GREEN);
    M5.Lcd.fillRect(120, 220-hE5, 30, hE5, GREEN);
    M5.Lcd.fillRect(120, 220-hE6, 30, hE6, GREEN);
}

void setup() {
    pinSetup();

    M5.begin(true, false, false, false);
    M5.Power.begin();

    adc.attach(G34);
    adc2.attach(G12);
    adc3.attach(G13);

    BLEDevice::init("M5Fire");

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ConnectionOverseer());

    BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID), 30, 0);
    // BLEService *pService = pServer->createService(SERVICE_UUID;

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

    M5.Lcd.fillCircle(160, 120, 5, BLUE);
    BLEAddress BDA = BLEDevice::getAddress();
    const char *sBDA = BDA.toString().c_str();
    M5.Lcd.drawString(sBDA, 5, 5);

    drawBattery();
    M5.Lcd.setBrightness(20);
}

void loop() {
    // before first connection
    if (!connection && !wasConnected) {
        // M5.Rtc.GetBm8563Time();
        // M5.Lcd.setCursor(0, 30, 2);
        // M5.Lcd.printf("%02d : %02d : %02d\n", M5.Rtc.Hour, M5.Rtc.Minute, M5.Rtc.Second);
        delay(1000);
    }

    // initiating connection
    if (connection && !wasConnected) {
        wasConnected = true;
        delay(1000);
        applyMode(true);
    }

    // during connection
    if (connection) {
        if (!operationMode) {
            // 이름 말고, 0999 1999 2999 3999 이렇게 하면 PC쪽에서 더 편할듯
            updateData();
            pE1->setValue("E1");  pE1->notify();
            pE2->setValue("E2");  pE2->notify();
            pE3->setValue("E3");  pE3->notify();
            pE4->setValue("E4");  pE4->notify();
            pE5->setValue("E5");  pE5->notify();
            pE6->setValue("E6");  pE6->notify();
            delay(1);
        }
        else if (operationMode) {
            updateData();
            visualizeData();
            pE1->setValue(E1);    pE1->notify();
            pE2->setValue(E2);    pE2->notify();
            pE3->setValue(E3);    pE3->notify();
            pE4->setValue(E4);    pE4->notify();
            pE5->setValue(E5);    pE5->notify();
            pE6->setValue(E6);    pE6->notify();
            delay(smallLoopDelay);
        }

        M5.update();
        if(M5.BtnA.wasPressed()) {
          applyMode(!operationMode);
          delay(1500);
        }
        // 근데 아두이노는 더블레지스터 오류를 어떻게 잡을까? 버튼 누른 상태로 루프 두면 돌면 그냥 되돌아가는거 아닌가? -> 딜레이, 아마?
    }

    // initiating disconnection
    if (!connection && wasConnected) {
        wasConnected = false;
        M5.Lcd.clear(BLACK);
        drawBattery();
        M5.Lcd.fillCircle(160, 120, 5, BLUE);
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising

    }
}

// 추가할것:
// 원 위치 조정 (해상도 기반, 계산 ㄱ)
// BLE-IMU 특징 최적화
// 전력관리: 
// 배터리 잔량 표시
// LCD 밝기 -> 7
// Wifi 종료
// 마이크로폰 종료
// 작업 최적화 (주파수 내려도 좋을듯)