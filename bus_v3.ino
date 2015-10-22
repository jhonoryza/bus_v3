//#include <fonts/Arial14.h>
#include <fonts/SystemFont5x7.h>

#include <fonts/Tahoma.h>
#include <SPI.h>
#include <DMD2.h>
#include <SD.h>
#include <LCD5110_Basic.h>
#include <elapsedMillis.h>

const uint8_t *FONT = Tahoma;
const uint8_t *smallFont = Tahoma; 
//const uint8_t *FONTKoridor = Tahoma; 

const int BTN_F = 0;
const int BTN_R = 1;
const int BTN_C = 2;
unsigned long last_read = 0;
unsigned long prevMillis = 0;
unsigned long prevMillis1 = 0;

int RST = 3;
int CLK = 4;
int DAT = 5; 
const int W1 = 4;
const int W2 = 2;
const int W3 = 3;
SoftDMD Depan(W1,1,22,23,24,26,25,27);
SoftDMD Indoor(W2,1,28,29,30,32,31,33);
SoftDMD SampingKiri(W3,1,34,35,36,38,37,39);
SoftDMD SampingKanan(W3,1,40,41,42,44,43,45);
DMD_TextBox box(Depan,20,0,107,16); 
DMD_TextBox boxIndoor(Indoor,0,0,128,16);
DMD_TextBox boxSampingKiri(SampingKiri,20,0,75,16);
DMD_TextBox boxSampingKanan(SampingKanan,20,0,75,16); 
int xDepan;
int xIndoor;
int xKiri;
int xKanan;
const int TotalKoridor = 4;
char* fileName_k[1]={"k1.txt"};
char* fileName_h1[TotalKoridor]={"1a.txt","2a.txt","3a.txt","4a.txt"};
//,"5a.txt",
//"6a.txt","7a.txt","8a.txt","9a.txt","10a.txt",
//"11a.txt","12a.txt","13a.txt","14a.txt","15a.txt",
//"16a.txt","17a.txt","18a.txt","19a.txt","20a.txt"};
//char* fileName_h2[100]={"1b.txt","2b.txt","3b.txt","4b.txt","5b.txt"};
//char* fileName_s1[100]={"s1a.txt","s2a.txt","s3a.txt","s4a.txt","s5a.txt"};
//char* fileName_s2[100]={"s1b.txt","s2b.txt","s3b.txt","s4b.txt","s5b.txt"};
int noKoridor = 1;
int noHalte = 1;
boolean pilihKoridor = false;
boolean pilihHalte = false;
boolean success = false;
String koridor[50];
String h[50][50];
File myFile;
int button_f;
int button_r;
int button_c;
extern uint8_t SmallFont[];
//extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];
LCD5110 myGLCD(8,9,10,11,12);
unsigned long intervalDelay = 0;
unsigned long debounceDelay = 300;
int countKoridor = 0;
int countSamping = 0;
int countHalte = 0;

int xx = 96;
int xx2 = 128;
char buf[2]={'a','b'};
elapsedMillis timeElapsed;
boolean sudahInputText = false;
void setup() {
  Serial.begin(9600);
  myGLCD.InitLCD();
   pinMode(RST, OUTPUT);
   pinMode(CLK, OUTPUT); 
   pinMode(DAT, OUTPUT);
    digitalWrite(RST, HIGH);
    digitalWrite(CLK, HIGH);
    digitalWrite(DAT, HIGH);    
    digitalWrite(RST, LOW);
    delay(5);
    digitalWrite(RST, HIGH);
    delay(300);

  lcdWrite("Initializing SD card...");
  if(!SD.begin(53)) {
    lcdWrite(" failed!");
    return;
  }
  success = true;
  lcdWrite("SUCCESS");
  
  // hi-speed SPI transfers
  SPI.setClockDivider(4);

  pinMode(BTN_F,INPUT_PULLUP);
  pinMode(BTN_R,INPUT_PULLUP);
  pinMode(BTN_C,INPUT_PULLUP);

  Depan.setBrightness(255);
  Depan.selectFont(FONT);
  Depan.begin();
  Indoor.setBrightness(255);
  Indoor.selectFont(FONT);
  Indoor.begin();
  SampingKiri.setBrightness(255);
  SampingKiri.selectFont(FONT);
  SampingKiri.begin();
  SampingKanan.setBrightness(255);
  SampingKanan.selectFont(FONT);
  SampingKanan.begin();

  xDepan = W1*32;
  xIndoor = W2*32;
  xKiri = W3*32;
  xKanan = xKiri;
  readKoridor();
}

void loop() {
  if(success==true){
      //jika belum memilih koridor
      if(!pilihKoridor){
        button_f = digitalRead(BTN_F);
        button_r = digitalRead(BTN_R);
        button_c = digitalRead(BTN_C);
          if(button_f == LOW)
          {
            myGLCD.clrScr();
            noKoridor += 1;
            if(noKoridor>20)
              noKoridor=20;
              delay(debounceDelay);
          }
          else if(button_r == LOW){
            myGLCD.clrScr();
            noKoridor-=1;
            if(noKoridor<0)
              noKoridor=0;
              delay(debounceDelay);
          }
          else if(button_c == LOW && noKoridor>0){
            myGLCD.clrScr();
            pilihKoridor = true;
            delay(debounceDelay);
          }
          else{
            if( (millis() - intervalDelay) >= 300) 
            {
              intervalDelay = millis();
              LCD("KORIDOR",noKoridor);
            }
          }
      }
      //jika sudah dipilih koridor tampilkan menu halte
      else if(pilihKoridor){
          showKoridor();
      }
    }
    else{
      lcdWrite("FAILED");
      delay(1000);
    }
}

void showKoridor(){
        countKoridor = koridor[noKoridor-1].length() * 6;
        if(countKoridor <= 110)
          countKoridor = 0;
        else
          countKoridor = countKoridor - 110;
        
        countSamping = koridor[noKoridor-1].length() * 6;
        if(countSamping <= 78)
          countSamping = 0;
        else
          countSamping = countSamping - 78;
          
        xDepan = W1*32;
        xIndoor = W2*32;
        xKiri = W3*32;
        xKanan = xKiri;
        
        Indoor.clearScreen();
        int count = 0;
        int count_2 = 0;        
        for(int i=0;i<(110+countKoridor);i++){
          button_f = digitalRead(BTN_F);
          button_r = digitalRead(BTN_R);
          button_c = digitalRead(BTN_C);
          if(button_f == LOW){
            myGLCD.clrScr();
            noHalte+=1;
            if(noHalte>20)
              noHalte=20;
               delay(debounceDelay);
          }
          else if(button_r == LOW){
            myGLCD.clrScr();
            noHalte-=1;
            if(noHalte<0)
              noHalte=0;
               delay(debounceDelay);
          }
          else if(button_c == LOW && noHalte>0){
            pilihHalte = true;
            myGLCD.clrScr();
            //showHalte();
             delay(debounceDelay);
          }
          else{
          //show lcd
          if( (millis() - intervalDelay) >= 100){
              intervalDelay = millis();
              LCD("Halte",noHalte);
          }
          if(!sudahInputText)
            inputTextKoridor(koridor[noKoridor-1]);  
          else
                    
            
            //Tampilkan halte
//                if(pilihHalte){
//                  countHalte = h[noKoridor-1][noHalte-1].length() * 6;
//                  if(countHalte <= 64)
//                    countHalte = 0;
//                  else
//                    countHalte = countHalte - 64;
//                    
//                  if(count_2 <=(64+countHalte)){
//                    Indoor.drawString(0,0,h[noKoridor-1][noHalte-1]);
//                    boxIndoor.scrollX(-1);
//                    xIndoor--;
//                  }
//                  else{
//                    count_2 = 0;
//                  }
//                  count_2++;
//               }
//                
//                delay(100);
          }
      }
}

void showHalte(){
  xIndoor = W2*32;
  Indoor.clearScreen();
    for(int i=0;i<64;i++){
            Indoor.drawString(0,0,h[noKoridor-1][noHalte-1]);
//            boxIndoor.scrollX(-1);
//            xIndoor--;
//            delay(100);
  }
}

void readKoridor(){
   char character;
   String settingValue;
   int counter = 0;
   myFile = SD.open(fileName_k[0]);
   lcdWrite("Read Koridor ");
   delay(100);
   if (myFile) {
      while (myFile.available()) {
        character = myFile.read();
           if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
            {
                 settingValue = settingValue + character;
            }
            else  
            {
              koridor[counter] = settingValue;
              settingValue = "";
              delay (100);
              counter++;
            }
    
      }
   myFile.close();
   lcdWrite("Success");
   delay(100);
   readHalte_a(); 
   } else {
   // if the file didn't open, print an error:
    lcdWrite("ERROR READ");
    delay(100);
   }   
}

void readHalte_a(){
   char character;
   String settingValue;
   int counter = 0;
   lcdWrite("Read Halte ");
   delay(100);
   for(int i=0; i<TotalKoridor;  i++){
     myFile = SD.open(fileName_h1[i]);
     if (myFile) {
        while (myFile.available()) {
          character = myFile.read();
             if (character != '\n') // define breaking char here (\n isnt working for some reason, i will follow this up later)
              {
                   settingValue = settingValue + character;
              }
              else{
                h[i][counter] = settingValue;
                settingValue = "";
                delay (100);
                counter++;
              }
        }
     myFile.close();
     } else {
     lcdWrite("ERROR READ");
     delay(100);
     }
     counter = 0;
   }
   lcdWrite("Success");
   delay(100);
   
}

void LCD(String z, long int y){
  myGLCD.setFont(SmallFont);
  myGLCD.print(z, CENTER, 0);
  myGLCD.setFont(BigNumbers);
  myGLCD.printNumI(y, CENTER, 8, 2, '0');
}

void lcdWrite(String z)
{
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print(z, CENTER, 0);
}

void inputTextKoridor(String text){
  String len = text;
  int max = 7;
  int panjang = len.length();
  
  int maxSplit = panjang / max;
  if(panjang % max != 0)
    maxSplit +=1;
  String temp[maxSplit];
  for(int i=0;i<maxSplit;i++)
    temp[i]="";
  int lastCount = (maxSplit-1)*7;
  if(panjang > max){
    //split
    int i = 0;
    int j = 0;
    while(i<maxSplit){
    char buff[panjang+1];
    len.toCharArray(buff,panjang+1);
        if(i >= maxSplit-1){
          for(int k=lastCount;k<panjang;k++){
            temp[i] += buff[j];
            j++;
          }
        }
        else{
          for(int k=0;k<7;k++){
            temp[i] += buff[j];
            j++;
          }
        }
    i++;
    }
    for(int m=0;m<maxSplit;m++){
      Serial.println(temp[m]);
    }
  }
  sudahInputText = true;
  displayKoridor(maxSplit, temp);
}

void displayKoridor(int maxSplit, String temp[]){
  if(noKoridor<10){
    SampingKiri.drawString(2,2,"0"+String(noKoridor));
    SampingKiri.drawBox(0,0,18,15);
  }
  else{
    SampingKiri.drawString(2,2,String(noKoridor));
    SampingKiri.drawBox(0,0,18,15);
  }
  Depan.drawString(2,2,String(noKoridor));
  Depan.drawBox(0,0,18,15);
  
  for(int m =0;m<maxSplit;m++){
    int i=0;
    xx = 96;
    xx2 = 128;
    int panjangSamping = SampingKiri.stringWidth(temp[m]);
    int panjangKoridor = Depan.stringWidth(temp[m]);
    while(i < panjangSamping+2){
      SampingKiri.drawString(xx,2,temp[m]);
      Depan.drawString(xx2,2,temp[m]);
      if (timeElapsed > 150){               
        box.scrollX(-1);
        boxSampingKiri.scrollX(-1);
        xx--;
        xx2--;
        i++;
        timeElapsed = 0;              
      }
    }
  }
}


