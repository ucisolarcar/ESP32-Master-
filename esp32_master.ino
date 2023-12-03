#include <SPI.h>              //Library for using SPI Communication 
#include <mcp2515.h>          //Library for using CAN Communication (https://github.com/autowp/arduino-mcp2515/)
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define I2C_SDA 35
#define I2C_SCL 34
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MCP2515 mcp2515(5);                 // SPI CS Pin 10

struct can_frame canMsg;
int decimalpoint = 3;
float datarecieved[10];
char datarecieve_category[] = {'w'};
void setup()
{
  Serial.begin(9600);                //Begins Serial Communication at 9600 baudrate
  SPI.begin();                       //Begins SPI communication
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  // Display static text
  display.println("Initialization Complete");
  display.display();
  mcp2515.reset();
  mcp2515.setBitrate(CAN_250KBPS, MCP_8MHZ); //Sets CAN at speed 500KBPS and Clock 8MHz
  mcp2515.setNormalMode();                  //Sets CAN at normal mode
  delay(1000);
  display.clearDisplay();
}
void display_information() {
  for (int i = 0; i < 1; i++) {
    display.clearDisplay();
    display.setCursor(0, 10);
    // Display static text
    display.print(datarecieve_category[i]);
    display.print(": ");
    display.println(datarecieved[i]);
    display.display();
  }

}
//breakdown floating points to a specific decimal point
int* decimal_breakdown(float value, int decimalpoint) {
  int digit_array [decimalpoint + 1];
  digit_array[0] = int(value);
  for (int i = 0; i < decimalpoint; i++) {
    value = value - int(value);
    value = value * 10;
    digit_array[i + 1] = int(value);
  }
//if (category == 'w') {
    //for (int i = 0; i < decimalpoint + 1; i++) {
      //windshieldwipercurrent[i] = digit_array[i];
    //}
  //}
}
//MCP2515 functions --------------------------------------------------------------------------------
void receiver() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) // To receive data (Poll Read)
  {

    if (canMsg.can_id == 0x0030) { //if arduino 2 sending data
      //scan which type of data is coming in
      for (int k = 0; k < sizeof(datarecieve_category) / sizeof(datarecieve_category[0]); k++) {
        if (canMsg.data[k] == datarecieve_category[k]) {
          float value  = 0;
          //do calculation
          for (int i = 1; i < decimalpoint + 2; i++) {
            value = value + canMsg.data[i] * pow(0.1, (i - 1));
          }
          //store to proper index of data array
          datarecieved[k] = value;
          Serial.print(datarecieve_category[k]);
          Serial.print(": ");
          Serial.println(datarecieved[k]);



        }
      }
    }
    if (canMsg.can_id == 0x0032) { //if arduino 1 sending data
      //int voltValue = canMsg1.data[0];
      //Serial.println(voltValue);
      //delay(200);
    }
  }
}
//MCP2515 functions --------------------------------------------------------------------------------
void loop()
{

  receiver();
  display_information();
  delay(400);
}
