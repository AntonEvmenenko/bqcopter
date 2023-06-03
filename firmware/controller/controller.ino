#include <SoftwareSerial.h>

#include <CircularBuffer.h>
#include <BQCopterControllerUtils.h>

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define NEED_CALIBRATION 0
#define BT_SERIAL Serial
#define DEBUG_SERIAL softwareSerial

#define AXIS_MAX 10000
#define AXIS_MIN 0
#define MIN_AXIS_DELTA 500
#define MIN_DELTA 50

#define NRF24L01_PACKAGE_SIZE 5

enum {
    CONTROL = 0
};

int LH_INV = 0, LV_INV = 0, RH_INV = 1, RV_INV = 0;

#if NEED_CALIBRATION == 1
int LH_MIN = AXIS_MAX, LV_MIN = AXIS_MAX, RH_MIN = AXIS_MAX, RV_MIN = AXIS_MAX;
int LH_MAX = AXIS_MIN, LV_MAX = AXIS_MIN, RH_MAX = AXIS_MIN, RV_MAX = AXIS_MIN;  
float RH_DELTA, RV_DELTA;
int calibration = 1;
#else
int LH_MIN, LV_MIN, RH_MIN, RV_MIN;
int LH_MAX, LV_MAX, RH_MAX, RV_MAX;
float RH_DELTA, RV_DELTA;
int calibration = 0;
#endif

RF24 radio(8, 7);
int nrf24l01Initialized = 0;

SoftwareSerial softwareSerial(2, 3); // RX, TX
CircularBuffer<char, 128> outputBTSerialBuffer;
CircularBuffer<char, 128> inputBTSerialBuffer;

/*
struct Data {
    int16_t id;
    float data[n];
    int16_t checksum;
};
sizeof(Data) = 8 + n * 8
*/

struct Data {
    byte id;
    float data;
};

void printByteAsHex(byte x) {
    byte b1 = (x >> 4) & 0x0F;
    byte b2 = x & 0x0F;
    outputBTSerialBuffer.push_back((char)((b1 < 10) ? b1 + '0' : b1 - 10 + 'A'));
    outputBTSerialBuffer.push_back((char)((b2 < 10) ? b2 + '0' : b2 - 10 + 'A'));
}

void printInt16AsHex(int16_t x) {
    printByteAsHex(((byte*)&x)[1]);
    printByteAsHex(((byte*)&x)[0]);
}

void printFloatAsHex(float x) {
    for (int i = 3; i >= 0; --i) {
        printByteAsHex(((byte*)&x)[i]);
    }
}

void sendToBT(int16_t id, int count, float first, ...) {
    printInt16AsHex(id);
    int16_t checksum = id;
    float* pointer = &first;
    unsigned i = 0;

    while (count--) {
        int16_t* tempPointer = (int16_t*)pointer;
        checksum += tempPointer[0] + tempPointer[1];
        printFloatAsHex(*(pointer++));
    }

    printInt16AsHex(checksum);
    outputBTSerialBuffer.push_back('\n');
}

void printThroughComma(int count, int first, ...) {
    int *pointer = &first;
    while(count--) {
        DEBUG_SERIAL.print(*(pointer++));
        DEBUG_SERIAL.print(' ');
    }
    DEBUG_SERIAL.println();
}

void save_config() {
    EEPROM_int16_write(0, LH_MIN);
    EEPROM_int16_write(2, LV_MIN);
    EEPROM_int16_write(4, RH_MIN);
    EEPROM_int16_write(6, RV_MIN);
    EEPROM_int16_write(8, LH_MAX);
    EEPROM_int16_write(10, LV_MAX);
    EEPROM_int16_write(12, RH_MAX);
    EEPROM_int16_write(14, RV_MAX);
    EEPROM_float_write(16, RH_DELTA);
    EEPROM_float_write(20, RV_DELTA);
}

void load_config() {
    LH_MIN = EEPROM_int16_read(0);
    LV_MIN = EEPROM_int16_read(2);
    RH_MIN = EEPROM_int16_read(4);
    RV_MIN = EEPROM_int16_read(6);
    LH_MAX = EEPROM_int16_read(8);
    LV_MAX = EEPROM_int16_read(10);
    RH_MAX = EEPROM_int16_read(12);
    RV_MAX = EEPROM_int16_read(14);
    RH_DELTA = EEPROM_float_read(16);
    RV_DELTA = EEPROM_float_read(20);
}

void setup() {
    BT_SERIAL.begin( 115200 );
    //DEBUG_SERIAL.begin( 9600 );

    //pinMode(2, INPUT_PULLUP);
    //pinMode(3, INPUT_PULLUP);
    pinMode(4, INPUT_PULLUP);
    pinMode(5, INPUT_PULLUP);
    pinMode(6, INPUT_PULLUP);
    pinMode(9, INPUT_PULLUP);
  
    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);

#if NEED_CALIBRATION == 0
    load_config();
#endif
}

int firstIteration = 1;

void loop() {
    if (calibration) {
        int LH = analogRead(A4);
        int LV = analogRead(A5);
        int RH = analogRead(A6);
        int RV = analogRead(A7);

        LH_MIN = min(LH_MIN, LH);
        LV_MIN = min(LV_MIN, LV);
        RH_MIN = min(RH_MIN, RH);
        RV_MIN = min(RV_MIN, RV);
        LH_MAX = max(LH_MAX, LH);
        LV_MAX = max(LV_MAX, LV);
        RH_MAX = max(RH_MAX, RH);
        RV_MAX = max(RV_MAX, RV); 
    
        if (LH_MAX - LH_MIN > MIN_AXIS_DELTA && LV_MAX - LV_MIN > MIN_AXIS_DELTA && RH_MAX - RH_MIN > MIN_AXIS_DELTA && RV_MAX - RV_MIN > MIN_AXIS_DELTA &&
            abs(LH - (LH_MAX + LH_MIN)/2) < MIN_DELTA && abs(LV - LV_MIN) < MIN_DELTA && abs(RH - (RH_MAX + RH_MIN)/2) < MIN_DELTA && abs(RV - (RV_MAX + RV_MIN)/2) < MIN_DELTA){
            LV_MIN += 20;

            RH_DELTA = (RH - (RH_MAX + RH_MIN) / 2.0f);
            RV_DELTA = (RV - (RV_MAX + RV_MIN) / 2.0f);

            save_config();

            calibration = 0;
        }
    } else {
        if (!nrf24l01Initialized) {
            byte addresses[][6] = {"1Node","2Node"}; 
            radio.begin();
            radio.setChannel(90);
            radio.setPALevel(RF24_PA_MAX);
            radio.setRetries(2,15);  
            radio.enableAckPayload();
            radio.enableDynamicPayloads();
            radio.openWritingPipe(addresses[1]);
            radio.openReadingPipe(1,addresses[0]);
            radio.stopListening();
            
            nrf24l01Initialized = 1;
        }

        /*
        printThroughComma(8, LH_MIN, LV_MIN, RH_MIN, RV_MIN, LH_MAX, LV_MAX, RH_MAX, RV_MAX);
        DEBUG_SERIAL.print(RH_DELTA);
        DEBUG_SERIAL.print(' ');
        DEBUG_SERIAL.println(RV_DELTA);
        */

        byte package[NRF24L01_PACKAGE_SIZE];
        int packageGotFromBT = 0;

        while (BT_SERIAL.available()) {
            char current = BT_SERIAL.read();

            if (!((current >= '0' && current <= '9') || (current >= 'A' && current <= 'F') || current == '#')) {
                continue;
            }
            
            if (current == '#') {
                if (inputBTSerialBuffer.get_size() >= 8 && (inputBTSerialBuffer.get_size() - 8) % 8 == 0) {
                    char temp[128];
                    for (unsigned i = 0; i < inputBTSerialBuffer.get_size(); ++i) {
                        temp[i] = inputBTSerialBuffer.get(i);
                    }

                    int16_t id = parseInt16(temp);
                    int16_t tempChecksum = id;

                    int n = (inputBTSerialBuffer.get_size() - 8) / 8;
                    float data[10];
                    for (unsigned i = 0; i < n; ++i) {
                        data[i] = parseFloat(temp + 4 + i * 8);
                        tempChecksum += parseInt16(temp + 4 + i * 8) + parseInt16(temp + 4 + i * 8 + 4);
                    }

                    int16_t checksum = parseInt16(temp + 4 + n * 8);

                    if (checksum == tempChecksum) {
                        // packages with only ONE data element now supported
                        if (n == 1) {
                            byte* pointer = (byte*)(data);

                            package[0] = id;
                            package[1] = pointer[0];
                            package[2] = pointer[1];
                            package[3] = pointer[2];
                            package[4] = pointer[3];

                            packageGotFromBT = 1;
                        }
                    }
                }
                inputBTSerialBuffer.clear();
            } else {
                inputBTSerialBuffer.push_back(current);
            }
        }

        byte LH = byte(range((analogRead(A4) - LH_MIN) / float(LH_MAX - LH_MIN), 0.0f, 1.0f) * 255);
        byte LV = byte(range((analogRead(A5) - LV_MIN) / float(LV_MAX - LV_MIN), 0.0f, 1.0f) * 255);
        byte RH = byte(range((analogRead(A6) - RH_MIN - RH_DELTA) / float(RH_MAX - RH_MIN), 0.0f, 1.0f) * 255);
        byte RV = byte(range((analogRead(A7) - RV_MIN - RV_DELTA) / float(RV_MAX - RV_MIN), 0.0f, 1.0f) * 255);
  
        LH = LH_INV ? (255 - LH) : LH;
        LV = LV_INV ? (255 - LV) : LV;
        RH = RH_INV ? (255 - RH) : RH;
        RV = RV_INV ? (255 - RV) : RV;

        //printThroughComma(4, LH, LV, RH, RV);

        if (!packageGotFromBT) {
            package[0] = CONTROL; // id
            package[1] = LV; // throttle
            package[2] = RH; // roll
            package[3] = RV; // pitch
            package[4] = 0; // control by camera
        }

        byte inputPackage[NRF24L01_PACKAGE_SIZE];

        if (radio.write(package, NRF24L01_PACKAGE_SIZE)) {
            if (radio.available()) {
                radio.read(inputPackage, NRF24L01_PACKAGE_SIZE);

                if (outputBTSerialBuffer.get_size() == 0 || (inputPackage[0] >= 8 && inputPackage[0] <= 13)) {
                    sendToBT(inputPackage[0], 1, *((float*)(inputPackage + 1)));
                }
            } else {
                // empty payload
            }
        }

        if (outputBTSerialBuffer.get_size() > 0) {
            BT_SERIAL.print(outputBTSerialBuffer.pop_front());
        }
    }
}

