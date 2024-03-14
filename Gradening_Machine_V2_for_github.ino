#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <PubSubClient.h>
#include <CertStoreBearSSL.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>




//Wifi Credentials 
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

//defining the Firebase credintials so whenever these keywards are referd then it refers to the credentials 
#define FIREBASE_HOST "YOUR_REALTIME_DATABAE_LOCATION/URL"
#define FIREBASE_AUTH "YOUR_FIREBASE_DATABASE_SECRETS_KEY"
FirebaseData firebaseData;

//MQTT Service Address
const char* mqtt_server = "YOUR_MQTT_SERVER_URL";

//Client Autenication ClientID 
String clientId = "ESP8266";

//Object with BearSSL session 
WiFiClientSecure espclient;
//client object  create with BearSSL session object
PubSubClient client(espclient);

//Database Values
String AoT;
int AoT_HOUR;
int AoT_MIN;
String AoD;
String AoT_Status;

//Time Values
//int ON_MIN;
int OFF_MIN;
int OFF_SECOND;

//Time Server Varibales
//const char* ntpServer = "pool.ntp.org";
const long utcOffsetInSeconds = 19800;//PLEASE SET A APPROPRIATE UTC OFFSET ACCORDING TO YOUR LOCATION THIS OFFSET IS CURRENTLY SET FOR INDIA //setting Utc off set 
WiFiUDP ntpUDP;// setting Wifi UDP object
NTPClient timeClient(ntpUDP,"pool.ntp.org", utcOffsetInSeconds);

//Setting the relay pin
const int relay = 5;


void setup() { 
  //setting a baud rate so that the arudnio can communicate with the ESP
  //MAKE SURE THE SERIAL MONITOR ALSO HAS THE SAME BAUD RATE TO READ FROM THE SERIAL MONITOR 
  Serial.begin(9600);
  //setting the pin mode for the relay pin
  pinMode(relay, OUTPUT);
  //Connecting to wifi with credentials
  ConnectWifi(ssid,password);
  //Connect to Firebase 
  Firebase.begin(FIREBASE_HOST,FIREBASE_AUTH);
  //connects to the Mqtt server and subsribes to the topics
  ConnectMqttServer(clientId);
  //initalize all the values for auto on time and its status
  InitializeDataValues();
  //initialize the time for keeping a track of the time for auto an function
  InitializeTime();
  

}

void loop() {
  if(!client.connected()){
  ConnectMqttServer(clientId);
    }
  while (WiFi.status() != WL_CONNECTED) {
    ConnectWifi(ssid,password);
  }
  if( (AoT_MIN==minute()) && (AoT_HOUR==hour()) && (digitalRead(relay)==0) &&(AoT_Status=="Enabled")){
    Serial.println("The Auto On Time is Same");
    client.publish("Gardening_Machine_1","1");

  }
  if(digitalRead(relay)==1){
    if((OFF_MIN==minute())&&(OFF_SECOND==second())){
      client.publish("Gardening_Machine_1","0");
    }
  }
  //calls the client object in the loop so that the client object can process all the incoming message and keep the connection alive.
  client.loop();

}

void ConnectWifi(const char* ssid,const char* password){
  //Begin connection while it is not connected it keeps trying to connect,and once connected it proceeds
  int connection_requests = 0;
  WiFi.begin(ssid,password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    connection_requests = connection_requests+1;
    if(connection_requests==160){
      ErrorHandler();
    }
    }
    Serial.println("Connected to the WiFi network");
}


void ConnectMqttServer(String clientId){
  //CONNECTING TO MQTT CLIENT 
  //setting to insecure connection mode as the auth file is not there
  espclient.setInsecure();
  //Providing the Mqtt Server info to the client object 
  client.setServer(mqtt_server,8883);
  //Setting a callback function  to be executed when a Mqtt message is received
  client.setCallback(callback);
  //Setting the clientID so that we can send this to the server to establish a connection
  //String clientId = "ESP8266Client - MyClient";
  //providing authentication credentils to esablish a connection .... with  server Here we send a connection request
  client.connect(clientId.c_str(), "YOUR_CONNECTION_USERNAME(if u have one)", "YOUR_CONNECTION_PASSWORD(if u have one)");
  //subscribing to the topic which sends action requests to be performed
  if(!client.connected()){
  Serial.println("Unable to connect");
  ErrorHandler();
    }
  Serial.println("Sucessfully Connected to MQTT Server");
  client.subscribe("Gardening_Machine_1",0);
  client.subscribe("status_check",0);

}
void InitializeDataValues(){
  //Function to connect to firebase serivce and retrive relevent information and storing it 
  //AoT = Auto on Time 
  AoT = Firebase.get(firebaseData,"/Gardening_Machine_1/Time");
  AoT = firebaseData.stringData();
  //Setting the Auto on hour and the minute 
  AoT_MIN = Get_MIN(AoT);
  AoT_HOUR = Get_HOUR(AoT);
  //Aot Status to check if the auto turn on fetaure is enabled 
  AoT_Status = Firebase.getString(firebaseData,"/Gardening_Machine_1/Auto_On_Stats");
  AoT_Status = firebaseData.stringData();
}


void InitializeTime(){
  delay(500);
  timeClient.begin();
  //Function to retrive the time and set the time in the ESP's RTC to keep track of time
  delay(1000);
  timeClient.forceUpdate();//updating the time from  ntp
  int hr1 = hour();
  delay(1000);
  timeClient.forceUpdate();
  //for some reason NTP is not retuning correct time,sometimes so double checking
  if(hr1!=hour()){
    timeClient.forceUpdate();
    delay(500);
  }
  //set the time in the ESP by extracting the hours minutes and seconds from ntp 
  setTime(timeClient.getHours(),timeClient.getMinutes(),timeClient.getSeconds(),1,3,2024);
  Serial.println(hour());
  Serial.println(minute());
  
}

// These two functions return the Hour and minute from the raw time form 
int Get_HOUR(String AoT){
  int index = AoT.indexOf("/");
  int Hour = AoT.substring(0,index).toInt();
  return Hour;
}
int Get_MIN(String AoT){
  int index = AoT.indexOf("/");
  int MIN = AoT.substring(index+1,AoT.length()).toInt();
  return MIN;
}


void  callback(char* topic ,byte* payload,unsigned int length){
  String MqttMessage = "";
  for(int i = 0; i < length;i++){
    MqttMessage = MqttMessage +(char)payload[i];
  }

  Serial.println("Recieved Message:"+MqttMessage);
  if(MqttMessage=="1"){
    Turn_On_Relay();
  }

  else if(MqttMessage=="0"){
    Turn_OFF_Relay();
    }

  else if(MqttMessage == "11"){
    Firebase.set(firebaseData,"Gardening_Machine_1/Auto_On_Stats","Enabled");
    AoT_Status ="Enabled";
  }

  else if (MqttMessage == "00"){
    Firebase.set(firebaseData,"Gardening_Machine_1/Auto_On_Stats","Disabled");
    AoT_Status ="Disabled";
    }  

  else if (MqttMessage == "??"){
    client.publish("status_check","Online");
  }
  else if(MqttMessage == "Online"){
    Serial.println("Updated Status to mobile deivce");
  }

  else{
    AoT = MqttMessage;
    Serial.println("Setting Auto On TIme :"+AoT);
    AoT_MIN = Get_MIN(AoT);
    AoT_HOUR = Get_HOUR(AoT);
    Firebase.set(firebaseData,"Gardening_Machine_1/Time",AoT);
  }
}

void Turn_On_Relay(){
  Serial.println("Turning Relay - On ..");
  //Turining on the Realy pin to HIGH so that relay in turned on
  digitalWrite(relay,HIGH);
  //Fetching the Auto Off Duration
  AoD = Firebase.getString(firebaseData,"Gardening_Machine_1/Aot_dur");
  AoD = firebaseData.stringData();
  //Updating the the pump status(ON/OFF for pump)
  Firebase.set(firebaseData,"Gardening_Machine_1/Status","On"); 
  //Adding the fetched time to the OFF time to check when to turn off the relay
  OFF_MIN = minute()+AoD.toInt();//adding the AoD duration to the to the current minute to set the off time so that at that minute the device can be turned off
  OFF_SECOND = second();//Setting the second to turn of to be exact the time
  //if the off min is above 59 then that minute cannont exist so it cannot be turned OFF so it sets it as the next hours minute
  if(OFF_MIN > 59){
    OFF_MIN = OFF_MIN - 60;
  }}


void Turn_OFF_Relay(){
  Serial.println("Turning Relay - OFF ..");
    digitalWrite(relay,LOW);
    Firebase.set(firebaseData,"Gardening_Machine_1/Status","Off");  
}

void ErrorHandler(){
  //if any service is unble to connect then the device goes to deepsleep and then wakes back every 1 hrour to try again
  Serial.println("Going to Sleep will try again after 10 minutes");
  ESP.deepSleep(600e6);
}
