# SMSAI
This project is an SMS Alarm interface unit which began life on an arduino UNO and used a sim800 module. 

The main purpose is to take outputs from an intruder alarm, feed them into this system and send out messages to the user to alert them of the system status. Basically my own alarm communicator. 

I've done some development work offline and have moved onto using the ATMega1284P chip and added GPRS services to enable prowl notifications. 
Prowl notifications and SMS alerts are working but APN info and Prowl API key are hard-coded into the code, i need to work on having these changeable by SMS and stored in the EEPROM.

I would also like this to send the prowl notifications over Ethernet and fail over to SMS / GPRS if Ethernet is down, I've not had time to put any effort into this though. I have the hardware built for it though. 

I have a version of this running in my home and it works flawlessly and runs for months and months without resetting. 

I have uploaded some photos of some PCB boards I got made up for this to run on and a bit of an overview of the operation (although this was done before prowl functionality was added). 
