
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

#define IR_Sensor 17 /*D14 IR pin defined*/
#define getaranSensor 4
//#define LED 27      /*D27 LED Pin defined*/
int IR, IR_temp;
int counter; 
int vib_sens;
MPU6050 mpu6050(Wire);
long timer = 0;

const int trigPin = 18;
const int echoPin = 5;

const char* ssid = "gdg-wrk";
const char* password = "gdg123wrk";
const char* mqtt_server = "tampan.tech";

const char* mqttUser = "sutani_mqtt";
const char* mqttPassword = "sutanipass";
const char* mqttclientId = "musl";
const int MPU_ADDR = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;



char payload_data[1024];
// LED Pin
const int ledPin = 4;
int mot_cnt, mot_cnt_temp;


WiFiClient espClient;
PubSubClient client(espClient);
//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
/* lol */
DynamicJsonDocument doc(1024);
String input ="{\"roller\":{\"initial_position\":\"\",\"current_position\":\"0.3mm\",\"device_name\":\"\",\"time_stamp\":\"\"},\"motor\":{\"counter\":\"1\",\"total_counter\":\"\",\"device_name\":\"\",\"time_stamp\":\"\"},\"stang_wesel\":{\"burden\":\"100N\",\"device_name\":\"\",\"time_stamp\":\"\"},\"electriccurrent\":{\"tegangan\":\"100Volt\",\"time_stamp\":\"\"},\"position\":{\"latitude\":\"-0.368039\",\"longitude\":\"13.080904\",\"address\":\"\",\"daop\":\"2\",\"train_speed\":\"90km/jam\",\"name_train\":\"ArgoBromo\",\"station\":\"Jatibarang\"}}";



long duration;
float distanceCm;
float distanceInch;

int get_counterStats(){
  /* get counterstates */
  int counterstats;

  return counterstats;

}


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "v1/devices/me/telemetry") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
  
}

void setup() {
 


  Serial.begin(115200); // Starts the serial communication
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(IR_Sensor, INPUT);
  pinMode(getaranSensor, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  client.setCallback(callback);

  Wire.begin(21, 22, 100000); // sda, scl, clock speed
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  counter=0;
  IR_temp=0;
  vib_sens=1;

}



void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqttclientId, mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("v1/devices/me/telemetry");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void send_rol_cur_pos(float rol_cur_pos){
  char temp_rol_cur_pos[100];
  sprintf(temp_rol_cur_pos,"{\"rol_cur_pos(mm)\": \"%.2f\"}", rol_cur_pos);
  client.publish("v1/devices/me/telemetry", temp_rol_cur_pos);
}

void send_mot_tot_count(int total_count){
  char temp_mot_tot_count[100];
  sprintf(temp_mot_tot_count,"{\"mot_tot_count\": \"%d\"}", total_count);
  client.publish("v1/devices/me/telemetry", temp_mot_tot_count);
}
void send_tegangan(float tegangan){
  char temp_tegangan[100];
  sprintf(temp_tegangan,  "{\"tegangan(V)\": \"%.2f\"}", tegangan);
  client.publish("v1/devices/me/telemetry", temp_tegangan);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;

  /* lol */
  deserializeJson(doc, input);
  
  String output;
  serializeJson(doc, output);
  Serial.println(output);
  
  // Prints the distance in the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
  Serial.print("Distance (inch): ");
  Serial.println(distanceInch);
  
  /*IR*/
  IR = digitalRead(IR_Sensor); /*digital read function to check IR pin status*/
  Serial.println("nilai ir : ");
  Serial.println(IR);
  
  if(IR==0){
    Serial.println("lao");
  } else {
    Serial.println("tdk");
    output.toCharArray(payload_data, 1024);
    client.publish("v1/devices/me/telemetry", payload_data);
  }

  /* counter ++*/
  if(IR_temp!=IR){
    counter++;
  } 

  IR_temp=IR;

  Serial.print("Counter : ");
  Serial.println(counter);
  
  /* getaran */
  vib_sens = digitalRead(getaranSensor);
  if (vib_sens==1){
    Serial.println("bergetar");
    delay(5000);
  } else {
    Serial.println("tidak bergetar");
  }

  /* send data roller */
  send_rol_cur_pos(distanceCm*10);
  /* send data motor */
  send_mot_tot_count(counter/2);
  /* send tegangan*/
  send_tegangan(69.9);
  


  /* Accelerometeer*/
  mpu6050.update();
  if(millis() - timer > 10){
    Serial.print("angleX : ");
    Serial.print(mpu6050.getAngleX());
    Serial.print("\tangleY : ");
    Serial.print(mpu6050.getAngleY());
    Serial.print("\tangleZ : ");Serial.println(mpu6050.getAngleZ());
    Serial.println("");
    timer = millis();
  }

  delay(1000);
  
  
}


