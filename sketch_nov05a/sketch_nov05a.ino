#include <Servo.h>

Servo myservo;

int pos = 90;
int old_pos = 0;

const byte port_btn_plus = 2;
const byte port_btn_minus = 3;
const byte port_btn_record = 4;
const byte port_btn_playback = 5;

bool state_LED_BUILTIN = LOW;
bool old_state_LED_BUILTIN = LOW;
bool old_state_btn_plus = LOW;
bool old_state_btn_minus = LOW;
bool old_state_btn_record = LOW;
bool old_state_btn_playback = LOW;

unsigned long timer_btn_plus = 0;
unsigned long timer_btn_minus = 0;
unsigned long timer_btn_record = 0;
unsigned long timer_btn_playback = 0;

int myservo_array_value[100];

bool flag_record = false;
bool flag_playback = false;
bool flag_clear_array_myservo_value = false;
int pos_myservo_array_value = 0; // позиция ячейки массива для записи
int number_of_values_in_the_myservo_array = 0;
bool departure_from_the_current_position_to_the_initial_one = false;

void btn_control(){
  bool state_btn_plus = digitalRead(port_btn_plus);
  if((!old_state_btn_plus)&&(state_btn_plus)&&(millis()-timer_btn_plus >= 200)){
    state_LED_BUILTIN = !state_LED_BUILTIN;
    digitalWrite(LED_BUILTIN, state_LED_BUILTIN);
    timer_btn_plus = millis();

    pos++;
    if(pos>180){
      pos=180;
    }
    Serial.println(pos);

    departure_from_the_current_position_to_the_initial_one = false; // перед воспроизведением необходимо уехать из текущего положения в начальную точку воспроизведения
  }
  old_state_btn_plus = state_btn_plus;

  bool state_btn_minus = digitalRead(port_btn_minus);
  if((!old_state_btn_minus)&&(state_btn_minus)&&(millis()-timer_btn_minus >= 200)){
    state_LED_BUILTIN = !state_LED_BUILTIN;
    digitalWrite(LED_BUILTIN, state_LED_BUILTIN);
    timer_btn_minus = millis();

    pos--;
    if(pos<0){
      pos=0;
    }
    Serial.println(pos);

    departure_from_the_current_position_to_the_initial_one = false; // перед воспроизведением необходимо уехать из текущего положения в начальную точку воспроизведения
  }
  old_state_btn_minus = state_btn_minus;

  bool state_btn_record = digitalRead(port_btn_record);
  if((!old_state_btn_record)&&(state_btn_record)&&(millis()-timer_btn_record >= 200)){
    state_LED_BUILTIN = !state_LED_BUILTIN;
    digitalWrite(LED_BUILTIN, state_LED_BUILTIN);
    timer_btn_record = millis();

    flag_record = !flag_record;
    Serial.println(flag_record?"RECORD: TRUE":"RECORD: FALSE");
   
  }
  old_state_btn_record = state_btn_record;

  bool state_btn_playback = digitalRead(port_btn_playback);
  if((!old_state_btn_playback)&&(state_btn_playback)&&(millis()-timer_btn_playback >= 200)){
    state_LED_BUILTIN = !state_LED_BUILTIN;
    digitalWrite(LED_BUILTIN, state_LED_BUILTIN);
    timer_btn_playback = millis();

    flag_clear_array_myservo_value = false;
    number_of_values_in_the_myservo_array = 0; // здесь обнулить?
    for(int i=0;i<100;++i){
      if(myservo_array_value[i]!=-1){
        number_of_values_in_the_myservo_array++;
      }
    }
    Serial.print("number_of_values_in_the_myservo_array=");
    Serial.println(number_of_values_in_the_myservo_array);
    for(int i=0;i<number_of_values_in_the_myservo_array;++i){
      Serial.print(myservo_array_value[i]);
      Serial.print(" ");
    }
    Serial.println();

    flag_playback = !flag_playback;
    Serial.println(flag_playback?"PLAYBACK: TRUE":"PLAYBACK: FALSE");
  }
  old_state_btn_playback = state_btn_playback;
}

void setup() {
  myservo.attach(9);

  Serial.begin(9600);
  
  pinMode(port_btn_plus, INPUT_PULLUP);
  pinMode(port_btn_minus, INPUT_PULLUP);
  pinMode(port_btn_record, INPUT_PULLUP);
  pinMode(port_btn_playback, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  btn_control();
  if(!flag_playback){
    //этот блок кода нужен для ситуации, когда воспроизведение закончено, при этом сбрасывается флаг flag_playback и система мгновенно устремляется в позицию по энкодеру
    if(myservo.read()<pos){
        for(int i = myservo.read(); i<=pos; i++){
          myservo.write(i);
          delay(15);
        }
      }else{
        for(int i = myservo.read(); i>=pos; i--){
          myservo.write(i);
          delay(15);
        }
      }
    //--------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //myservo.write(pos);
  }

  if(flag_record){
    if(!flag_clear_array_myservo_value){
      flag_clear_array_myservo_value = true; // присвою значение "ложь" при старте цикла воспроизведения
      pos_myservo_array_value = 0; // сбрасываю ячейку для записи в массив 
      for(int i = 0; i<100; i++){
        myservo_array_value[i] = -1; // необходимо проверить не принимает ли библиотека такое значение???
      }
    }
    if(old_pos != pos){
      old_pos = pos;
      myservo_array_value[pos_myservo_array_value] = pos;
      pos_myservo_array_value++;
    }
  }

  if(flag_playback){
    
    //для плавного выезда в начальную точку воспроизведения----------
    //if(!departure_from_the_current_position_to_the_initial_one){
      //departure_from_the_current_position_to_the_initial_one = true;
      if(myservo.read()<myservo_array_value[0]){
        for(int i = myservo.read(); i<myservo_array_value[0]; i++){
          myservo.write(i);
          delay(15);
        }
      }else{
        for(int i = myservo.read(); i>=myservo_array_value[0]; i--){
          myservo.write(i);
          delay(15);
        }
      }
    //}
    //---------------------------------------------------------------
    
    for(int i = 0; i<number_of_values_in_the_myservo_array; i++){
      myservo.write(myservo_array_value[i]);
      delay(15);
    }
  }
  /*
  if (millis()%500==0){
    delay(5);
    Serial.println(pos);
  }
  */
}
