#include <SPI.h>
#include <RadioLib.h>

// Define the pins used by the transceiver module
#define NSS 5
#define RESET 14
#define DIO1 2
#define BUSY 12

// Create a new instance of the SX1268 class
SX1262 lora = new Module(5, 2, 14, 12);

int counter = 0;

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  while (!Serial);
  Serial.println("LoRa Sender");

  // Initialize LoRa module
  int state = lora.begin(434.0, 125.0, 9, 7, 0x12, 10, 8, 0, false); 
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("LoRa Initializing OK!");
  } else {
    Serial.print("LoRa Initialization Failed, code ");
    Serial.println(state);
    while (true);
  }

  // Set sync word (optional)
  lora.setSyncWord(0xF3);
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // Send LoRa packet to receiver
  int state = lora.transmit("hello " + String(counter));

  // Check for transmission errors
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Transmission successful!");
  } else {
    Serial.print("Transmission failed, code ");
    Serial.println(state);
  }

  counter++;
  delay(10000);
}