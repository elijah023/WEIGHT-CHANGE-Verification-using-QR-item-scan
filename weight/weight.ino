float weight;
#include<HX711.h>
#define DT_PIN 4
#define CLK_PIN 3
HX711 scale;
float calibration_factor =107650;
void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(8,OUTPUT);
scale.begin(4,3);
scale.set_scale();
scale.tare();
long zero_factor = scale.read_average();
Serial.println(zero_factor);
digitalWrite(8,LOW);
}
void loop() {
  // put your main code here, to run repeatedly:
scale.set_scale(calibration_factor);
weight = (scale.get_units(3)); 
Serial.println(weight);
delay(1000);
if (weight>=0.40){
  //Serial.println("done");
  digitalWrite(8,HIGH);
  delay(1000);
  digitalWrite(8,LOW);
  }
  
}