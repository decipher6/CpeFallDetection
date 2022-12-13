#include <M5Core2.h>
#include <WiFi.h>
#include <EMailSender.h>
#include <cmath>
 
//symbolic constants for multimodal notification thresholds
//thresholds are based on the counter value for the loop
#define display_threshold 0
#define beep_threshold 1
#define vibrate_threshold 2
#define email_threshold 5

// setting up credentials for wifi
const char* ssid = "NETGEAR71";//SSID for wifi connection
const char* password = "classyearth413";//password for wifi connection
const char* ntpServer = "ae.pool.ntp.org";  //Set the connect NTP server.  
const long gmtOffset_sec = 14400; //Set time zone to GST by correcting GMT offset
const int daylightOffset_sec = 14400; //Set time zone to GST by correcting UTC offset
 
// Email setup
EMailSender emailSend("melearner9@gmail.com", "ghmuaoemvwtoasth");//sender email address + password
 
using namespace std;

//function declarations
void display();
void beep();
void vibrate();
void send_email();
bool is_pressed();
void printLocalTime();

void setup() {
  //initializing the M5 stack
  M5.begin();
  M5.IMU.Init();
  //connecting to wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {  //If the wifi connection fails.
    //loops until the device is connected to wifi    
    delay(500);//delay 0.5s
    M5.Lcd.print(".");
  }
  //printing acknowledgements after connecting to wifi
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.println("\n CONNECTED!");
  delay(500);

  //clearing the screen
  M5.Lcd.fillScreen(BLACK);
 
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  //init and get the time.
  printLocalTime();
  delay(20);

  //initialize the speaker
  M5.Spk.begin();

  //clear the screen
  M5.Lcd.fillScreen(BLACK);
}

//this is the main loop code of the module
void loop() {
  float accln_x, accln_y, accln_z;
  //initialize the counter to 0 and button status to false
  int counter = 0;
  bool bttn_pressed = false;
  M5.IMU.getAccelData(&accln_x, &accln_y, &accln_z); // Obtain accelerometer data
 
  
  if (accln_z > 0 && accln_z < 0.2){//condition for fall detection
 
    while(bttn_pressed == false){//loops while button is not pressed

      //display when counter is greater than or equal to diaplay_threshold    
      if (counter>= display_threshold){
        display();
      }
      //beep when counter is greater than or equal to beep_threshold
      if (counter>= beep_threshold){
        beep();
      }
      //vibrate when counter is greater than or equal to vibrate_threshold
      if (counter >= vibrate_threshold){
        vibrate();
      }
      //send an email when counter is equal to email_threshold
      if (counter == email_threshold){
        send_email();
        break;//end loop after email is sent
      }
 
      counter++;//increment counter by 1

      //set cursor, size and colour for long press message
      M5.lcd.setCursor(10, 160);
      M5.Lcd.setTextColor(YELLOW);
      M5.lcd.setTextSize(2);
      M5.Lcd.println("Long Press Any Button if you did not fall.");
      delay(3000);
      M5.Lcd.fillScreen(BLACK);

      //update the status of the button
      bttn_pressed = is_pressed();
    }
  }
  //print the local time
  printLocalTime();
  delay(100);
  //clear screen
  M5.Lcd.fillScreen(BLACK);
}

//this functino sends the emergency emails
void send_email(){
 
  EMailSender::EMailMessage message;
  //email contents
  message.subject = "EMERGENCY!";
  message.message = "Dear User,\n If you have received this email, it is because your friend has fallen down. Please verify their situation and report to the nearest health center.";
  
  //sending email to receiver email
  EMailSender::Response resp = emailSend.send("an3854@nyu.edu", message);
  
  //printing acknowledgement saying email was sent
  //set cursor, size and colour for acknowledgement message
  M5.lcd.fillScreen(BLACK);
  M5.lcd.setCursor(10, 90);
  M5.Lcd.setTextColor(RED);
  M5.lcd.setTextSize(3);
  M5.lcd.println("Email Sent");
  delay(4000);//delay 4s.
}
 
//this function generates the vibrations
void vibrate(){
  //start vibration
  M5.Axp.SetLDOEnable(3, true);
  delay(500);//delay 0.5s
  //end vibration
  M5.Axp.SetLDOEnable(3, false);
 
}

//this function produces the beep sound
void beep(){
  //produce sound notification
  for (int i = 0; i < 3; ++i){
    M5.Spk.DingDong();//produce system sound 3 times continuously
  }
}

//this function displays the "FALL DETECTED!" message 
void display(){
  //display fall detected 
  //set cursor, size and colour for fall detection message
  M5.lcd.setCursor(10, 90);
  M5.Lcd.setTextColor(RED);
  M5.lcd.setTextSize(3);
  M5.Lcd.println("FALL DETECTED!");
  delay(2000);//keep the message for 2 seconds
}

//this function checks and updates the status of the buttons
bool is_pressed(){
  //updates the button status
  M5.update();

  //displays appropriate message if any button was pressed
  if (M5.BtnA.wasPressed() ||M5.BtnB.wasPressed() ||M5.BtnC.wasPressed()) {
    //fill screen with white
    M5.Lcd.fillScreen(WHITE);
    //set cursor, size and colour for acknowledgement message
    M5.lcd.setCursor(12, 90);
    M5.Lcd.setTextColor(RED);
    M5.lcd.setTextSize(2);
    M5.Lcd.println("Button was pressed");
    delay(3000);//delay 3s
    //clear screen
    M5.Lcd.fillScreen(BLACK);
    return true;
  }
  return false;
 
 
}
 
//this function gets and  prints the local time
void printLocalTime(){
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {  //Return 1 when the time is successfully obtained.
    M5.Lcd.println("Failed to obtain time");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  //init and get the time.
  }
  //set cursor, size and colour for time display
  M5.lcd.setCursor(9, 90);
  M5.Lcd.setTextColor(GREEN);
  M5.lcd.setTextSize(3);
  M5.Lcd.println(&timeinfo, "%d %B %Y \n     %A \n\n     %H:%M:%S");  //Screen prints date and time.
}
