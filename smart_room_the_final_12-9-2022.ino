/* settting up the configeration of board */ 
#include <SoftwareSerial.h>
#include <IRremote.h>
#define doorlockON 2 
#define IR_LED 3
#define doorlockOFF 4 
#define pazzar 6 
#define fan 5 
#define relay2 7 
#define relay1 8 
#define relay3 9 
#define relay4 12 
#define inSWITCH 13 
#define RECV_PIN A0
#define microphoneSENSEOR A1 
#define lightSENSOR A2 
#define tempratureSENSOR A3 
#define resestorSENSOR A4 
#define electricMETER A5 
#define ON 1
#define OFF 0
IRsend irsend;
IRrecv irrecv(RECV_PIN);
decode_results results;

SoftwareSerial bt(11,10);
String bt_device= "" ;
unsigned char bt_value;

bool relay_1_status=false;
bool relay_2_status=false;
bool relay_3_status=false;
bool relay_4_status=false;



unsigned char pazzarValue = 255;
typedef enum{
	maniual_state=0,
	automatic_state,
	IR_control_state,
	bluetooth_state,
	wifi_state,
	ethernet_state
}State;

void pin_setup(){
  pinMode(doorlockON,OUTPUT);
  pinMode(doorlockOFF,OUTPUT);
  pinMode(fan,OUTPUT);
  }
void sensores_setup(){
  pinMode(inSWITCH,INPUT);
  pinMode(microphoneSENSEOR,INPUT);
  pinMode(lightSENSOR,INPUT);
  pinMode(tempratureSENSOR,INPUT);
  pinMode(resestorSENSOR,INPUT);
  pinMode(electricMETER,INPUT);
  }
void relays_setup(){
  pinMode(relay1,OUTPUT);
  pinMode(relay2,OUTPUT);
  pinMode(relay3,OUTPUT);
  pinMode(relay4,OUTPUT);
}
void sound_setup(){
  pinMode(pazzar,OUTPUT);
  }
void IR_setup(){
   irrecv.enableIRIn();  
   }

/*/*      relays functions     **/ 
void relay_on(unsigned char channel){
	switch (channel){
		case 1:
		digitalWrite(relay1,HIGH);
		relay_1_status = true;
		break;
		case 2:
		digitalWrite(relay2,HIGH);
		relay_2_status = true;
		break;
		case 3:
		digitalWrite(relay3,HIGH);
		relay_3_status = true;
		break;
		case 4:
		digitalWrite(relay4,HIGH);
		relay_4_status = true;
		break;
	}
}
void relay_off(unsigned char channel){
	switch (channel){
		case 1:
		digitalWrite(relay1,LOW);
		relay_1_status = false;
		break;
		case 2:
		digitalWrite(relay2,LOW);
		relay_2_status = false;
		break;
		case 3:
		digitalWrite(relay3,LOW);
		relay_3_status = false;
		break;
		case 4:
		digitalWrite(relay4,LOW);
		relay_4_status = false;
		break;
	}
}

/*/*   IR functions  		  **/
void IRsend_Code(unsigned int sent_address,unsigned int sent_command){
	irsend.sendNEC(sent_address, 32); //send address for chose the device 
	delay(50);
	irsend.sendNEC(sent_command, 32); //send the command to control on another device
}
unsigned int IRrecv_Code(unsigned int recv_address){
	if (irrecv.decode(&results)) {
		if(results.value == recv_address){
			if(irrecv.decode(&results)){
				return results.value;
			}
		}
		irrecv.resume();
	}
	else{return 0;}
}

/*/*   puzzar function        **/
void pazzarFunction(unsigned char it,unsigned char percent){
  for(unsigned char i=0;i<it;i++){
	  analogWrite(pazzar,percent);
	  delay(100);
	  analogWrite(pazzar,0);
	  delay(100);
    }
}
/*/*	fan function       	 **/  
void fanFunction(unsigned char percent){
  for(unsigned char i=0;i<percent;i++){digitalWrite(fan,1);delay(20);}
  for(unsigned char i=0;i<100-percent;i++){digitalWrite(fan,0);delay(20);}
  }

/* scanning for every prepheral and making report */
/*/* SENSING FUNCTIONS **/
float tempratureSENSOR_value(){
    int Vo;
    float R1 = 4480.9016;
    float logR2, R2, Temperature;
    float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
      Vo = analogRead(tempratureSENSOR);
      R2 = R1 * (1023.0 / (float)Vo - 1.0);
      logR2 = log(R2);
      Temperature = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
      Temperature = Temperature - 273.15;
      //T = (T * 9.0)/ 5.0 + 32.0; 
    return Temperature;
  }
float lightSENSOR_value(){
      int sensorValue = analogRead(lightSENSOR);
      int percent = sensorValue * (100.0 / 1023.0);
      percent = percent*(100.0/51.0);
      return percent;
    }
String microphoneSENSEOR_value(){
      int sensorValue = analogRead(microphoneSENSEOR);
      float voltage = sensorValue * (5.0 / 1023.0);
      if(voltage>2.7){
        return"clap";
	   }
	  else if(voltage>1.94&&voltage<2.1){
		  return "stillHere";
		}
	  else if(voltage<1.94){
		  return "noOne";
		}
	}
float electricMETER_value(){
      int sensorValue1 = analogRead(electricMETER);
      float percent = sensorValue1 *((5.0*100) /(5.48 * 1023.0)); //5.48 max charge
      return percent;
  }
int resestorSENSOR_value(){
      int sensorValue = analogRead(resestorSENSOR);
      int percent = sensorValue * (100.0 / 1023.0);
      return percent;
  }
bool inSWITCH_value(){
      bool value = digitalRead(inSWITCH);
      return value;
  } 

/*/* SCANING ALGORITHM FOR REAL SENSING VALUE **/
float sensing_temperature(){
  float sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = tempratureSENSOR_value();
    }
  float index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){
      num = max(indexItteration[i],num);
    }
    unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
         }
    }
  return index[indexNumber];
}
unsigned char sensing_light(){
  float sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = lightSENSOR_value();
    }
  float index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){
      num = max(indexItteration[i],num);
    }
    unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
         }
    }
  return index[indexNumber];
}
/*String sensing_sound(){
  String sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = microphoneSENSEOR_value();
    }
  String index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){
      num = max(indexItteration[i],num);
    }
    unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
         }
    }
    return index[indexNumber];
}
*/
unsigned char sensing_charge(){
  float sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = electricMETER_value();
    }
  float index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){
      num = max(indexItteration[i],num);
    }
    unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
         }
    }
  return index[indexNumber];
}

unsigned char sensing_resistor(){
  float sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = resestorSENSOR_value();
    }
  float index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){
      num = max(indexItteration[i],num);
    }
    unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
         }
    }
  return index[indexNumber];
}

unsigned char sensing_switch(){
  float sensorValue[10];
  for(unsigned char i=0;i<10;i++){
    sensorValue[i] = inSWITCH_value();
    }
  float index[10];
  unsigned char indexItteration[10]={0};
  for(unsigned char i=0;i<10;i++){
    index[i]= sensorValue[i];
    for(unsigned char j=0;j<10;j++){
      if(index[i]== sensorValue[j]){indexItteration[i]++;}
    }
  }
  unsigned char num=0;
  for(int i=0;i<10;i++){num = max(indexItteration[i],num);}
  unsigned char indexNumber;
  for(int i=0;i<10;i++){
      if(indexItteration[i]== num){
         indexNumber = i;
        }
    }
  return index[indexNumber];
}

String irSENSOR_sense(){
  if (irrecv.decode(&results)) {
    switch(results.value){
         case 0xE318261B:
         return "turnAll";///1 for turn all the control pins togillized 
         break;
         
         case 0x511DBB:
         return "modeChange";///2 for mode of automatic or maniual to control of somethings in system togillized
         break;
         
         case 0xEE886D7F:
         return "pazzarOff";///3 for mute togillized
         break;
         
         case 0x52A3D41F:
         return "play";///4 for play / pause 
         break;
         
         case 0xD7E84B1B:
         return "backward";///5 for backward
         break;
         
         case 0x20FE4DBB:
         return "forward";///6 for forward
         break;
         
         case 0xF076C13B:
         return "EQ";///7 for EQ
         break;
         
         case 0xA3C8EDDB:
         return "vol-";///8 for vol-
         break;
         
         case 0xE5CFBD7F:
         return "vol+";///9 for vol+
         break;
      
         case 0xC101E57B:
         return "0";///10 for 0
         break;
      
         case 0x97483BFB:
         return "recall";///11 for recall
         break;
      
         case 0xF0C41643:
         return "scn";///12 for scn
         break;
         
         case 0x9716BE3F:
         return "1";///13 for 1
         Serial.println("pressed");
         break;
         
         case 0x3D9AE3F7:
         return "2";///14 for 2
         break;
         
         case 0x6182021B:
         return "3";///15 for 3
         break;
         
         case 0x8C22657B:
         return "4";///16 for 4
         break;
         
         case 0x488F3CBB:
         return "5";///17 for 5
         break;
         
         case 0x449E79F:
         return "6";///18 for 6
         break;
         
         case 0x32C6FDF7:
         return "7";///19 for 7
         break;
         
         case 0x1BC0157B:
         return "8";///20 for 8
         break;
         
         case 0x3EC3FC1B:
         return "9";///21 for 9
         break;
      //end of main remote 
         case 0x143226DB:
         return "turnAll";///1 for turn all the control pins
         break;
      
         case 0x371A3C86:
         return "1";///13 for 1
         break;
      
         case 0xA32AB931:
         return "timer";///22 for timer
         break;
         
         case 0xE0984BB6:
         return "2";///14 for 2
         break;
            
         case 0x39D41DC6:
         return "3";///15 for 3
         break;
         
         case 0x1D2FEFF6:
         return "4";///16 for 4
         break;
         
         case 0x4EA240AE:
         return "modeChange";///2 for mode to control of somethings in system
         break;
        }
        irrecv.resume();
    }
}
   
   
   
   
   
// start of control
State statu = bluetooth_state;
/* maniual control */
bool no = 0;
bool no1=0;
bool no2 = 0;
void manual(){
  
  if(no==0)
  {Serial.println("maniual");no = 1;no1=0;no2 = 0;pazzarFunction(1,255);}
	//
	static unsigned char tick = 0;
	
	if(inSWITCH_value() == ON){delay(1000);if(inSWITCH_value() == ON){tick++;while(inSWITCH_value() == ON);}}
	switch (tick){
		case 1:
			//pazzarFunction(1,150);
			relay_on(1);
			break;
		case 2:
			//pazzarFunction(1,150);
			relay_off(1);
			break;
		case 3:
			//pazzarFunction(1,150);
			relay_on(2);
			break;
		case 4:
			//pazzarFunction(1,150);
			relay_off(2);
			break;
		case 5:
			//pazzarFunction(1,150);
			relay_on(3);
			break;
		case 6:
			//pazzarFunction(1,150);
			relay_off(3);
			break;
		case 7:
			//pazzarFunction(1,150);
			relay_on(4);
			break;
		case 8:
			//pazzarFunction(1,150);
			relay_off(4);
			break;
		case 9:
			//pazzarFunction(1,150);
			relay_on(1);
			relay_on(2);
			relay_on(3);
			relay_on(4);
			break;
		case 10:
			//pazzarFunction(1,150);
			relay_off(1);
			relay_off(2);
			relay_off(3);
			relay_off(4);
			break;
		case 11:
			statu = automatic_state;
			tick = 0;
			break;
		}
	if (bt.available()){statu = bluetooth_state;}
  
	else if (irrecv.decode(&results)) {
		switch(results.value){
		    case 0x511DBB:
			 //return "modeChange";///2 for mode of automatic or maniual to control of somethings in system togillized
			statu = IR_control_state;
			break;
		 }
		irrecv.resume();
	}
  
	if(microphoneSENSEOR_value()=="clap"){if(relay_1_status){relay_off(1);}else{relay_off(1);}}

}






// serve part
void setup() {
  
  pin_setup();
  sensores_setup();
  relays_setup();
  sound_setup();
  IR_setup();
  
  Serial.begin(9600);
  bt.begin(9600);

}

void loop() {

switch (statu){
		case maniual_state: // manual
			manual();
			break;
//		case automatic_state: // automatic
//			automatic();
//			break;
		case IR_control_state: // IR_control
			IR_control();
			break;
		case bluetooth_state: // bluetooth
			bluetooth_control();
			break;
		case wifi_state: // wifi
			wifi_control();
			break;
		case ethernet_state: // ethernet
			ethernet_control();
			break;
      
		/*default:
			automatic();
			break;*/
	}

}
