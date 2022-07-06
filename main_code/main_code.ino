#include "ESPino32CAM.h"
#include "ESPino32CAM_QRCode.h"
#include "WiFi.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include "driver/gpio.h"
#include "HTTPClient.h"

char* ssid = "Redmi 6";
char* password = "000123456789";

String sendval, sendval2,postData;
struct combinedVariables{
  const uint8_t gpio_No;
  uint32_t counted_interrupt;
  bool action;
  String value;
  };
  combinedVariables object1 = {13,0,false};
  String updated_info = "REMOVED";

  ESPino32CAM cam; //image capture object
ESPino32QRCode qr; //image decoding object
//Define os pinos da câmera
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22
#define flash 4
//interrupt function
  static void IRAM_ATTR detected_change(void * arg){
    object1.counted_interrupt +=1;
    object1.action =true;
    }
HTTPClient http;  
//http post data
void postDataToDatabase(String postData){
  http.begin("http://192.168.137.1/dbwrite.php");                             // Connect to host where MySQL database is hosted
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //Specify content-type header
  int httpCode = http.POST(postData);   // Send POST request to php file and store server response code in variable named httpCode
  Serial.println("Values are, sendval = " + object1.value + " and sendval2 = "+updated_info );
  // if connection eatablished then do this
  if (httpCode == 200) { Serial.println("Values uploaded successfully."); Serial.println(httpCode); 
    String webpage = http.getString();    // Get html webpage output and store it in a string
    Serial.println(webpage + "\n");
  } else { 
    // if failed to connect then return and restart
    Serial.println(httpCode); 
    Serial.println("Failed to upload values. \n"); 
    http.end(); 
    return; 
  }
  }
//camera and qr decoding function
     void QR_decode(){
    camera_fb_t *fb = cam.capture();//Capture an image
  
  if (!fb)
  {
    Serial.println("Failure to capture");
    return;
  }
  /*Convert the JPG File to RGB format
    (R 8 bits , G 8 bits , B 8 bits) and store it in dl_matrix3du_t *rgb888.*/
  dl_matrix3du_t *rgb888, *rgb565;
  if (cam.jpg2rgb(fb, &rgb888))
  {
    rgb565 = cam.rgb565(rgb888);
  }
  cam.clearMemory(rgb888);
  cam.clearMemory(rgb565);
  dl_matrix3du_t *image_rgb;

    //try to read a qr code from the image
   if (cam.jpg2rgb(fb, &image_rgb))
  {
    cam.clearMemory(fb);

    qrResoult res = qr.recognition(image_rgb); //decode the image containing the data

    if (res.status) //if you decode the image it shows the data on the screen
    {

      cam.printDebug("Payload: " + res.payload);
      String text = "QR Read:" + res.payload;
      Serial.print("QR detected :" + res.payload);
    object1.value = res.payload;
    postData="sendval=" + object1.value + "&sendval2=" + updated_info;
   
  
    }
    else{
      cam.printDebug("FAIL");
    Serial.print("waiting for QR  ");
  }
  }
  cam.clearMemory(image_rgb); //delete image to receive a new one
}
void setup() {
  // put your setup code here, to run once:
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG,0);
  Serial.begin(115200);
  //pinMode(object1.gpio_No, INPUT_PULLUP);
pinMode(13,INPUT);
digitalWrite(13,LOW);
//Define pin of flash
  digitalWrite(flash, LOW);
  //Configure the pins of camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0; config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA;
  config.jpeg_quality = 4;
  config.fb_count = 1;
  esp_err_t err = esp_camera_init(&config); //Initialization of camera
  if (err != ESP_OK) {
    Serial.printf("Camera start with error 0x%x", err);//Information error of camera
    delay(1000);
    ESP.restart();//Reboot the ESP
  }
   //intial QR CODE
  qr.init(&cam);
  sensor_t *s = cam.sensor();
  s->set_framesize(s, FRAMESIZE_CIF);
  s->set_whitebal(s, true);
  Serial.println();
  Serial.println("started qr scanner");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  err= gpio_isr_handler_add(GPIO_NUM_13, &detected_change, (void *)13);
  if (err != ESP_OK){
    Serial.printf("handler add failed with error 0x%x \r\n",err);}
 err= gpio_set_intr_type(GPIO_NUM_13,GPIO_INTR_HIGH_LEVEL);
 if (err!=ESP_OK){
  Serial.printf("set intr type failed with error 0x%x \r\n", err);
  }
//attachInterrupt(object1.gpio_No,isr, FALLING);
}

void loop() {
  // put your main code here, to run repeatedly:
 
  if (object1.action){
  Serial.printf("Interrupt occured %u times\n",object1.counted_interrupt);
  QR_decode();
     postDataToDatabase(postData);
  object1.action= false;

}

}
