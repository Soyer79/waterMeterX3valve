#include <SuplaDevice.h>//////https://github.com/SUPLA
#include <WiFiClientSecure.h>
#include <supla/network/esp_wifi.h>
#include <supla/storage/littlefs_config.h>
#include <supla/network/esp_web_server.h>
#include <supla/network/html/device_info.h>
#include <supla/network/html/protocol_parameters.h>
#include <supla/network/html/wifi_parameters.h>
#include <supla/network/html/custom_parameter.h>
#include <supla/network/html/custom_text_parameter.h>
#include <supla/device/supla_ca_cert.h>
#include <supla/device/notifications.h>
#include <supla/control/relay.h>
#include <PubSubClient.h>
#include <supla/clock/clock.h> //int clock1.getYear(); getMonth(); getDay(); getDayOfWeek();1 - Sunday, 2 - Monday getHour(); getMin(); getSec();
#include "HWCDC.h"
#include <supla/device/enter_cfg_mode_after_power_cycle.h>

HWCDC USBSerial;
Supla::ESPWifi wifi;
Supla::Clock clock1;
Supla::LittleFsConfig configSupla(4000);
Supla::EspWebServer suplaServer;
Supla::Html::DeviceInfo htmlDeviceInfo(&SuplaDevice);
Supla::Html::WifiParameters htmlWifi;
Supla::Html::ProtocolParameters htmlProto;

WiFiClientSecure client1;
PubSubClient client(client1);

const char PARAM1[] = "MQTT server";
const char PARAM2[] = "MQTT user";
const char PARAM3[] = "MQTT password";
const char PARAM4[] = "device ID:1";
const char PARAM5[] = "channel ID:1";
const char PARAM6[] = "night_h_start:1";
const char PARAM7[] = "night_h_end:1";
const char PARAM8[] = "deviceName in message:1";
const char PARAM9[] = "message:1";
const char PARAM10[] = "level alarm:1";
const char PARAM11[] = "level alarm night:1";
const char PARAM12[] = "period of time:1";
const char PARAM13[] = "period of time night:1";

const char PARAM14[] = "device ID:2";
const char PARAM15[] = "channel ID:2";
const char PARAM16[] = "night_h_start:2";
const char PARAM17[] = "night_h_end:2";
const char PARAM18[] = "deviceName in message:2";
const char PARAM19[] = "message:2";
const char PARAM20[] = "level alarm:2";
const char PARAM21[] = "level alarm night:2";
const char PARAM22[] = "period of time:2";
const char PARAM23[] = "period of time night:2";

const char PARAM24[] = "device ID:3";
const char PARAM25[] = "channel ID:3";
const char PARAM26[] = "night_h_start:3";
const char PARAM27[] = "night_h_end:3";
const char PARAM28[] = "deviceName in message:3";
const char PARAM29[] = "message:3";
const char PARAM30[] = "level alarm:3";
const char PARAM31[] = "level alarm night:3";
const char PARAM32[] = "period of time:3";
const char PARAM33[] = "period of time night:3";
const char PARAM34[] = "number of counters";

int counter_1 = 0;
int tempCounter_1;
int tempCounterNight_1;
boolean ResetTempCounterNight_1= 0;
int counter_2 = 0;
int tempCounter_2;
int tempCounterNight_2;
boolean ResetTempCounterNight_2= 0;
int counter_3 = 0;
int tempCounter_3;
int tempCounterNight_3;
boolean ResetTempCounterNight_3= 0;

char mqtt_server[200] ="a";
char mqtt_user[200] ="a";
char mqtt_pass[200] ="a";

int32_t dv_id_1 =1;
int32_t chann_id_1 =1;
int32_t night_h_start_1 =1;
int32_t night_h_stop_1 =1;
char dev_name_message_1[200] ="a";
char message_1[200] ="a";
int32_t level_alarm_1 = 40;
int32_t level_alarm_night_1 = 10;
int32_t time_period_1 = 40;
int32_t time_period_night_1 = 10;

int32_t dv_id_2 =1;
int32_t chann_id_2 =1;
int32_t night_h_start_2 =1;
int32_t night_h_stop_2 =1;
char dev_name_message_2[200] ="a";
char message_2[200]="a";
int32_t level_alarm_2 = 40;
int32_t level_alarm_night_2 = 10;
int32_t time_period_2 = 40;
int32_t time_period_night_2 = 10;

int32_t dv_id_3 =1;
int32_t chann_id_3 =1;
int32_t night_h_start_3 =1;
int32_t night_h_stop_3 =1;
char dev_name_message_3[200] ="a";
char message_3[200] ="a";
int32_t level_alarm_3 = 40;
int32_t level_alarm_night_3 = 10;
int32_t time_period_3 = 40;
int32_t time_period_night_3 = 10;

int32_t num_of_counter = 0;
String top1 = "xxx";
String top2 = "xxx";
String top3 = "xxx";
String tim;

boolean night_1 = 0;
boolean night_2 = 0;
boolean night_3 = 0;
boolean on_milli_start = 0;
unsigned long valveOn_millis;
unsigned long valveOff_millis;
unsigned long sek_millis;
unsigned long prev_min_millis_1;
unsigned long prev_hour_millis_1;
unsigned long prev_min_millis_2;
unsigned long prev_hour_millis_2;
unsigned long prev_min_millis_3;
unsigned long prev_hour_millis_3;
unsigned long prev_minute_millis;
unsigned long lastReconnectAttempt = 0;
Supla::Control::Relay *valveOpen = nullptr;
Supla::Control::Relay *valveClose = nullptr;

void setup() {
  USBSerial.begin(9600);
  Supla::Storage::Init();
 
  SuplaDevice.addClock(new Supla::Clock);
  auto clock1 = SuplaDevice.getClock();
  if(num_of_counter==0){
    new Supla::Html::CustomParameter(PARAM34, "number of counters(1,2,3)");
  }

  if (Supla::Storage::ConfigInstance()->getInt32(PARAM34, &num_of_counter)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM34, num_of_counter);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM34);
  }

  if(num_of_counter==1){
    inputs1();
  }
  else if(num_of_counter==2){
    inputs1();
    inputs2();
   }
  else if(num_of_counter==3){
    inputs1();
    inputs2();
    inputs3();
  }
  new Supla::Device::EnterCfgModeAfterPowerCycle(15000, 3, true);
  Supla::Notification::RegisterNotification(-1);
  SuplaDevice.setSuplaCACert(suplaCACert);
  SuplaDevice.setSupla3rdPartyCACert(supla3rdCACert);
  SuplaDevice.begin();
  paramSave();
  
  top1 = "supla/"+String(mqtt_user)+"/devices/"+String(dv_id_1)+"/channels/"+String(chann_id_1)+"/state/calculated_value";
  top2 = "supla/"+String(mqtt_user)+"/devices/"+String(dv_id_2)+"/channels/"+String(chann_id_2)+"/state/calculated_value";
  top3 = "supla/"+String(mqtt_user)+"/devices/"+String(dv_id_3)+"/channels/"+String(chann_id_3)+"/state/calculated_value";
  
  valveOpen = new Supla::Control::Relay(8, false);
  valveClose = new Supla::Control::Relay(9, false);
  
  prev_min_millis_1 = millis();
  prev_hour_millis_1 = millis();
  prev_min_millis_2 = millis();
  prev_hour_millis_2 = millis();
  prev_min_millis_3 = millis();
  prev_hour_millis_3 = millis();
  sek_millis = millis();
  lastReconnectAttempt = 0;

  delay(2000);
  
  USBSerial.print("user.............:");
  USBSerial.println(mqtt_user);
  USBSerial.print("serwer.............:");
  USBSerial.println(mqtt_server);
  USBSerial.print("password:.............");
  USBSerial.println(mqtt_pass);
  USBSerial.print("dv.............1: ");
  USBSerial.println(dv_id_1);
  USBSerial.print("channel.............1: ");
  USBSerial.println(chann_id_1);
  USBSerial.print("message:.............1: ");
  USBSerial.println(message_1);
  USBSerial.print("devName message:.............1: ");
  USBSerial.println(dev_name_message_1);
  USBSerial.print("level alarm:.............1: ");
  USBSerial.println(level_alarm_1);
  USBSerial.print("level alarm night:.............1: ");
  USBSerial.println(level_alarm_night_1);
  USBSerial.print("time period.............1: ");
  USBSerial.println(time_period_1);
  USBSerial.print("time period night.............1: ");
  USBSerial.println(time_period_night_1);
  USBSerial.println(top1);
  USBSerial.print("dv.............2: ");
  USBSerial.println(dv_id_2);
  USBSerial.print("channel.............2: ");
  USBSerial.println(chann_id_2);
  USBSerial.print("message:.............2: ");
  USBSerial.println(message_2);
  USBSerial.print("devName message:.............2: ");
  USBSerial.println(dev_name_message_2);
  USBSerial.print("level alarm:.............2: ");
  USBSerial.println(level_alarm_2);
  USBSerial.print("level alarm night:.............2: ");
  USBSerial.println(level_alarm_night_2);
  USBSerial.print("time period.............2: ");
  USBSerial.println(time_period_2);
  USBSerial.print("time period night.............2: ");
  USBSerial.println(time_period_night_2);
  USBSerial.println(top2);
  USBSerial.print("dv.............3: ");
  USBSerial.println(dv_id_3);
  USBSerial.print("channel.............3: ");
  USBSerial.println(chann_id_3);
  USBSerial.print("message:.............3: ");
  USBSerial.println(message_3);
  USBSerial.print("devName message:.............3: ");
  USBSerial.println(dev_name_message_3);
  USBSerial.print("level alarm:.............3: ");
  USBSerial.println(level_alarm_3);
  USBSerial.print("level alarm night:.............3: ");
  USBSerial.println(level_alarm_night_3);
  USBSerial.print("time period.............3: ");
  USBSerial.println(time_period_3);
  USBSerial.print("time period night.............3: ");
  USBSerial.println(time_period_night_3);
  USBSerial.println(top3);
  USBSerial.print("numofcounter.............: ");
  USBSerial.println(num_of_counter);
  if(dv_id_1 != 1){
   mqttConfig();
  }
}

void loop() {
 SuplaDevice.iterate();
  if(dv_id_1 != 1){
    valveControl();
    nightReady();
    if (!client.connected()) {
      long now = millis();
      if (now - lastReconnectAttempt > 5000) {
       lastReconnectAttempt = now;
        if (reconnect()) {
         lastReconnectAttempt = 0;
        }
      }
    } 
    else {
     client.loop();
    }

  if(num_of_counter==1){
    waterControl1();
  }
  else if(num_of_counter==2){
    waterControl1();
    waterControl2();
   }
  else if(num_of_counter==3){
    waterControl1();
    waterControl2();
    waterControl3();
  }
}
}

boolean reconnect() {
  if (WiFi.status() == WL_CONNECTED) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      USBSerial.println("!!connected to MQTT!!");
      if(num_of_counter==1){
        client.subscribe(top1.c_str());
      }
      else if(num_of_counter==2){
        client.subscribe(top1.c_str());
        client.subscribe(top2.c_str());
      }
      else if(num_of_counter==3){
        client.subscribe(top1.c_str());
        client.subscribe(top2.c_str());
        client.subscribe(top3.c_str());
      }
    } else {
      USBSerial.print("failed, rc=");
      USBSerial.print(client.state());
    }
  }
  return client.connected();
}
void mqttConfig() {
  USBSerial.println("MQTT config...");
  client1.setInsecure();
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void callback(char* topic, byte* payload, unsigned int length) {
  USBSerial.print("Message arrived [");
  USBSerial.print(topic);
  USBSerial.print("] ");
  for (int i = 0; i < length; i++) {
    USBSerial.print((char)payload[i]);
  }
  USBSerial.println();
  if (strcmp(topic, top1.c_str()) == 0) {
    payload[length] = '\0';
    counter_1 = atof((char*)payload);
    USBSerial.print("COUNTER1:......................");
    USBSerial.println(counter_1);
  }
  if (strcmp(topic, top2.c_str()) == 0) {
    payload[length] = '\0';
    counter_2 = atof((char*)payload);
    USBSerial.print("COUNTER2:......................");
    USBSerial.println(counter_2);
  }
  if (strcmp(topic, top3.c_str()) == 0) {
    payload[length] = '\0';
    counter_3 = atof((char*)payload);
    USBSerial.print("COUNTER3:......................");
    USBSerial.println(counter_3);
  }
}

void paramSave(){
 if(num_of_counter==1){
  stor1();
 }
 else if(num_of_counter==2){
  stor1();
  stor2();
 }
 else if(num_of_counter==3){
  stor1();
  stor2();
  stor3();
 }
}
void valveControl(){
  if((millis() - sek_millis) >1000){
    isOnValve();
    turnOffValve();
    sek_millis = millis();
  }
}
void isOnValve(){
  if((valveOpen->isOn()) && (!on_milli_start)) { 
    valveOn_millis = millis();
    on_milli_start = true;
  }
  if((valveOpen->isOn()) && ((millis() - valveOn_millis) > 60000)){
    valveOpen->turnOff();
    on_milli_start = false;
  }
}
void turnOffValve(){
  if((valveClose->isOn()) && ((millis() - valveOff_millis)> 60000)){ 
    valveClose->turnOff();
  }
}
void waterControl1() {
  if (millis() - prev_min_millis_1 > time_period_1) {
    if (counter_1 > (tempCounter_1 + level_alarm_1)) {
      valveClose->turnOn();
      valveOff_millis = millis();
      Supla::Notification::Send(-1, dev_name_message_1, message_1);
    }
    tempCounter_1 = counter_1;
    prev_min_millis_1=millis();
  }
  if ((prev_hour_millis_1 - millis() > time_period_night_1) && night_1) {
    if (counter_1 > (tempCounterNight_1 + level_alarm_night_1)) {
      valveClose->turnOn();
      valveOff_millis = millis();
      String n = "NIGHT1: ";
      String n_message = n + message_1;
      Supla::Notification::Send(-1, dev_name_message_1, n_message.c_str());
    }
    tempCounterNight_1 = counter_1;
    prev_hour_millis_1=millis();
  }
}
void waterControl2() {
  if (millis() - prev_min_millis_2 > time_period_2) {
    if (counter_2 > (tempCounter_2 + level_alarm_2)) {
      Supla::Notification::Send(-1, dev_name_message_2, message_2);
    }
    tempCounter_2 = counter_2;
    prev_min_millis_2=millis();
  }
  if ((prev_hour_millis_2 - millis() > time_period_night_2) && night_2) {
    if (counter_2 > (tempCounterNight_2 + level_alarm_night_2)) {
      String n = "NIGHT2: ";
      String n_message = n + message_2;
      Supla::Notification::Send(-1, dev_name_message_2, n_message.c_str());
    }
    tempCounterNight_2 = counter_2;
    prev_hour_millis_2=millis();
  }
}
void waterControl3() {
  if (millis() - prev_min_millis_3 > time_period_3) {
    if (counter_3 > (tempCounter_3 + level_alarm_3)) {
      Supla::Notification::Send(-1, dev_name_message_3, message_3);
    }
    tempCounter_3 = counter_3;
    prev_min_millis_3=millis();
  }
  if ((prev_hour_millis_3 - millis() > time_period_night_3) && night_3) {
    if (counter_3 > (tempCounterNight_3 + level_alarm_night_3)) {
      String n = "NIGHT3: ";
      String n_message = n + message_3;
      Supla::Notification::Send(-1, dev_name_message_3, n_message.c_str());
    }
    tempCounterNight_3 = counter_3;
    prev_hour_millis_3=millis();
  }
}
static bool isNight(int32_t tim, int32_t night_start, int32_t night_end){
    if(night_start < night_end){
      USBSerial.print("hour.............: ");
     USBSerial.println(tim);
      return tim >= night_start && tim < night_end;
    }
    else{
      return tim >= night_start || tim < night_end;
    }
}
void nightReady(){
  if((millis()-prev_minute_millis)> 30000){//sprawdzamy czas co 30s

     bool nn_1 = isNight(clock1.getHour(), night_h_start_1, night_h_stop_1);
     bool nn_2 = isNight(clock1.getHour(), night_h_start_2, night_h_stop_2);
     bool nn_3 = isNight(clock1.getHour(), night_h_start_3, night_h_stop_3);
  
  if (!night_1 && nn_1){
    tempCounterNight_1 = counter_1;  
  }   
  if (!night_2 && nn_2){
    tempCounterNight_2 = counter_2;  
   }  
  if (!night_3 && nn_3){
   tempCounterNight_3 = counter_3;  
  }  
     night_1 = nn_1;
     night_2 = nn_2;
     night_3 = nn_3;
     USBSerial.print("NIGHT1:.............: ");
     USBSerial.println(night_1);
     USBSerial.print("NIGHT2:.............: ");
     USBSerial.println(night_2);
     USBSerial.print("NIGHT3:.............: ");
     USBSerial.println(night_3);
     prev_minute_millis=millis(); 
  }
}
void inputs1(){
  new Supla::Html::CustomTextParameter(PARAM1, "MQTT server", 50);
   new Supla::Html::CustomTextParameter(PARAM2, "MQTT user", 50);
   new Supla::Html::CustomTextParameter(PARAM3, "MQTT password", 50);
   new Supla::Html::CustomParameter(PARAM4, "device ID :1");
   new Supla::Html::CustomParameter(PARAM5, "channel ID :1");
   new Supla::Html::CustomParameter(PARAM6, "night_h_start :1");
   new Supla::Html::CustomParameter(PARAM7, "night_h_end :1");
   new Supla::Html::CustomTextParameter(PARAM8, "devName message :1", 25);
   new Supla::Html::CustomTextParameter(PARAM9, "push message :1", 25);
   new Supla::Html::CustomParameter(PARAM10, "level alarm :1");
   new Supla::Html::CustomParameter(PARAM11, "level alarm night :1");
   new Supla::Html::CustomParameter(PARAM12, "time period(s) :1");
   new Supla::Html::CustomParameter(PARAM13, "time period(s) night :1");
}
void inputs2(){
  new Supla::Html::CustomParameter(PARAM14, "device ID :2");
  new Supla::Html::CustomParameter(PARAM15, "channel ID :2");
  new Supla::Html::CustomParameter(PARAM16, "night_h_start :2");
  new Supla::Html::CustomParameter(PARAM17, "night_h_end :2");
  new Supla::Html::CustomTextParameter(PARAM18, "devName message :2", 25);
  new Supla::Html::CustomTextParameter(PARAM19, "push message :2", 25);
  new Supla::Html::CustomParameter(PARAM20, "level alarm :2");
  new Supla::Html::CustomParameter(PARAM21, "level alarm night :2");
  new Supla::Html::CustomParameter(PARAM22, "time period(s) :2");
  new Supla::Html::CustomParameter(PARAM23, "time period night(s) :2");
}
void inputs3(){
  new Supla::Html::CustomParameter(PARAM24, "device ID :3");
  new Supla::Html::CustomParameter(PARAM25, "channel ID :3");
  new Supla::Html::CustomParameter(PARAM26, "night_h_start :3");
  new Supla::Html::CustomParameter(PARAM27, "night_h_end :3");
  new Supla::Html::CustomTextParameter(PARAM28, "devName message :3", 25);
  new Supla::Html::CustomTextParameter(PARAM29, "push message :3", 25);
  new Supla::Html::CustomParameter(PARAM30, "level alarm :3");
  new Supla::Html::CustomParameter(PARAM31, "level alarm night :3");
  new Supla::Html::CustomParameter(PARAM32, "time period(s) :3");
  new Supla::Html::CustomParameter(PARAM33, "time period night(s) :3");
}
void stor1(){
  if (Supla::Storage::ConfigInstance()->getString(PARAM1, mqtt_server, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM1, mqtt_server);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM1);
  }
  
  if (Supla::Storage::ConfigInstance()->getString(PARAM2, mqtt_user, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM2, mqtt_user);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM2);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM3, mqtt_pass, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM3, mqtt_pass);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM3);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM4, &dv_id_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM4, dv_id_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM4);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM5, &chann_id_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM5, chann_id_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM5);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM6, &night_h_start_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM6, night_h_start_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM6);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM7, &night_h_stop_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM7, night_h_stop_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM7);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM8, dev_name_message_1, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM8, dev_name_message_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM8);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM9, message_1, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM9, message_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM9);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM10, &level_alarm_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM10, level_alarm_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM10);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM11, &level_alarm_night_1)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM11, level_alarm_night_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM11);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM12, &time_period_1)) {
    time_period_1 = time_period_1*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM12, time_period_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM12);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM13, &time_period_night_1)) {
    time_period_night_1 = time_period_night_1*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM13, time_period_night_1);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM13);
  }
}
void stor2(){
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM14, &dv_id_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM14, dv_id_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM14);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM15, &chann_id_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM15, chann_id_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM15);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM16, &night_h_start_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM16, night_h_start_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM16);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM17, &night_h_stop_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM17, night_h_stop_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM17);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM18, dev_name_message_2, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM18, dev_name_message_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM18);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM19, message_2, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM19, message_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM19);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM20, &level_alarm_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM20, level_alarm_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM20);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM21, &level_alarm_night_2)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM21, level_alarm_night_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM21);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM22, &time_period_2)) {
    time_period_2 = time_period_2*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM22, time_period_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM22);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM23, &time_period_night_2)) {
    time_period_night_2 = time_period_night_2*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM23, time_period_night_2);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM23);
  }
}
void stor3(){
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM24, &dv_id_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM24, dv_id_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM24);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM25, &chann_id_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM25, chann_id_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM25);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM26, &night_h_start_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM26, night_h_start_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM26);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM27, &night_h_stop_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM27, night_h_stop_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM27);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM28, dev_name_message_3, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM28, dev_name_message_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM28);
  }
  if (Supla::Storage::ConfigInstance()->getString(PARAM29, message_3, 200)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %s", PARAM29, message_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM29);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM30, &level_alarm_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM30, level_alarm_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM30);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM31, &level_alarm_night_3)) {
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM31, level_alarm_night_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM31);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM32, &time_period_3)) {
    time_period_3 = time_period_3*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM32, time_period_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM32);
  }
  if (Supla::Storage::ConfigInstance()->getInt32(PARAM33, &time_period_night_3)) {
    time_period_night_3 = time_period_night_3*1000;
    SUPLA_LOG_DEBUG(" **** Param[%s]: %d", PARAM33, time_period_night_3);
  } else {
    SUPLA_LOG_DEBUG(" **** Param[%s] is not set", PARAM33);
  }
}
