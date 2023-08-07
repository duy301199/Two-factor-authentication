#include <RingBuffer.h>
#include <SoftwareSerial.h>

#include <Wire.h>

#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(0, 1);
#else
#define mySerial Serial1
#endif
const int ledGREEN = 8;
const int ledGREEN2 = 7;
const int ledRED = 4;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  // Your setup code here...
  pinMode(ledGREEN, OUTPUT);
  pinMode(ledGREEN, OUTPUT);
  pinMode(ledRED, OUTPUT);
  Serial.begin(9600);
  finger.begin(57600);
  delay(2);
  finger.verifyPassword();
  finger.getTemplateCount();
  Serial.println("Commands: face_detected");
  
}
String command;
void loop() {
   //Print options
  //Serial.println("\nPlease choose an option:");
  //Serial.println("1: Access");
  //Serial.println("2: Create new fingerprint");
  //Serial.println("3: Delete fingerprint");
  //Serial.println("4: Quit");

  // Read option
  if(Serial.available()){
    command = Serial.readStringUntil('\n');
      if(command.equals("face_detected")) {
        digitalWrite(ledGREEN, HIGH);
        delay(500);
        digitalWrite(ledGREEN, LOW);
        delay(500);
        digitalWrite(ledGREEN, HIGH);
        delay(500);
        digitalWrite(ledGREEN, LOW);
        delay(500);
        digitalWrite(ledGREEN, HIGH);
        getUserAccess();
        delay(500);
      }
    }
  
  delay(5000); // Delay for 5 seconds
}

void getUserAccess() {
  // Your get access code here...
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("ACCESSING");
      break;
    case FINGERPRINT_NOFINGER:
      return;
    case FINGERPRINT_PACKETRECIEVEERR:
      return;
    case FINGERPRINT_IMAGEFAIL:
      return;
    default:
      return;
  }

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    digitalWrite(ledGREEN2, HIGH);
    delay(500);
    digitalWrite(ledGREEN2, LOW);
    delay(500);
    digitalWrite(ledGREEN2, HIGH);
    Serial.println("ACCESSED SUCCESSFUL");
    Serial.print("Found ID #"); Serial.print(finger.fingerID);
    Serial.print(" with confidence of "); Serial.println(finger.confidence);
    delay(2000);
    digitalWrite(ledGREEN, LOW);
    digitalWrite(ledGREEN2, LOW);
    delay(1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return;
  } else if (p == FINGERPRINT_NOTFOUND) {
    digitalWrite(ledGREEN2, HIGH);
    delay(500);
    digitalWrite(ledGREEN2, LOW);
    delay(500);
    digitalWrite(ledRED, HIGH);
    delay(500);
    digitalWrite(ledRED, LOW);
    delay(500);
    digitalWrite(ledRED, HIGH);
    Serial.println("FAILED TO VERIFY");
    delay(2000);
    digitalWrite(ledRED, LOW);
    digitalWrite(ledGREEN, LOW);
    delay(1000);
    return;
  } else {
    digitalWrite(ledGREEN, LOW);
    delay(1000);
    return;
  }
}

/*void enrollNewFingerprint() {
  // Your enroll fingerprint code here...
  uint8_t id = 0;  // Change this with the ID you want to associate with the new fingerprint

  // Step 1: Get image
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to get image");
    return;
  }

  // Step 2: Convert image to feature
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image to feature");
    return;
  }

  // Step 3: Ask for same finger again and get image
  p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to get image");
    return;
  }

  // Step 4: Convert image to feature
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to convert image to feature");
    return;
  }

  // Step 5: Create model
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Failed to create model");
    return;
  }

  // Step 6: Store model
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Fingerprint successfully enrolled!");
  } else {
    Serial.println("Failed to store model");
  }

}

void deleteFingerprint() {
  // First, verify admin fingerprint
  Serial.println("Verifying admin fingerprint for deletion operation...");
  if (getFingerprintID() == 1) {  // Assuming 1 is the admin ID
    Serial.println("Admin verified. Please place the finger to delete...");
    // Your delete fingerprint code here...
    uint8_t id = 0;  // Change this with the ID of the fingerprint you want to delete

    // Deletion
    uint8_t p = finger.deleteModel(id);
    if (p == FINGERPRINT_OK) {
      Serial.println("Fingerprint successfully deleted!");
    } else {
      Serial.println("Failed to delete fingerprint");
    }
  } else {
    Serial.println("Failed to verify admin. Deletion operation cancelled.");
  }
}

int getFingerprintID() {
  // Your get fingerprint ID code here...
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  
  return finger.fingerID;
  //return -1;  // Return -1 if failed
} */
