#include "BluetoothSerial.h"
#include "wii_i2c.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// pins connected to the Nunchuk:
#define PIN_SDA 32
#define PIN_SCL 33

// ESP32 I2C port (0 or 1):
#define WII_I2C_PORT 0

// Global variable for bluetooth communication
BluetoothSerial SerialBT;

// HC-06 address
uint8_t address[6]  = {0x98, 0xDA, 0x40, 0x00, 0xF4, 0x3B};

// id & password for the HC-06
String name = "junseok";
char *pin = "1234"; 
bool connected;

// ==================== FUNCTIONS ======================
void initSerialBT(){
    SerialBT.setPin(pin); 
    SerialBT.begin("ESP32test", true);
}

void connectedtest(){
    connected = SerialBT.connect(address);
    if(connected) {
        Serial.println("Connected Succesfully!");
    } else {
      while(!SerialBT.connected(10000)) {
          Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
      }
    }
    SerialBT.connect(address);
}

void initNunchuk(){
    if (wii_i2c_init(WII_I2C_PORT, PIN_SDA, PIN_SCL) != 0) {
        Serial.printf("Error initializing nunchuk :(");
        return;
    }
    wii_i2c_request_state();
}
// ====================================================

void setup() {
    Serial.begin(115200);
    initSerialBT();
    Serial.println("The device started in master mode, make sure remote BT device is on!");
    connectedtest();
    initNunchuk();
}

void loop() {

    const unsigned char *data = wii_i2c_read_state();
    wii_i2c_request_state();
    if (! data) {
      Serial.printf("no data available :(");
    } else {
      wii_i2c_nunchuk_state state;
      wii_i2c_decode_nunchuk(data, &state);
      Serial.printf("Stick position: (%d,%d)\n", state.x, state.y);
      Serial.printf("C button is %s\n", (state.c) ? "pressed" : "not pressed");
      Serial.printf("Z button is %s\n", (state.z) ? "pressed" : "not pressed");
    }
    delay(1000);

    if(SerialBT.available())            //송신
    {
      Serial.println(SerialBT.read());  
    }
    if(Serial.available()){
      SerialBT.write(Serial.read());
    }
    delay(20);
}