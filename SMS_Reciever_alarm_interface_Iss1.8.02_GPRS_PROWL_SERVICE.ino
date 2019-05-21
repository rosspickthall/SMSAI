/*
  Sketch: GPRS HTTP Test

*/
#include <gprs.h>
#include <SoftwareSerial.h>


char buffer[0];
//char http_cmd[2000] ;
GPRS gprs;


byte lastAState = 2;
byte lastBState = 2;
//byte lastCState = 2;
//byte lastDState = 2;

//Software Version Number//
String ver = "1.8.02 (GPRS PROWL Only Service) - 06/05/2019";

//char clientData[] = "TEST";

void setup() {

  pinMode (2, INPUT); //Digi ins
  pinMode (3, INPUT); //Digi ins
  pinMode (5, INPUT); //Digi ins
  pinMode (6, INPUT); //Digi ins
  Serial.begin(9600);
  while (!Serial);
  delay(100);
  delay(2000);
  Serial.print(ver);
  delay(100);
  gprs.preInit();
}

void loop() {
  alarmStuff();

  delay(5000);
}


void alarmStuff() {
  Serial.println("CHECKING");
  byte temp = 0;


  // tries = 0; // Retries for ethernet conn
  //Alarm Pin
  temp = digitalRead(2);
  if (temp != lastAState) {
 lastAState = temp;
    if (temp == LOW) {

     sendData("GET /example.php?API=05de2f8241285a5e4fa515a90ae00e2d7d4d4179&eventtitle=Alert&desc=Front%20Door%20Open&systitle=ALARM%20SYSTEM HTTP/1.0\r\nHost: pickthall.000webhostapp.com\r\n\r\n");
    // sendData();
      Serial.println("DONE");
    } else {
     sendData("GET /example.php?API=05de2f8241285a5e4fa515a90ae00e2d7d4d4179&eventtitle=Alert&desc=Front%20Door%20Closed&systitle=ALARM%20SYSTEM HTTP/1.0\r\nHost: pickthall.000webhostapp.com\r\n\r\n");
      
    }
   
  }

  temp = digitalRead(3);
  if (temp != lastBState) {
lastBState = temp;
    if (temp == LOW) {
    //  sendData();
   sendData("GET /example.php?API=05de2f8241285a5e4fa515a90ae00e2d7d4d4179&eventtitle=Alert&desc=Back%20Door%20Open&systitle=ALARM%20SYSTEM HTTP/1.0\r\nHost: pickthall.000webhostapp.com\r\n\r\n");
    } else {
    sendData("GET /example.php?API=05de2f8241285a5e4fa515a90ae00e2d7d4d4179&eventtitle=Alert&desc=Back%20Door%20Closed&systitle=ALARM%20SYSTEM HTTP/1.0\r\nHost: pickthall.000webhostapp.com\r\n\r\n");
    }
    
  }

  delay(500);

  /*
    //  tries = 0; // Retries for ethernet conn
    //  delay(100);
    //Alarm Mains Power Status
    temp = digitalRead(3);
    if (temp != lastBState) {
    if (temp == LOW) {

      char data = "Back%20Door%20Open";
      Serial.println(s2Status);
      sendData(data);
    } else {

      char data = "Back%20Door%20Closed";
      Serial.println(s2Status);
      sendData(data);
    }
    lastBState = temp;
    }

    tries = 0; // Retries for ethernet conn
    //  delay(100);
    //Fire Status
    temp = digitalRead(5);
    if (temp != lastCState) {
    if (temp == LOW) {
      s3Status = F("Fire Activated. ");
      String data = "Fire%20Activated";
      sendData(s3Status, data);
    } else {
      s3Status = F("Fire healthy. ");
      String data = "Fire%20Healthy";
      sendData(s3Status, data);
    }
    lastCState = temp;
    }
    tries = 0; // Retries for ethernet conn
    //  delay(100);
    //Set Status
    temp = digitalRead(6);
    if (temp != lastDState) {
    if (temp == LOW) {
      s4Status = F("System Set (Armed). ");
      String data = "System%20Armed";
      sendData(s4Status, data);
    } else {
      s4Status = F("System Unset (Disarmed). ");
      String data = "System%20Disarmed";
      sendData(s4Status, data);
    }
    lastDState = temp;
    }
    tries = 0;     // Retries for ethernet conn
    //  delay(100);
  */
  /*  //Aux Mains Power Status
    temp = digitalRead(6);
    if (temp != lastEState) {
      if (temp == LOW) {
        s5Status = F("Auxillary Mains Power Fail. ");
        sendMessage(s5Status);
      } else {
        s5Status = F("Auxillary Mains Power Healthy. ");
        sendMessage(s5Status);
      }
      lastEState = temp;
    }
  */

  /*
    Serial.println(s1Status);
    Serial.println(s2Status);
    Serial.println(s3Status);
    Serial.println(s4Status);
    Serial.println(s5Status);

    Serial.println(s1Status + s2Status + s3Status + s4Status + s5Status);

    delay(1000);
  */
}


void sendData(char http_cmd[1024]) {

  Serial.println("SENDING DATA");
  gprs.init();
  gprs.join("hologram");


  Serial.println(http_cmd);

  //GET /example.php?API=05de2f8241285a5e4fa515a90ae00e2d7d4d4179&eventtitle=Alert&desc=TEST&systitle=ALARM%20SYSTEM HTTP/1.1\r\nHost: pickthall.000webhostapp.com\r\n\r\n

  //  Serial.println("GPRS - HTTP Connection Test...");

  delay(500);
  while (0 != gprs.init()) {
    delay(1000);
    Serial.println("init error");
  }

  while (0 == gprs.join("hologram, , ")) { //change "cmnet" to your own APN
    Serial.println("gprs join network error");

    delay(2000);
  }
  // successful DHCP
  Serial.print("IP Address is ");
  Serial.println(gprs.getIPAddress());

  Serial.println("Init success, start to connect server...");

  if (0 == gprs.connectTCP("pickthall.000webhostapp.com", 80)) {
    Serial.println("connect server success");
  } else {
    Serial.println("connect error");
    while (1);
  }


 gprs.sendTCPData(http_cmd);

 
  
  delay(500);
Serial.println("Movingf on");

  ///  Serial.println("close");

}
