#include <SoftwareSerial.h>
#include <EEPROM.h>

///////////////////////////////// VER d
#include <gprs.h>
GPRS gprs;
#define TIMEOUT 5000
//Variable to hold last line of serial output from SIM800
char currentLine[50] = "";
int currentLineIndex = 0;

//Boolean to be set to true if message notificaion was found and next
//line of serial output is the actual SMS message content
bool nextLineIsMessage = false;


//////////////////////////////////////////////////////////
//Software Version Number//
String ver = "1.8 4chNH 14/01/2018";




int x = 0;
//String content = "";
String telNumber; //holder for incoming tel number
char alertNumber1 [12] ;//1st alert number (1.7d)
char alertNumber2 [12] ;//1st alert number (1.7d)
char alertNumber3 [12] ;//1st alert number (1.7d)
int validNumbers [3] ; //flag of 1 for valid and 0 for invalid
char newPass[4];

String s1Status;
String s2Status;
String s3Status;
String s4Status;
//String s5Status;
byte lastAState = 2;
byte lastBState = 2;
byte lastCState = 2;
byte lastDState = 2;
//byte lastEState = 2;

String securityEnabled;
String password;

//char telNo;
//String telNo;

void setup()
{

  ////////////          ERASE COMMAND       /////////////////////////////
 
 //   for (int n = 0; n < 50; n++) {
//      EEPROM.write(n, 0); //this line should only be uncommented to erase programmed phone numbers

//    }
  
  ////////////////////////////////////////////////////////////////////////

  pinMode (6, OUTPUT); //watchdog pat
  //  serialSIM800.begin(9600);   // Setting the baud rate of GSM Module
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.print(F("SMS Alarm Monitor. Verison:"));
  Serial.println(ver);
  //  RecieveMessage();
  Serial.print("System Loading...");
  //  delay(20000);
  Serial.println("Starting SIM800 SMS Command Processor");
  gprs.preInit();
  delay(1000);

  while (0 != gprs.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }

  //Set SMS mode to ASCII
  if (0 != gprs.sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }

  //Start listening to New SMS Message Indications
  if (0 != gprs.sendCmdAndWaitForResp("AT+CNMI=1,2,0,0,0\r\n", "OK", TIMEOUT)) {
    ERROR("ERROR:CNMI");
    return;
  }


  if (digitalRead(7) == HIGH) {
    securityEnabled = ("FALSE");
    Serial.println(F("Security Disabled"));
  } else {
    securityEnabled = ("TRUE");
    Serial.println(F("Security Enabled"));

  }
  loadPassword();
  delay(1000);
  initialiseNumbers();
  //    serialSIM800.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  Serial.println("System Ready... ");





  //////////////////////////////////////////////////////////////////////ver d

  Serial.println(F("Init success"));



  ///////////////////////////////////////////////////////////////////////////

  sendMessage(F("System just powered on. Status messages to follow. "));

}
void loop() {
//  externalDogPat();
  serialSIM800Event(); //checks for incoming messages and starts the process for doing somethign with them!
  alarmStuff();
}

void externalDogPat() {

  digitalWrite(6, HIGH);
  delay(300);
  digitalWrite(6, LOW);
  delay(300);
}

void loadPassword() {
  Serial.println(F("Loading Password from EEPROM"));
  String tempNumber;
  tempNumber = String(EEPROM.read(30)) + String(EEPROM.read(31)) + String(EEPROM.read(32)) + String(EEPROM.read(33));
  //  tempNumber = String(EEPROM.read(30)) + String(EEPROM.read(31)) + String(EEPROM.read(32)) + String(EEPROM.read(33))
  password = tempNumber;
  Serial.print(F("Password Loaded. Current Password: "));
  Serial.println(password);

}

void initialiseNumbers() {      //Reads stored mobile numbers from EEPROM.
  String alertNumber [3] = {"0", "0", "0"};    //Holder for alert nummbers, populated from EEPROM during "initialiseNumbers" routime
  String tempNumber;
  tempNumber = String(EEPROM.read(0)) + String(EEPROM.read(1)) + String(EEPROM.read(2)) + String(EEPROM.read(3)) + String(EEPROM.read(4)) + String(EEPROM.read(5)) + String(EEPROM.read(6)) + String(EEPROM.read(7)) + String(EEPROM.read(8)) + String(EEPROM.read(9));
  alertNumber[0] = tempNumber;
  tempNumber = "0" + tempNumber;
  tempNumber.toCharArray(alertNumber1, 12);

  tempNumber = String(EEPROM.read(10)) + String(EEPROM.read(11)) + String(EEPROM.read(12)) + String(EEPROM.read(13)) + String(EEPROM.read(14)) + String(EEPROM.read(15)) + String(EEPROM.read(16)) + String(EEPROM.read(17)) + String(EEPROM.read(18)) + String(EEPROM.read(19));
  alertNumber[1] = tempNumber;
  tempNumber = "0" + tempNumber;
  tempNumber.toCharArray(alertNumber2, 12);

  tempNumber = String(EEPROM.read(20)) + String(EEPROM.read(21)) + String(EEPROM.read(22)) + String(EEPROM.read(23)) + String(EEPROM.read(24)) + String(EEPROM.read(25)) + String(EEPROM.read(26)) + String(EEPROM.read(27)) + String(EEPROM.read(28)) + String(EEPROM.read(29));
  alertNumber[2] = tempNumber;
  tempNumber = "0" + tempNumber;
  tempNumber.toCharArray(alertNumber3, 12);

  Serial.println(F("Numbers found in EEPROM:"));
  Serial.println(alertNumber1);
  Serial.println(alertNumber2);
  Serial.println(alertNumber3);

  for (int n = 0; n < 3; n++) {
    if (alertNumber[n] == "0000000000") {
      validNumbers[n] = '0';
    } else {
      validNumbers[n] = '1';
    }
  }
}

void sendMessage(String myMessage)  //used for all alarm condition messages (ie not for set / unset)
{
  char messageToSend [myMessage.length() + 1];
  myMessage.toCharArray(messageToSend, myMessage.length() + 1);

  if (validNumbers[0] == '0' ) { // helps not send messages to numbers which arent real.
    Serial.println(F("1 - invalid number"));
  } else {
    gprs.sendSMS(alertNumber1, messageToSend);
    Serial.print(F("Message: "));
    Serial.print(myMessage);
    Serial.print(F("Sent to: 1"));
    Serial.println(alertNumber1);
    //    delay(100);
  }
  if (validNumbers[1] == '0' ) { // helps not send messages to numbers which arent real.
    Serial.println(F("2 - invalid number"));
  } else {
    gprs.sendSMS(alertNumber2, messageToSend);
    Serial.print(F("Message: "));
    Serial.print(myMessage);
    Serial.print(F("Sent to: 2"));
    Serial.println(alertNumber2);
    //   delay(100);
  }
  if (validNumbers[2] == '0' ) { // helps not send messages to numbers which arent real.
    Serial.println(F("3 - invalid number"));
  } else {
    gprs.sendSMS(alertNumber2, messageToSend);
    Serial.print(F("Message: "));
    Serial.print(myMessage);
    Serial.print(F("Sent to: 3"));
    Serial.println(alertNumber3);
    //    delay(100);
  }
}

void sendInfo(String myMessage)  //uised only for set / unset message
{

  if (digitalRead(8) == LOW) {  //decide to send to all opr just one.
    for (int a = 0; a < 1; a++) { //         //this causes problems on the sime where it wont send as empty numbers are generating errors  this needs fixed
      char messageToSend [myMessage.length() + 1];
      myMessage.toCharArray(messageToSend, myMessage.length() + 1);

      if (validNumbers[0] == '0' ) { // helps not send messages to numbers which arent real.
        Serial.println(F("1 - invalid number"));
      } else {
        gprs.sendSMS(alertNumber1, messageToSend);
        Serial.print(F("Message: "));
        Serial.print(myMessage);
        Serial.print(F("Sent to: 1"));
        Serial.println(alertNumber1);
      }
    }
  } else {
    sendMessage(myMessage); //sends to all 3 numbers, where valid
  }
}

void serialSIM800Event()  //ie read incoming message
{
  if (gprs.serialSIM800.available()) {
    //   Serial.println("Message incoming");
    char lastCharRead = gprs.serialSIM800.read();

    //Read each character from serial output until \r or \n is reached (which denotes end of line)
    if (lastCharRead == '\r' || lastCharRead == '\n') {
      //     Serial.print("Message");
      //      Serial.println(myNewMessage);
      String lastLine = String(currentLine);

      //If last line read +CMT, New SMS Message Indications was received.
      //Hence, next line is the message content.
      if (lastLine.startsWith("+CMT:")) {

        Serial.println(lastLine);
        telNumber = lastLine.substring(10, 20);
        //Serial.print(lastLine.substring);
        Serial.print("Incoming telephone number: 0");  Serial.println(telNumber);
        nextLineIsMessage = true;

      } else if (lastLine.length() > 0) {

        if (nextLineIsMessage) {
          //          Serial.print("LastLine ");
          //          Serial.println(lastLine);
          //          Serial.println(telNumber);
          nextLineIsMessage = false;
          verifyIdent(telNumber, lastLine);
        }
      }

      //Clear char array for next line of read
      for ( int i = 0; i < sizeof(currentLine); ++i ) {
        currentLine[i] = (char)0;
      }
      currentLineIndex = 0;
    } else {
      currentLine[currentLineIndex++] = lastCharRead;
    }
  }
}


void verifyIdent(String telNumber, String lastLine) {
  Serial.print(F("Verifying User...\t"));
  //  Serial.println(telNumber);
  //  String tPass;
  //  int strLen = tPass.length();
  //  Serial.print(F("Password length: "));
  //  Serial.println(tPass.length());

  if (securityEnabled == "FALSE") {
    Serial.println(F("Security Disabled."));
    decodeIncoming(telNumber, lastLine);

  } else {
    //   Serial.println(F("Checking Password"));

    if (lastLine.indexOf(password) >= 0) {
      Serial.println(F("Verified User"));
      decodeIncoming(telNumber, lastLine);

    } else {
      Serial.println(F("Access Denied"));
      //      Serial.print(serialSIM800.read());
      reply(telNumber, F("Access Denied."));// Ensure you send message with PIN code at beginning or disable security in Hardware Setup.");

    }
  }

}

void decodeIncoming(String telNumber, String content) {

  int toAdd;
  Serial.print(F("Message to decode: "));
  Serial.println(content);

  if (content.indexOf("Status") > 0) {
    //  if (content == "Status") {
    Serial.println(F("Reply with current Status"));
    Serial.println(s1Status + s2Status + s3Status + s4Status);
    reply(telNumber, s1Status + s2Status + s3Status + s4Status);

  } else if (content.indexOf("Config") > 0) {
    Serial.println(F("Reply with tel Nos: "));
    String n1(alertNumber1);
    String n2(alertNumber2);
    String n3(alertNumber3);
    Serial.println("Configured telephone numbers: \n 1: 0" + n1 + "\n 2: 0" + n2 + "\n 3: 0" + n3);
    reply(telNumber, "Configured mobile #'s: \n 1: 0" + n1 + "\n 2: 0" + n2 + "\n 3: 0" + n3);

  } else if (content.indexOf("Ver") > 0) {
    Serial.println(F("Reply with security and version info: "));
    Serial.println(" \n Secuirty Enabled? " + securityEnabled + "\n SW Ver: " + ver);
    reply(telNumber, " \n Secuirty Enabled? " + securityEnabled + "\n SW Ver: " + ver);

  } else if (content.indexOf("Add1") > 0) {
    Serial.print(F("found Add1"));
    toAdd = 1;
    addNumber(telNumber, toAdd);
  } else if (content.indexOf("Add2") > 0) {
    Serial.print(F("found Add2"));
    toAdd = 2;
    addNumber(telNumber, toAdd);
  } else if (content.indexOf("Add3") > 0) {
    Serial.print(F("found Add3"));
    toAdd = 3;
    addNumber(telNumber, toAdd);
  } else if (content.indexOf("Erase") > 0) {
    for (int n = 0; n < 50; n++) {
      EEPROM.write(n, 0);
    }
    reply(telNumber, F("EEPROM and Settings Erased - No further alerts will be sent."));
  } else if (content.indexOf("CP") > 0) {

    Serial.print(F("Request to Change Password. "));
    int commaPosition = content.indexOf('!');                    //No longer use a comma as this is in the SIM data too. !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Serial.print("comma position: ");
    Serial.println(commaPosition);
    newPass[0] = content.charAt(commaPosition + 1);
    newPass[1] = content.charAt(commaPosition + 2);
    newPass[2] = content.charAt(commaPosition + 3);
    newPass[3] = content.charAt(commaPosition + 4);

    changePassword();
  } else {
    reply(telNumber, "Invalid incoming message.");
    Serial.println(F("Invalid format incoming"));
  }
}

void changePassword() {
  //Write to EEPROM:
  int tempNo [4];  //used for incoming passcode to be added temporarily
  for (int n = 0; n < 4; n++) {
    char b = newPass[n] - 48;
    tempNo[n] = b;
  }
  for (int n = 0; n < 4; n++) {
    Serial.print(tempNo[n]);
  }
  for (int n = 0; n < 4; n++) {
    //   delay(10);
    EEPROM.write(n + 30, tempNo[n]);
  }
  Serial.print(F("Data written to EEPROM: "));
  for (int m = 30; m < 34; m++) {
    Serial.print(EEPROM.read(m));
  }
  Serial.println(newPass);
  //  Serial.println("Reminder to self - this has not programmed to EEPROM! Just played back over serial!");
  loadPassword();              //read the new password from the EEPROM & load ready for use!
  loop();
}

void addNumber(String telNumber, int toAdd) {
  Serial.print(F("Request to add number in location "));
  Serial.println(toAdd);
  writeNoToEEPROM(toAdd, telNumber); //writes incoming phone number to relevant eeprom location
  reply(telNumber, "Telephone number added ok.");
}

void writeNoToEEPROM(int toAdd, String telNo) {
  //Write to EEPROM:
  int tempNo [10];  //used for incoming phone number to be added in Add Number routines //was global
  for (int n = 0; n < 10; n++) {
    char b = telNo[n] - 48;
    tempNo[n] = b;
  }
  for (int n = 0; n < 10; n++)
    Serial.print(tempNo[n]);

  if (toAdd == 1) {
    for (int n = 0; n < 10; n++) {
      //   delay(10);
      EEPROM.write(n, tempNo[n]);
    }
    Serial.print(F("Data written to EEPROM: "));
    for (int m = 0; m < 10; m++) {
      Serial.print(EEPROM.read(m));
    }
  } else if (toAdd == 2) {
    for (int n = 0; n < 10; n++) {
      //   delay(10);
      EEPROM.write(n + 10, tempNo[n]);
    }
    Serial.print(F("Data written to EEPROM: "));
    for (int m = 10; m < 20; m++) {
      Serial.print(EEPROM.read(m));
    }
  } else if (toAdd == 3) {
    for (int n = 0; n < 10; n++) {
      //   delay(10);
      EEPROM.write(n + 20, tempNo[n]);
    }
    Serial.print(F("Data written to EEPROM: "));
    for (int m = 20; m < 30; m++) {
      Serial.print(EEPROM.read(m));
    }
  }
  initialiseNumbers();
}


void reply(String telNumber, String myMessage) {
  char replyNumber [12];
  telNumber = "0" + telNumber;
  telNumber.toCharArray(replyNumber, 12);
  char messageToSend [myMessage.length() + 1];
  myMessage.toCharArray(messageToSend, myMessage.length() + 1);
  gprs.sendSMS(replyNumber, messageToSend); //define phone number and text
  Serial.print("Replied: ");
  Serial.print(messageToSend);
  Serial.print(" to: ");
  Serial.println(replyNumber);
}


void alarmStuff() {
  byte temp = 0;


  //Alarm Pin
  temp = digitalRead(2);
  if (temp != lastAState) {
    if (temp == LOW) {
      s1Status = "Alarm Activated. ";
      Serial.println(s1Status);
      sendMessage(s1Status);
    } else {
      s1Status = "Alarm Deactivated. ";
      Serial.println(s1Status);
      sendMessage(s1Status);
    }
    lastAState = temp;
  }

  //  delay(100);
  //Alarm Mains Power Status
  temp = digitalRead(3);
  if (temp != lastBState) {
    if (temp == LOW) {
      s2Status = F("Alarm Mains Fail. ");
      sendMessage(s2Status);
    } else {
      s2Status = F("Alarm Mains Healthy. ");
      sendMessage(s2Status);
    }
    lastBState = temp;
  }

  //  delay(100);
  //Fire Status
  temp = digitalRead(4);
  if (temp != lastCState) {
    if (temp == LOW) {
      s3Status = F("Fire Activated. ");
      sendMessage(s3Status);
    } else {
      s3Status = F("Fire healthy. ");
      sendMessage(s3Status);
    }
    lastCState = temp;
  }
  //  delay(100);
  //Set Status
  temp = digitalRead(5);
  if (temp != lastDState) {
    if (temp == LOW) {
      s4Status = F("System Set (Armed). ");
      sendInfo(s4Status);
    } else {
      s4Status = F("System Unset (Disarmed). ");
      sendInfo(s4Status);
    }
    lastDState = temp;
  }
  //  delay(100);
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

