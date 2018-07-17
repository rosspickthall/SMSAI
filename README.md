# SMSAI
This project is an SMS Alarm interface unit which began life on an arduino UNO and used a sim800 module. I've done some development work offline and have moved onto using the ATMega1284P chip and added GPRS services to enable prowl notifications. 
Prowl notifications and SMS alerts are working but APN info and Prowl API key are hard-coded into the code, i need to work on having these changeable by SMS and stored in the EEPROM.
