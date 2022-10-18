#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <HTTPClient.h>
//  Variables
LiquidCrystal_I2C lcd(0x27,20,4);
const int tempPin=19; //Arduino pin connected to DS18B20 sensor's DQ pin
OneWire oneWire(tempPin); //setup a oneWire instance
DallasTemperature sensors(&oneWire); //pass oneWire to DallasTemperature library
float tempCelsius; //temperature in Celsius
float tempFahrenheit; //temperature in Fahrenheit
int PulseSensorPurplePin = 36;        // Pulse Sensor PURPLE WIRE connected to ANALOG PIN 0
int LED13 = 18;   //  The on-board Arduion LED
int speakerPin = 14; // The voice recorder module
float Signal;                // holds the incoming raw data. Signal value can range from 0-1024
int Threshold = 100;            // Determine which Signal to "count as a beat", and which to ingore. 
float esp_BPM;
float pulse;
float real_pulse=0;
float ave_pulse;
const char* ssid = "TECNO Pouvoir 2";
const char* password = "ogundele";
// Domain Name with full URL Path for HTTP POST Request
const char* serverName = "http://api.thingspeak.com/update";
String msg_apiKey = "954432";              //Add your Token number that bot has sent you on WhatsApp messenger
String whatsapp_apiKey = "989508";
String phone_number = "+2349067867583"; //Add your WhatsApp app registered phone number (same number that bot send you in url)
String msg_content;
String url2;
String url;                            //url String will be used to store the final generated URL
// Service API Key
String apiKey = "RWPE6SY1MEC4NBXN";
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Set timer to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Timer set to 6 seconds (10000)
unsigned long timerDelay = 6000;
// The SetUp Function:
void setup() {
Serial.begin(115200);         // Set's up Serial Communication at certain speed.
sensors.begin(); //initialize the sensor
pinMode(speakerPin,OUTPUT);
Serial.println("Initializing..."); 
delay(1000);
 WiFi.begin(ssid, password);
 Serial.println("Connecting");
 while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
 }
 Serial.println("");
 Serial.print("Connected to WiFi network with IP Address: ");
 Serial.println(WiFi.localIP());
 Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
pinMode(LED13,OUTPUT);         // pin that will blink to your heartbeat!
lcd.begin(); // initialize the lcd 
lcd.backlight();
startup_MSG();
delay(400);
lcd.clear();
init_LOADING();
digitalWrite(speakerPin,HIGH);
}
// The Main Loop Function
void loop() {
lcd.clear();
bpm_LOADING();
for(int i=0;i<=2;i=i+1){
  Signal = analogRead(PulseSensorPurplePin);  // Read the PulseSensor's value.
                                              // Assign this value to the "Signal" variable.
  esp_BPM = (1024*Signal)/4095;
  pulse = (300*esp_BPM*0.32)/(1024);
  real_pulse = real_pulse+pulse;
  delay(300);
}
ave_pulse = real_pulse/3;
Serial.println(Signal);                    // Send the Signal value to Serial Plotter.
Serial.println(esp_BPM);                    // Send the Signal value to Serial Plotter.
Serial.println(pulse);
Serial.println(ave_pulse,0);
Serial.println("");
lcd.clear();
print_BPM();
if(ave_pulse > Threshold){                          // If the signal is above "550", then "turn-on" Arduino's on-Board LED.
  digitalWrite(LED13,HIGH);
}else {
  digitalWrite(LED13,LOW);                //  Else, the sigal must be below "550", so "turn-off" this LED.
  }
real_pulse=0;
temp_LOADING();
sensors.requestTemperatures(); //send the command to get temperatures
tempCelsius = sensors.getTempCByIndex(0); //read the temperature in Celsius
tempFahrenheit = ((tempCelsius*9)/5)+32; //convert Celsius to Fahrenheit
//Send an HTTP POST request every 6 seconds
if ((millis() - lastTime) > timerDelay) {
//Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
      Serial.println("Connected");
      HTTPClient http;
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);      
      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "api_key=" + apiKey + "&field1=" + String(tempCelsius)+ "&field2=" + String(tempFahrenheit) + "&field3=" + String(ave_pulse);       
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);
      String tempR = String(tempCelsius);
      String pulseP = String(ave_pulse);
      msg_content = "Temp: " + tempR + "  " + "BPM: " + pulseP;
      message_to_signal(msg_content);  // text message content
      message_to_whatsapp(msg_content);
      delay(500);
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      // JSON data to send with HTTP POST
      String httpRequestData = "{\"api_key\":\"" + apiKey + "\",\"field1\":\"" + String(random(40)) + "\"}";           
      // Send HTTP POST request
      int httpResponseCode = http.POST(httpRequestData);*/
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);      
      // Free resources
      http.end();
   }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
}
Serial.print("Temperature: ");
Serial.print(tempCelsius); //print the temperature in Celsius
Serial.print((char)223);
Serial.print("F");
Serial.print("   ~   "); //separator between Celsius and Fahrenheit
Serial.print(tempFahrenheit); //print the temperature in Fahrenheit
Serial.print((char)223);
Serial.println("C");
lcd.clear();
print_BPM();
print_TEMP();
real_pulse=0;
delay(2500);
}
void print_BPM(){
lcd.setCursor(0,0);
lcd.print("HeartBeat Happened !"); // If test is "true", print a message "a heartbeat happened".
lcd.setCursor(0,1);
lcd.print("BPM: "); // Print phrase "BPM: "
lcd.print(ave_pulse,0);
}
void print_TEMP(){
lcd.setCursor(0,2);
lcd.print("Temperature captured");
lcd.setCursor(0,3);
lcd.print("Temp: ");
lcd.setCursor(6,3);
lcd.print(tempCelsius,1);
lcd.setCursor(10,3);
lcd.print((char)223);
lcd.setCursor(11,3);
lcd.print("C");
lcd.setCursor(12,3);
lcd.print(" ~ "); //separator between Celsius and Fahrenheit
lcd.setCursor(15,3);
lcd.print(tempFahrenheit,0); //print the temperature in Fahrenheit
lcd.setCursor(17,3);
lcd.print((char)223);
lcd.setCursor(18,3);
lcd.print("F");
}
void startup_MSG(){
lcd.setCursor(2,0);
lcd.print("Covid-19 Health");
lcd.setCursor(2,1);
lcd.print("Monitoring System");
lcd.setCursor(0,2);
lcd.print("      done by");
lcd.setCursor(0,3);
lcd.print("Tunde,Iyanu & Magaji");
}
void init_LOADING(){
lcd.setCursor(0,1);
lcd.print("Initializing");
for(int j=1;j<2;j=j+1){
lcd.setCursor(12,1);
lcd.print(".");
delay(400);
lcd.setCursor(13,1);
lcd.print(".");
delay(400);
lcd.setCursor(14,1);
lcd.print(".");
delay(400);
}
}
void bpm_LOADING(){
lcd.setCursor(0,1);
lcd.print("   Getting BPM");
for(int j=1;j<2;j=j+1){
lcd.setCursor(14,1);
lcd.print(".");
delay(700);
lcd.setCursor(15,1);
lcd.print(".");
delay(700);
lcd.setCursor(16,1);
lcd.print(".");
delay(600);
}
}
void temp_LOADING(){
lcd.setCursor(0,2);
lcd.print("Temp readings");
for(int j=1;j<2;j=j+1){
lcd.setCursor(13,2);
lcd.print(".");
delay(700);
lcd.setCursor(14,2);
lcd.print(".");
delay(700);
lcd.setCursor(15,2);
lcd.print(".");
delay(600);
}
}
void  message_to_signal(String message)       // user define function to send meassage to Signal app
{
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + msg_apiKey + "&text=" + urlencode(message);
  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}
void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // begin the HTTPClient object with generated url
  httpCode = http.POST(url); // Finaly Post the URL with this function and it will store the http code
  if (httpCode == 200)      // Check if the responce http code is 200
  {
    Serial.println("Sent ok."); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
}
String urlencode(String str)  // Function used for encoding the url
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}
void  message_to_whatsapp(String message2)       // user define function to send meassage to WhatsApp app
{
  //adding all number, your api key, your message into one complete url
  url2 = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&apikey=" + whatsapp_apiKey + "&text=" + urlencode2(message2);
  postData2(); // calling postData to run the above-generated url once so that you will receive a message.
}
void postData2()     //userDefine function used to call api(POST data)
{
  int httpCode2;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url2);  // begin the HTTPClient object with generated url
  httpCode2 = http.POST(url2); // Finaly Post the URL with this function and it will store the http code
  if (httpCode2 == 200)      // Check if the responce http code is 200
  {
    Serial.println("Sent ok."); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
}
String urlencode2(String str)  // Function used for encoding the url
{
    String encodedString2="";
    char c;
    char code00;
    char code11;
    char code22;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString2+= '+';
      } else if (isalnum(c)){
        encodedString2+=c;
      } else{
        code11=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code11=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code00=c+'0';
        if (c > 9){
            code00=c - 10 + 'A';
        }
        code22='\0';
        encodedString2+='%';
        encodedString2+=code00;
        encodedString2+=code11;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString2;
}
