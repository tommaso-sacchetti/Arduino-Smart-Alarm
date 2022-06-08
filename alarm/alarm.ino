#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <SimpleDHT.h>
#include "pitches.h"

SoftwareSerial SerialEsp8266(11, 10); //RX, TX

String SSID = "TechF0AA";
String PASS = "GT2TRSXR3GCQPT";

int node_ID = 1;
String my_IP = "";

// COMMANDS
String setTimeCommand = "SET_TIME";
String setAlarmCommand = "SET_ALARM";
String setSongCommand = "SET_SONG";

// Setting components
// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(12, 3, 4, 5, 6, 7);
volatile int second, minute = 0, hour = 0;
int hourButton = 12;
int minutesButton = 13;
int stopButton = 2;
int pinDHT = 8;
int pinBuzzer = 9;
bool stop = false;

volatile int alarmHour, alarmMinute, alarmSecond;
bool alarmActive = false;
bool alarmEnabled = false;

int songSelected = 1;

SimpleDHT11 dht11;


// SONGS SETUP ---------------------------------------- //

// DEATH STAR THEME

const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;


//A-AH TAKE ON ME

// The melody array 
int melody[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

// The note duration, 8 = 8th note, 4 = quarter note, etc.
int durations[] = {
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8, 
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8
};
// determine the length of the arrays to use in the loop iteration
int songLength = sizeof(melody)/sizeof(melody[0]);


void ring(int song) {
    delay(1000);
    if(song == 0) {
        // Credits to GeneralSpud https://create.arduino.cc/projecthub/GeneralSpud/passive-buzzer-song-take-on-me-by-a-ha-0f04a8
         for (int thisNote = 0; thisNote < songLength; thisNote++){
            int duration = 1000/ durations[thisNote];
            tone(pinBuzzer, melody[thisNote], duration);
            int pause = duration * 1.3;
            delay(pause);
            noTone(pinBuzzer);
        }
    } else if(song == 1) {
        deathStar();
    }
}

void deathStar() {
    //Credits to https://youtu.be/83xHpj3PstI
    beep(a, 500);
    beep(a, 500);    
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);  
    beep(a, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);

    delay(500);

    beep(eH, 500);
    beep(eH, 500);
    beep(eH, 500);  
    beep(fH, 350);
    beep(cH, 150);
    beep(gS, 500);
    beep(f, 350);
    beep(cH, 150);
    beep(a, 650);

    delay(500);
}

void beep(int note, int duration) {
    //Play tone on buzzerPin
    tone(pinBuzzer, note, duration);
    delay(duration);
    //Stop tone on buzzerPin
    noTone(pinBuzzer);

    delay(50);

}

//-------------------------------------------------------//

/**
void buttons() {
    lcd.clear();
    stop = true;
    Serial.println("BUTTON PRESSED");
}
**/

boolean initializeEsp8266() {
    do {
        // Reset the module
        Serial.print("Restarting the Esp8266 module");
        SerialEsp8266.println("AT+RST");
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("ready")) {
            Serial.println("...Module is ready");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    do {
        // STATION MODE configuration
        Serial.println("Configuring mode 1");
        SerialEsp8266.println("AT+CWMODE_DEF=1");
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("STATION MODE enabled");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    do {
        // DHCP enabling
        Serial.print("Enabling DHCP");
        SerialEsp8266.println("AT+CWDHCP_DEF=1,1");
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("DHCP enabled");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    do {
        // The router is connected
        Serial.println("Connecting to the router with SSID: " + SSID + " with password: " + PASS);
        String cmd = "AT+CWJAP_CUR=\"";
        cmd += SSID;
        cmd += "\",\"";
        cmd += PASS;
        cmd += "\"";
        SerialEsp8266.println(cmd);
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("Connected to: " + SSID);
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    // The assigned IP is obtained
    Serial.println("Obtaining the IP");
    SerialEsp8266.println("AT+CIFSR");
    do {
        delay(100);
    } while(!SerialEsp8266.available());

    if(SerialEsp8266.find("\"")) {
        my_IP = SerialEsp8266.readStringUntil('\"');
        Serial.print("IP assigned: ");
        Serial.println(my_IP);
    }
}

boolean configureServerTCP(int port) {
    do {
        // Accept multiple connections
        Serial.println("Establishing multiple connections");
        SerialEsp8266.println("AT+CIPMUX=1");
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("establishing");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    do {
        // Configure as a TCP server in listening in "port"
        Serial.println("Configuring server TCP in port: " + String(port));
        String cmd = "AT+CIPSERVER=1,";
        cmd += String(port);
        SerialEsp8266.println(cmd);
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("Configuring");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    do {
        // Configuring the timeout
        Serial.println("Configuring the server timeout...");
        String cmd = "AT+CIPSTO=15";
        SerialEsp8266.println(cmd);
        do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find("OK")) {
            Serial.println("Configuring");
            break;
        } else {
            Serial.println("...ERROR");
        };
    } while(true);

    Serial.println("Waiting for new connections...");
}

boolean sendMessage(String mess, int connection_ID) {
    boolean result = false;
    String message = mess;
    String cmd = "AT+CIPSEND=";
    cmd += String(connection_ID);
    cmd += ",";
    cmd += String(message.length() + 2);
    Serial.print("Sending message");
    SerialEsp8266.println(cmd);
    do {
            delay(100);
        } while(!SerialEsp8266.available());

        if(SerialEsp8266.find(">")) {
            // The message is sent
            SerialEsp8266.println(message);
            do {
            delay(100);
            } while(!SerialEsp8266.available());

            if(SerialEsp8266.find("OK")) {
                Serial.println("...message sent: " + message);
                result = true;
            } else {
                Serial.println("...ERROR in sending the message: " + message);
            };

        } else {
            Serial.println("...ERROR didn't receive >");
        };

        return result;
}

void shutConnection(int connection_ID) {
    String cmd = "AT+CIPCLOSE";
    cmd += "=";
    cmd += String(connection_ID);
    Serial.println("shutting the connection");
    SerialEsp8266.println(cmd);

    do {
            delay(100);
    } while(!SerialEsp8266.available());

    if(SerialEsp8266.find("OK")) {
        Serial.println("Connection closed");
    } else {
        Serial.println("...ERROR connection might be closed");
    };

    SerialEsp8266.readStringUntil('\n');
}

void printLcd(String top, String bottom) {
    lcd.setCursor(0,0);
    lcd.print(top);
    lcd.setCursor(0,1);
    lcd.print(bottom);
}

void time() {
  delay(1000);
    if(hour < 10) {
        lcd.print("0");
        lcd.print(hour);
    } 
    else { lcd.print(hour); }

    lcd.print(":");

    if(minute < 10) {
        lcd.print("0");
        lcd.print(minute);
    } 
    else { lcd.print(minute); }

    lcd.print(":");

    if(second < 10) {
        lcd.print("0");
        lcd.print(second);
    } 
    else { lcd.print(second); }

    lcd.print(" ");

    second++;
    if(second > 59) {
        minute++;
        second = 0;
    }

    if(minute > 59) {
        hour++;
        minute = 0;
    }
    if (hour > 23) {
        hour = 0;
    }
    
}

void connect(){
    unsigned long startTime = millis();

    if(SerialEsp8266.available() > 0) { // See if some messages arrived
        if(SerialEsp8266.find("+IPD,")) { // See if server received data
            // +IPD,<ID>,<len>[,<remoteIP>,<remote port>]:<data>

            String S = SerialEsp8266.readStringUntil('\r');
            char c;
            String value = "";
            //Serial.println("S: " + S);
            
            int pos_sig_comma = S.indexOf(",");
            String s_connection_ID = S.substring(0, pos_sig_comma);
            int connection_ID = s_connection_ID.toInt();

            int pos_colomn = S.indexOf(":", pos_sig_comma);
            String command = S.substring(pos_colomn + 1);

            Serial.println("Command: " + command);

            int pos_equals = command.indexOf("=");
            if(pos_equals != -1) {
                value = command.substring(pos_equals + 1);
                command = command.substring(0, pos_equals);
            }

            if (command == setTimeCommand) {
                String hourString, minuteString, secondString;
                int i = value.indexOf(":");
                hourString = value.substring(0, i);
                int j = value.lastIndexOf(":");
                secondString = value.substring(j + 1);
                minuteString = value.substring(0, j).substring(i + 1);

                hour = hourString.toInt();
                minute = minuteString.toInt();
                second = secondString.toInt();

                Serial.println("Time received: " + hourString + ":" + minuteString + ":" + secondString);

                // Serial.println("Time received: " + String(hour) + ":" + String(minute) + ":" + String(second));
            }
            else if(command == setAlarmCommand) {
                if(value.equals("disable")) {
                    alarmEnabled = false;
                    Serial.println("* Alarm disabled");
                }
                else {
                    alarmEnabled = true;
                    Serial.println("* Alarm enabled");
                    String hourString, minuteString, secondString;
                    int i = value.indexOf(":");
                    hourString = value.substring(0, i);
                    int j = value.lastIndexOf(":");
                    secondString = value.substring(j + 1);
                    minuteString = value.substring(0, j).substring(i + 1);

                    alarmHour = hourString.toInt();
                    alarmMinute = minuteString.toInt();
                    alarmSecond = secondString.toInt();

                    // Serial.println("STRING: " + hourString + " " + minuteString);

                    Serial.println("* Alarm is set at: " + (String)alarmHour + ":" + (String)alarmMinute);

                    // printLcd("ALARM SET:", "AT: " + hourString + ":" + minuteString + ":" + secondString);
                }
            }
            else if(command == setSongCommand) {
                printLcd("MODIFY SONG", "");
                songSelected = value.toInt();
                Serial.println("* Song modified to: " + (String)songSelected);
            }
            sendMessage("OK", connection_ID);
            shutConnection(connection_ID);

            Serial.println("\n");
        }
    }

    // FIXING THE TIME THE ARDUINO WAS IN SETUP MODE
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    elapsedTime = elapsedTime / 1000;
    
    calculateElapsedTime(elapsedTime);

}

void calculateElapsedTime(unsigned long millis) {
    unsigned long elapsedTime = millis;
    elapsedTime = elapsedTime / 1000;
    
    if(elapsedTime / 60 > 0) {
        if (elapsedTime / 3600 > 0) {
            hour += (int)floor(elapsedTime / 3600);
            elapsedTime = elapsedTime - (3600 * (elapsedTime % 3600));
        }

        minute += (int)floor(elapsedTime / 60);
        elapsedTime = elapsedTime - (60 * (elapsedTime % 60));
    }
    second += elapsedTime;
}

bool checkAlarm() {
    if(!alarmEnabled) {
        return false;
    }
    else if(alarmHour == hour && alarmMinute == minute && second == 3) {
            Serial.println("****ALARM ACTIVE****");
            return true;
    }
    return false;
    
}

// Initial configuration
void setup() {
    lcd.begin(16,2);
    printLcd("Tommaso Sacchetti", "Wi-Fi syn Alarm");
    delay(3000);
    lcd.clear();

    //pinMode(hourButton, INPUT_PULLUP);
    //pinMode(minutesButton, INPUT_PULLUP);
    pinMode(stopButton, INPUT_PULLUP);
    
    //attachInterrupt(digitalPinToInterrupt(stopButton), buttons, FALLING);

    // To connect to esp8266 module
    SerialEsp8266.begin(9600);
    
    Serial.begin(9600);
    initializeEsp8266();
    configureServerTCP(80);
}

// Excecution cycle
void loop() {
    connect();

    byte temperature = 0;
    byte humidity = 0;
    byte data[40] = {0};
    if(dht11.read(pinDHT, &temperature, &humidity, data)) {
        // printLcd("Read DHT11 failed", "");
        return;
    }

    printLcd("Temp:" + String((int)temperature) + " Hum.:" + String((int)humidity) + "%", "");

    //Serial.print((int)temperature); Serial.print(" *C, ");
    //Serial.print((int)humidity); Serial.println(" %");

    time();
    alarmActive = checkAlarm();

    // RINGING THE ALARM
    unsigned long startTime = millis();

    while(alarmActive && !stop) {
        lcd.clear();
        printLcd("WAKE UP!", "YOU LAZY GUY");
        ring(songSelected);
        lcd.clear();
        if(digitalRead(pinBuzzer) == LOW) {
            Serial.println("BUTTON PRESSED");
            stop = true;
        }
    }

    stop = false;
    alarmActive = false;

    // FIXING THE TIME THE ARDUINO WAS IN ALARM MODE
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    calculateElapsedTime(elapsedTime);
}
