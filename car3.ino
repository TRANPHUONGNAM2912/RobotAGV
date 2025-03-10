#include <WiFi.h>
#include <WebServer.h>
#include <MatrixMath.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// Đặt tên WiFi (SSID) và mật khẩu
const char* ssid = "ESP32-AP";
const char* password = "123456789";
#define MAX_SIZE 1000
float thetaArray[MAX_SIZE];
float xArray[MAX_SIZE];
float yArray[MAX_SIZE];
// Chỉ số của mảng
int dataIndex = 0;

// Biến lưu giá trị trước đó
float prevTheta = 0;
float prevX = 0;
float prevY = 0;


mtx_type Td[2][2];
mtx_type TdInv[2][2];
LiquidCrystal_I2C lcd(0x27, 16, 2);  

float kp=0.02;      //kp=0.02
float ki=0.00015;   //ki=0.00015
float kd=0.01;      //kd=0.01
hw_timer_t *timer1 = NULL;

int motor1Pin1 = 2; 
int motor1Pin2 = 4; 
int enable1Pin = 12;

int motor2Pin1 = 19; 
int motor2Pin2 = 18; 
int enable2Pin = 15; 

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;

int encoA_motor1 = 26; 
int encoB_motor1 = 13; 
int encoA_motor2 = 32; 
int encoB_motor2 = 14; 

float quangduong1= 0;
float quangduong2= 0;
float quangduongthuc= 0;
float l = 0.18; 
float vantoc = 0;
float vthuc = 0;
volatile unsigned long count = 0;
volatile unsigned long count2 = 0;
unsigned long count_prev = 0;
float Theta_now1; 
float Theta_prev1 = 0;
float Theta_now2; 
float Theta_prev2 = 0;
float Vl_output;
float RPM_output1, RPM_input1;
float Vr_output;
float RPM_output2, RPM_input2;
float dt1;  
float dt2;                     
 
unsigned long t_prev = 0;
unsigned long t_now1;
unsigned long t_prev1 = 0;
unsigned long t_now2;
unsigned long t_prev2 = 0;
volatile long dem_motor1 = 0;
volatile long dem_motor2 = 0;
volatile long dem_motor11 = 0;
volatile long dem_motor22 = 0;
volatile long dem_motor111 = 0;
volatile long dem_motor222 = 0;
int PWMval1=0;
int PWMval2=0;
float Vmax = 12;      
float Vmin = 0;
float V1 = 0;
float V2 = 0;
float error_now1, error_prev1 = 0, integ_now1, integ_prev1 = 0;
float error_now2, error_prev2 = 0, integ_now2, integ_prev2 = 0;
float globalVL = 0;
float globalVR = 0;
float globalVLL = 0;
float globalVRR = 0;
float v = 0 , w = 0;
float x = 0, y = 0, toadox = 0, toadoy = 0;
float theta = 0;
unsigned long prevTime = 0;
float leftWheelSpeedpre = 0;
float rightWheelSpeedpre = 0;
float omega = 0.4;
float R = 50;
float xd=0,yd=0,xd_dot=0,yd_dot=0,xd_2dot,yd_2dot=0,Theta=0,x1d=0,y1d=0,wd=0,x1d_dot=0,y1d_dot=0,
V=0,W=0,VL=0,VR=0,Theta0=0,Thetad=0,X1=0,Y1=0,ex=0,ey=0,eth=0,xe=0,ye=0,thetae=0,Vc=0,Wc=0,vt=0,vp=0,
x0d_dot=0,y0d_dot=0,xdat=0,ydat=0;
float t =0;
float b = 0.03,Kx=1.2,Ky=4.5,Kth=1.0,pi=3.14;
float wthuc=0;
float vr =0;
float wr=0;
bool check = false;
bool flag=false;
unsigned long startTime;
const unsigned long timeoutDuration = 5000;
int buttonStart=25;
int buttonStop=33;
bool prevSw2State = HIGH;
int i=0;
// Khởi tạo server trên cổng 80
WebServer server(80);

// Xử lý yêu cầu truy cập vào root "/"
void handleRoot() {
  server.send(200, "text/html", "OK");
}
void IRAM_ATTR dem_xung_motor1() {
  if (digitalRead(encoB_motor1) == HIGH) {
    dem_motor1++;
    dem_motor11--;
    // dem_motor111++;
  } else {
    dem_motor1++;
    dem_motor11++;
    // dem_motor111--;
  }
}

void IRAM_ATTR dem_xung_motor2() {
  if (digitalRead(encoB_motor2) == HIGH) {
    dem_motor2++;
    dem_motor22++;
    // dem_motor222++;
  } else {
    dem_motor2++;
    dem_motor22--;
    // dem_motor222--;
  }
}
void IRAM_ATTR startbutton() {
  // lcd.clear();
  check = true;
}
void IRAM_ATTR stopbutton() {
 stop();
}
void IRAM_ATTR onTimer1() {
  count++;
  count2++;
}

// Khởi tạo Access Point và server
void setup() {
  Serial.begin(115200);
 pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  ledcSetup(pwmChannel, freq, resolution);
  ledcSetup(pwmChannel + 1, freq, resolution);
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel + 1);

  pinMode(encoA_motor1, INPUT);
  pinMode(encoB_motor1, INPUT);
  pinMode(encoA_motor2, INPUT);
  pinMode(encoB_motor2, INPUT);
  pinMode(buttonStart, INPUT_PULLUP);
  pinMode(buttonStop, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoA_motor1), dem_xung_motor1, RISING);
  attachInterrupt(digitalPinToInterrupt(encoA_motor2), dem_xung_motor2, RISING);
  // attachInterrupt(digitalPinToInterrupt(buttonStart), startbutton, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonStop), stopbutton, FALLING);
  timer1 = timerBegin(0, 80, true); 
  timerAttachInterrupt(timer1, &onTimer1, true); 
  timerAlarmWrite(timer1, 150000, true); 
  timerAlarmEnable(timer1); 
  lcd.init(); 
  lcd.backlight(); 
  // Khởi động WiFi ở chế độ Access Point
  WiFi.softAP(ssid, password);
  Serial.println("Access Point đã được khởi tạo");
  Serial.print("Tên WiFi (SSID): ");
  Serial.println(ssid);
  Serial.print("Địa chỉ IP của Access Point: ");
  Serial.println(WiFi.softAPIP());

  // Cấu hình route cho root "/"
  server.on("/", handleRoot);
  server.on("/T", Tien);
  server.on("/B", Lui);
  server.on("/L", Trai);
  server.on("/R", Phai);
  server.on("/0", Dung);
  server.on("/1", Start);
  server.on("/2", Stop);
  server.on("/3", Startsave);
  server.on("/4", Save);
  server.on("/5", Delete);
   server.on("/6", Contiunue);

  // Khởi động server
  server.begin();
  Serial.println("Web server đã được khởi động");
}
void Tien(){
  Serial.println("tien");
  globalVRR=0.15;
  globalVLL=0.15;
  server.send(200, "text/html", "OK");
}
void Lui(){
  Serial.println("lui");
   globalVRR=-0.15;
      globalVLL=-0.15;
  server.send(200, "text/html", "OK");
}
void Trai(){
  Serial.println("trai");
   globalVRR=0.27;
      globalVLL=0.15;
  server.send(200, "text/html", "OK");
}
void Phai(){
  Serial.println("phai");
  globalVRR=0.15;
      globalVLL=0.27;
  server.send(200, "text/html", "OK");
}
void Dung(){
  Serial.println("dung");
  globalVRR=0.0;
      globalVLL=0.0;
  server.send(200, "text/html", "OK");
}
void Start(){
  Serial.println("start");
        check=true;
        i=0;
      x=0.0;
      y=0.0;
      theta=0.0;
  server.send(200, "text/html", "OK");
}
void Startsave(){
  Serial.println("startsave");
   flag=true;
  server.send(200, "text/html", "OK");
}
void Save(){
  Serial.println("save");
  flag=false;
  server.send(200, "text/html", "OK");
}
void Stop(){
  Serial.println("stop");
   globalVRR=0.0;
      globalVLL=0.0;
      check=false;
  server.send(200, "text/html", "OK");
}
void Delete(){
  Serial.println("delete");
  dataIndex=0;
  i=0;
        x=0.0;
      y=0.0;
      theta=0.0;
  for (int i = 0; i < MAX_SIZE; i++) {
        thetaArray[i] = 0;
        xArray[i] = 0;
        yArray[i] = 0;
    }
  server.send(200, "text/html", "OK");
}
void Contiunue(){
  check=true;
  server.send(200, "text/html", "OK");
}
void loop() {
  // Xử lý các yêu cầu đến web server
  server.handleClient();
   tinhtoado();
   bool currentSw2State = digitalRead(buttonStart);
  if (currentSw2State == LOW && prevSw2State == HIGH) {
    check=true;
    lcd.clear();
  }
  prevSw2State = currentSw2State; 


  if(globalVLL==0){
    ledcWrite(pwmChannel+1, 0);
  }
  if(globalVRR==0){
    ledcWrite(pwmChannel, 0);
  }

  if (count > count_prev) {
    quydao();
    save();
    t_now1 = millis();
    t_now2 = millis();
    dt1 = (t_now1 - t_prev1);
    dt2 = (t_now2 - t_prev2);
    PID1();
    PID2();
    vthuc = (Vl_output+Vr_output)/2;
    wthuc = (Vl_output-Vr_output)/l;
    t_prev1 = t_now1;
    t_prev2 = t_now2;
    count_prev = count;
  }

}
void tinhtoado(){
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0;
  float leftWheelSpeed = (dem_motor22 / 234.3) * 3.14*0.065;
  float rightWheelSpeed = (dem_motor11 / 234.3) * 3.14*0.065;

  v = (rightWheelSpeed + leftWheelSpeed)/2;
  w = (leftWheelSpeed-rightWheelSpeed)/l;
  theta = theta + w;
  x = x + v*cos(theta + (w/2));
  y = y + v*sin(theta + (w/2));

  toadox = x*100;
  toadoy = y*100;
  dem_motor11 = 0;
  dem_motor22 = 0;
  prevTime = currentTime;
}



void handleStart() {
  lcd.clear();
  // String vantocgoc_str = server.arg("vantocgoc");
  // String vantoctrungbinh_str = server.arg("vantoctrungbinh");
  // vantocgoc = vantocgoc_str.toFloat();
  // vantoctrungbinh = vantoctrungbinh_str.toFloat();
  // vantoc = vantoctrungbinh_str.toFloat();
  // String bankinh_str = server.arg("bankinh");
  // R = bankinh_str.toFloat();
  // String omega_str = server.arg("omega");
  // omega = omega_str.toFloat();
  // Serial.println(omega);
  // Serial.println(R);  
  check = true;
  server.send(200, "text/html", "start");
}
void save(){
  if(flag==true){

  
  if (theta != prevTheta || x != prevX || y != prevY) {
    
      // Lưu giá trị mới vào mảng
       thetaArray[dataIndex] = theta;
      xArray[dataIndex] = x;
      yArray[dataIndex] = y;

      // In giá trị để kiểm tra
      Serial.print("Index ");
      Serial.print(dataIndex);
      Serial.print(": theta = ");
      Serial.print(theta);
      Serial.print(", x = ");
      Serial.print(x);
      Serial.print(", y = ");
      Serial.println(y);

      // Cập nhật giá trị trước đó
      prevTheta = theta;
      prevX = x;
      prevY = y;

      // Tăng chỉ số mảng
      dataIndex++;
  
  }
  }
}

void quydao(){
  if(check==true){
  //     if(i >= dataIndex){
  //    globalVRR=0.0;
  //     globalVLL=0.0;
  // }
  //     if(i < dataIndex){
  // xdat = (R)*cos(omega*t);
  // ydat = (R)*sin(omega*t);
  // xd = (R/100)*cos(omega*t);
  // yd = (R/100)*sin(omega*t);
  // xd_dot = -(R/100)*omega*sin(omega*t);
  // yd_dot = (R/100)*omega*cos(omega*t);
  // x0d_dot = -(R/100)*omega*sin(omega);
  // y0d_dot = (R/100)*omega*cos(omega);
  // xd_2dot = -(R/100)*omega*omega*cos(omega*t);
  // yd_2dot = -(R/100)*omega*omega*sin(omega*t);
  // Theta0 = atan2(y0d_dot,x0d_dot)-omega;
  // Thetad = omega*t + Theta0;
  // x1d = xd + b*cos(Thetad);
  // y1d = yd + b*sin(Thetad);
  // wd = (yd_2dot * xd_dot - xd_2dot * yd_dot) / (xd_dot * xd_dot + yd_dot * yd_dot);
  // x1d_dot = xd_dot - b * wd * sin(Thetad);
  // y1d_dot = yd_dot + b * wd * cos(Thetad);
  // Td[0][0] = cos((Thetad));
  // Td[0][1] = -b * sin((Thetad));
  // Td[1][0] = sin((Thetad));
  // Td[1][1] = b * cos((Thetad));
  // Matrix.Invert((mtx_type*)Td, 2);
  // Matrix.Copy((mtx_type*)Td, 2, 2, (mtx_type*)TdInv);
  // vr = TdInv[0][0]*x1d_dot + TdInv[0][1]*y1d_dot;
  // wr = TdInv[1][0]*x1d_dot + TdInv[1][1]*y1d_dot;
  // X1=x+b*cos(theta);
  // Y1=y+b*sin(theta);
  xd = xArray[i];
  yd = yArray[i];
  Thetad = thetaArray[i];
  ex = xd - x;
  ey = yd - y;
  eth = Thetad - theta;
  // Serial.println(ex);
  // Serial.println(ey);
  // Serial.println(eth);
  //   Serial.println(i);
  // Serial.println(Index);
    
  //    Serial.println("       ");
  xe = cos((theta))*ex + sin((theta))*ey;
  ye = -sin((theta))*ex + cos((theta))*ey;
  thetae = eth;

  Vc= 2.0*xe; 
  Wc=2.0*ye + 2.0*thetae;

  // V= Vc-vthuc;
  // W= Wc-wthuc;
  // globalVR = (((2*60)/(d*pi))*V+((0.2*60)/(d*pi))*W)/2; 
  // globalVL = (((2*60)/(d*pi))*V-((0.2*60)/(d*pi))*W)/2;
  globalVRR = (2 * Vc + (l * (Wc))) / 2;
  globalVLL = 2 * Vc - globalVRR;
  float p=sqrt((xd - x) * (xd - x) + (yd - y) * (yd - y));
  // 2. Tính góc hướng tới đích (alpha)
  float alpha = atan2(yd - y, xd - x) - theta;

  // 3. Tính góc lệch cuối (beta)
  float beta = Thetad - theta - alpha;

  // 4. Tính vận tốc tuyến tính (v) và vận tốc góc (omega)
  // Vc = 1.0 * p;
  // Wc = 1.0 * alpha ;

  // 5. Tính vận tốc bánh trái (v_L) và bánh phải (v_R)
  // globalVLL = Vc - (Wc * 0.38 / 2.0);
  // globalVRR = v + (Wc * 0.38 / 2.0);
  // if (count2 % 14 == 0) {
  //    lcd.setCursor(0,0);
  // lcd.print("Vd= ");
  // lcd.setCursor(3,0);
  // lcd.print(Vc);
  //  lcd.setCursor(9,0);
  // lcd.print("Vt= ");
  // lcd.setCursor(12,0);
  // lcd.print(vthuc);
  //    lcd.setCursor(0,1);
  // lcd.print("Wd= ");
  // lcd.setCursor(3,1);
  // lcd.print(Wc);
  //  lcd.setCursor(9,1);
  // lcd.print("Wt= ");
  // lcd.setCursor(12,1);
  // lcd.print(wthuc);
  // }

  //  lcd.setCursor(0,0);
  // lcd.print("Vd= ");
  // lcd.setCursor(3,0);
  // lcd.print(Vc);
  //  lcd.setCursor(9,0);
  // lcd.print("Vt= ");
  // lcd.setCursor(12,0);
  // lcd.print(vthuc);
  //    lcd.setCursor(0,1);
  // lcd.print("Wd= ");
  // lcd.setCursor(3,1);
  // lcd.print(Wc);
  //  lcd.setCursor(9,1);
  // lcd.print("Wt= ");
  // lcd.setCursor(12,1);
  // lcd.print(wthuc);
  // if(globalVRR<0){
  //   globalVRR=0;
  // }
  //  if(globalVLL<0){
  //   globalVLL=0;
  // }
  Serial.println(xd);
  Serial.println(yd);
  Serial.println(Thetad);
    Serial.println(x);
  Serial.println(y);
  Serial.println(theta);
    Serial.println(i);
  Serial.println(dataIndex);
      Serial.println(Vc);
  Serial.println(Wc);
  Serial.print(ESP.getFreeHeap());
    Serial.println(" bytes");
    Serial.println("    ");
    
  t = t+0.15;
  if(i<dataIndex-1){
      i++;
  }

  }

}
void stop() {
  check=false;
  globalVRR=0;
  globalVLL=0;

  // R=0;
  // Vc=0;
  // Wc=0;
  RPM_input1=0;
  RPM_input2=0;
  server.send(200, "text/html", "stop");
}
// void continuee() {
//   String bankinh_str = server.arg("bankinh");
//   R = bankinh_str.toFloat();
//   String omega_str = server.arg("omega");
//   omega = omega_str.toFloat();
//   String kx_str = server.arg("kx");
//   Kx = kx_str.toFloat();
//   String ky_str = server.arg("ky");
//   Ky = ky_str.toFloat();
//   String ktheta_str = server.arg("ktheta");
//   Kth = ktheta_str.toFloat();
//   server.send(200, "text/html", "continuee");
// }
void reset() {
  server.send(200, "text/html", "reset");
}
void PID1(){
  RPM_input1=abs((globalVLL/(0.065*3.14))*60);
  Theta_now1 = dem_motor1 / 234.3;
  RPM_output1 = (Theta_now1 - Theta_prev1) / (dt1 / 1000.0) * 60;
  // quangduong1 = float(float(dem_motor111/249.6)*0.2*3.14) ;
  Vr_output = (RPM_output1/60)*(0.065*3.14);
  error_now1 = RPM_input1 - RPM_output1;
  integ_now1 = integ_prev1 + (dt1 * (error_now1 + error_prev1) / 2);

  V1 = kp * error_now1 + ki * integ_now1 + (kd * (error_now1 - error_prev1) / dt1) ;
    
  if (V1 > Vmax) {
    V1 = Vmax;
    integ_now1 = integ_prev1;
  }
  if (V1 < Vmin) {
    V1 = Vmin;
    integ_now1 = integ_prev1;
  }
    
  PWMval1 = int(255 * abs(V1) / Vmax);
  if (PWMval1 > 255) {
    PWMval1 = 255;
  }
  if(globalVLL>0){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
    ledcWrite(pwmChannel, PWMval1);
  }
  if(globalVLL<0){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
    ledcWrite(pwmChannel, PWMval1);
  }
  Theta_prev1 = Theta_now1;
  integ_prev1 = integ_now1;
  error_prev1 = error_now1;
  
}

void PID2(){
  RPM_input2=abs((globalVRR/(0.065*3.14))*60);
  Theta_now2 = dem_motor2 / 234.3;
  RPM_output2 = (Theta_now2 - Theta_prev2) / (dt2 / 1000.0) * 60;
  // quangduong2 = float(float(dem_motor222/249.6)*0.2*3.14) ;
  Vl_output = (RPM_output2/60)*(0.065*3.14);
  error_now2 = RPM_input2 - RPM_output2;
  integ_now2 = integ_prev2 + (dt2 * (error_now2 + error_prev2) / 2);

  V2 = kp * error_now2 + ki * integ_now2 + (kd * (error_now2 - error_prev2) / dt2) ;
    
  if (V2 > Vmax) {
    V2 = Vmax;
    integ_now2 = integ_prev2;
  }
  if (V2 < Vmin) {
    V2 = Vmin;
    integ_now2 = integ_prev2;
  }
    
  PWMval2 = int(255 * abs(V2) / Vmax);
  if (PWMval2 > 255) {
    PWMval2 = 255;
  }
  if(globalVRR>0){
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);
    ledcWrite(pwmChannel + 1, PWMval2);
  }
  if(globalVRR<0){
    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);
    ledcWrite(pwmChannel + 1, PWMval2);
  }

  Theta_prev2 = Theta_now2;
  integ_prev2 = integ_now2;
  error_prev2 = error_now2;
}