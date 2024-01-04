#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <SPI.h>

const char *ssid = "YOUR SSID";
const char *password = "YOUR PW";
const char *telegramBotToken = "YOUR TOKEN";
const char *chatId = "YOUR CHAT ID";

#define TCS34725_INTEGRATIONTIME_700MS 700 // Milliseconds

#define deleteButtonPin 2
int deleteButton = HIGH; 

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);
HTTPClient http;

void connectToNetwork();
void teleSendMessage(String payload);
void deleteStoredData(); // Added function to delete stored data
bool isColorInRange(float value, float minValue, float maxValue);

float totalAmount = 0.0;  // Variable to store the total amount

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
  connectToNetwork();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Initialize the sensor
  if (!tcs.begin()) {
    Serial.println("Failed to initialize sensor");
    while (1);
  }

  // Load total amount from SPIFFS
  File file = SPIFFS.open("/total_amount.txt", "r");
  if (file) {
    totalAmount = file.parseFloat();
    file.close();
  } else {
    Serial.println("Failed to open total_amount.txt");
  }

  pinMode(deleteButtonPin, INPUT_PULLUP);  // Set the button pin as input with pull-up resistor

  delay(1000);

  // Initialize Telegram message payload
  teleSendMessage("Money detection started");
}

void loop() {
  // Read the color values
  float red, green, blue;
  tcs.getRGB(&red, &green, &blue);

  // Print the color values
  Serial.print("Red: ");
  Serial.print(red);
  Serial.print(" Green: ");
  Serial.print(green);
  Serial.print(" Blue: ");
  Serial.println(blue);

  // Check color conditions and output messages
  float previousAmount = totalAmount;  // Save the previous total amount

  // Add your logic to calculate the total amount based on color values
  if (isColorInRange(red, 90.0, 95.0) && isColorInRange(green, 96.0, 100.0) && isColorInRange(blue, 58.0, 62.0)) {
    Serial.println("Uang 1000 lama");
    totalAmount += 1000.0;
  } else if (isColorInRange(red, 91.0, 94.0) && isColorInRange(green, 96.0, 99.0) && isColorInRange(blue, 62.0, 65.0)) {
    Serial.println("Uang 1000 baru");
    totalAmount += 1000.0;
  } else if (isColorInRange(red, 86.0, 93.0) && isColorInRange(green, 96.0, 102.0) && isColorInRange(blue, 62.0, 68.0)) {
    Serial.println("Uang 2000 lama");
    totalAmount += 2000.0;
  } else if (isColorInRange(red, 80.0, 83.0) && isColorInRange(green, 96.0, 99.0) && isColorInRange(blue, 70.0, 73.0)) {
    Serial.println("Uang 2000 baru");
    totalAmount += 2000.0;
  } else if (isColorInRange(red, 94.0, 98.0) && isColorInRange(green, 96.0, 99.0) && isColorInRange(blue, 55.0, 59.0)) {
    Serial.println("Uang 5000 lama");
    totalAmount += 5000.0;
  } else if (isColorInRange(red, 97.0, 101.0) && isColorInRange(green, 92.0, 95.0) && isColorInRange(blue, 58.0, 62.0)) {
    Serial.println("Uang 5000 baru");
    totalAmount += 5000.0;
  } else if (isColorInRange(red, 88.0, 94.0) && isColorInRange(green, 83.0, 88.0) && isColorInRange(blue, 75.0, 80.0)) {
    Serial.println("Uang 10000 lama");
    totalAmount += 10000.0;
  } else if (isColorInRange(red, 82.0, 87.0) and isColorInRange(green, 92.0, 96.0) && isColorInRange(blue, 71.0, 74.0)) {
    Serial.println("Uang 10000 baru");
    totalAmount += 10000.0;
  } else if (isColorInRange(red, 74.0, 82.0) && isColorInRange(green, 101.0, 107.0) && isColorInRange(blue, 64.0, 79.0)) {
    Serial.println("Uang 20000 lama");
    totalAmount += 20000.0;
  } else if (isColorInRange(red, 74.0, 82.0) && isColorInRange(green, 101.0, 107.0) && isColorInRange(blue, 64.0, 79.0)) {
    Serial.println("Uang 20000 baru");
    totalAmount += 20000.0;
  } else if (isColorInRange(red, 70.0, 74.0) && isColorInRange(green, 96.0, 99.0) && isColorInRange(blue, 75.0, 83.0)) {
    Serial.println("Uang 50000 lama");
    totalAmount += 50000.0;
  } else if (isColorInRange(red, 70.0, 74.0) && isColorInRange(green, 98.0, 101.0) && isColorInRange(blue, 77.0, 80.0)) {
    Serial.println("Uang 50000 baru");
    totalAmount += 50000.0;
  } else if (isColorInRange(red, 96.0, 101.0) && isColorInRange(green, 88.0, 94.0) && isColorInRange(blue, 60.0, 64.0)) {
    Serial.println("Uang 100000 lama");
    totalAmount += 100000.0;
  } else if (isColorInRange(red, 95.0, 98.0) && isColorInRange(green, 88.0, 91.0) && isColorInRange(blue, 63.0, 67.0)) {
    Serial.println("Uang 100000 baru");
    totalAmount += 100000.0;
  }

  // Check if the total amount has changed
  if (totalAmount != previousAmount) {
    // Save the total amount to SPIFFS
    File file = SPIFFS.open("/total_amount.txt", "w");
    if (file) {
      file.print(totalAmount);
      file.close();
    }

    // Construct Telegram message payload
    String payload = "{";
    payload += "\"chat_id\":\"" + String(chatId) + "\",";
    
    if (totalAmount > 0.0) {
      // Convert totalAmount to a string
      String totalAmountStr = String(totalAmount, 2);  // Adjust the precision as needed
      payload += "\"text\":\"Total amount: " + totalAmountStr + "\"";
    } else {
      payload += "\"text\":\"Total amount not available\"";
    }

    payload += "}";

    // Send message to Telegram
    teleSendMessage(payload);
  }

  deleteButton = digitalRead(deleteButtonPin);

  if (deleteButton == LOW) {
    deleteStoredData();
    delay(5000);  // Delay to avoid rapid button presses
  }

  delay(1000); // Delay for detection
}

void connectToNetwork() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to network");
}

void teleSendMessage(String payload) {
  http.begin("https://api.telegram.org/bot" + String(telegramBotToken) + "/sendMessage");
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}

void deleteStoredData() {
  // Connect to Wi-Fi
  connectToNetwork();

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount file system");
    return;
  }

  // Delete the total_amount.txt file
  if (SPIFFS.remove("/total_amount.txt")) {
    Serial.println("File deleted successfully");
    totalAmount = 0.0;  // Reset the total amount to 0
  } else {
    Serial.println("Error deleting file");
  }
}

bool isColorInRange(float value, float minValue, float maxValue) {
  return (value >= minValue && value <= maxValue);
}
