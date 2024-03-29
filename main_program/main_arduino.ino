#include <Wire.h>
#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT22.h>

//Zegar
DS3231 clock;
RTCDateTime dt;

//Wyswietlacz
LiquidCrystal_I2C lcd(0x27, 40, 4);


//Temp DS18B20
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int sterowanieSwiatlem = 7;
int Podsiwietlenie = 4;
bool oswietlenie = false;
int temp_wilg = 12;
int woda = 8;
int sterowanieWoda = 9;

//DHT22 temp i wilgotnosc
DHT22 dht22(temp_wilg); 


//defining rain Icon
byte rainChar1[] = {
  B01110,
  B11110,
  B11111,
  B01101,
  B00001,
  B10010,
  B10010,
  B00100
};

byte rainChar2[] = {
  B00100,
  B01001,
  B01001,
  B10010,
  B10010,
  B00100,
  B00100,
  B01001
};

byte rainChar3[] = {
  B01110,
  B01111,
  B11111,
  B11111,
  B11110,
  B11001,
  B01001,
  B10010
};

byte rainChar4[] = {
  B10010,
  B00100,
  B00100,
  B01001,
  B01001,
  B10010,
  B10010,
  B00100
};






byte rainCharA[] = {
  B01110,
  B11110,
  B11111,
  B01100,
  B10001,
  B00010,
  B10010,
  B00100
};

byte rainCharB[] = {
  B00100,
  B01000,
  B01001,
  B10010,
  B10010,
  B00100,
  B00100,
  B01001
};

byte rainCharC[] = {
  B01110,
  B01111,
  B11111,
  B11111,
  B11110,
  B11000,
  B01001,
  B10010
};

byte rainCharD[] = {
  B10010,
  B00000,
  B00100,
  B00100,
  B01001,
  B01001,
  B10010,
  B10010
};


void setup()
{
  Serial.begin(9600);

  // Inicjalizacja DS3231
  Serial.println("Initialize DS3231");
  clock.begin();

  // Inicjalizacja LCD
  lcd.init();

  // Inicjalizacja DS18B20
  sensors.begin();

  //Sterowanie przełącznikiem
  pinMode(sterowanieSwiatlem, OUTPUT);
  digitalWrite(sterowanieSwiatlem,HIGH);

  pinMode(sterowanieWoda, OUTPUT);
  digitalWrite(sterowanieWoda,HIGH);

  //Chwilowe podswietelnie
  pinMode(Podsiwietlenie,INPUT_PULLUP);

  //Nawodnienie
  pinMode(woda,INPUT_PULLUP);

  lcd.createChar(1 , rainChar1); //Numbering should start at 1, not 0
  lcd.createChar(2 , rainChar2);
  lcd.createChar(3 , rainChar3);
  lcd.createChar(4 , rainChar4);
  lcd.createChar(5 , rainCharA);
  lcd.createChar(6 , rainCharB);
  lcd.createChar(7 , rainCharC);
  lcd.createChar(8 , rainCharD);


}

void loop()
{

  dt = clock.getDateTime(); 
  
  Serial.print(dt.hour);
  Serial.print(":");
  Serial.print(dt.minute);
  Serial.println();
  float tOtoczenie = dht22.getTemperature();
  Serial.println(tOtoczenie);

  if(godzinaCheck())
  {

    if(!oswietlenie)
    {
      oswietlenie = true;
      lcd.backlight();
      lcd.clear();
      digitalWrite(sterowanieSwiatlem, LOW);
    }

    
    if(temperaturaCheck(temperaturaLampa()))
    {

      if(digitalRead(sterowanieSwiatlem) == HIGH)
      {
        digitalWrite(sterowanieSwiatlem, LOW);
        lcd.clear();
      }

      
      if(digitalRead(woda) == LOW)
      {
        lcd.clear();
        nawodnienie();
        lcd.clear();
      }
      lcdStandard();


    }
    else
    {
      if(digitalRead(sterowanieSwiatlem) == LOW)
        digitalWrite(sterowanieSwiatlem, HIGH);
        
    }
    
    
    delay(800);
  }
  else
  {

    if(oswietlenie)
    {
      lcd.noBacklight();
      lcd.clear();
      digitalWrite(sterowanieSwiatlem, HIGH);
      oswietlenie = false;
    }

    if(digitalRead(woda) == LOW)
    {
      lcd.backlight();
      nawodnienie();
      lcd.noBacklight();
      lcd.clear();
    }

    if(digitalRead(Podsiwietlenie) == LOW)
    {
      Serial.print(digitalRead(Podsiwietlenie));
      lcdStandard();
      lcd.backlight();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      delay(700);
      nawodenienieNoc();
      lcd.noBacklight();
      lcd.clear();
    }

    



  }
  

  

  
}

bool godzinaCheck(){
  if(dt.hour >= 8 && dt.hour < 17)
    return true;
  else
    return false;
  
}

bool nawodnienie(){
  digitalWrite(sterowanieWoda, LOW);
  while(digitalRead(woda) == LOW)
    {
      float tLampa = temperaturaLampa();
      float tOtoczenie = dht22.getTemperature();
      float wilgotnosc = dht22.getHumidity();

      lcd.setCursor(0, 0);
      lcd.print(dt.year);   lcd.print("-");
      printDigits(dt.month);  lcd.print("-");
      printDigits(dt.day);
      lcd.setCursor(15, 0);
      printDigits(dt.hour);   lcd.print(":");
      printDigits(dt.minute); 

      lcd.setCursor(0, 1);
      lcd.print(tLampa,1);lcd.print((char)223);lcd.print("C|");
      lcd.setCursor(7, 1);
      lcd.print(tOtoczenie,1);
      lcd.print((char)223);lcd.print("C");
      lcd.setCursor(15, 1);
      lcd.print(wilgotnosc,1);
      lcd.print("%");

      lcd.setCursor(8, 2);
      lcd.print("TRWA");
      lcd.setCursor(4, 3);
      lcd.print("NAWADNIANIE");

      lcd.setCursor(0,2);
      lcd.print(" ");
      lcd.setCursor(0,2);
      lcd.write(1);
      lcd.setCursor(0,3);
      lcd.print(" ");
      lcd.setCursor(0,3);
      lcd.write(2);
      lcd.setCursor(1,2);
      lcd.print(" ");
      lcd.setCursor(1,2);
      lcd.write(3);
      lcd.setCursor(1,3);
      lcd.print(" ");
      lcd.setCursor(1,3);
      lcd.write(4);
      lcd.setCursor(18,2);
      lcd.print(" ");
      lcd.setCursor(18,2);
      lcd.write(5);
      lcd.setCursor(18,3);
      lcd.print(" ");
      lcd.setCursor(18,3);
      lcd.write(6);
      lcd.setCursor(19,2);
      lcd.print(" ");
      lcd.setCursor(19,2);
      lcd.write(7);
      lcd.setCursor(19,3);
      lcd.print(" ");
      lcd.setCursor(19,3);
      lcd.write(8);
      delay(600);
      
      lcd.setCursor(0,2);
      lcd.print(" ");
      lcd.setCursor(0,2);
      lcd.write(5);
      lcd.setCursor(0,3);
      lcd.print(" ");
      lcd.setCursor(0,3);
      lcd.write(6);
      lcd.setCursor(1,2);
      lcd.print(" ");
      lcd.setCursor(1,2);
      lcd.write(7);
      lcd.setCursor(1,3);
      lcd.print(" ");
      lcd.setCursor(1,3);
      lcd.write(8);
      lcd.setCursor(18,2);
      lcd.print(" ");
      lcd.setCursor(18,2);
      lcd.write(1);
      lcd.setCursor(18,3);
      lcd.print(" ");
      lcd.setCursor(18,3);
      lcd.write(2);
      lcd.setCursor(19,2);
      lcd.print(" ");
      lcd.setCursor(19,2);
      lcd.write(3);
      lcd.setCursor(19,3);
      lcd.print(" ");
      lcd.setCursor(19,3);
      lcd.write(4);
      delay(400);

    }
  digitalWrite(sterowanieWoda, HIGH);
}

bool temperaturaCheck(float tLampa){
  if(tLampa > 40)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AWARIA LAMPY");
    lcd.setCursor(0, 1);
    lcd.print("Temp");
    lcd.setCursor(7, 1);
    lcd.print(tLampa,1);lcd.print((char)223);lcd.print("C");
    return false;
  }
  return true;
}

void lcdStandard(){

  float tLampa = temperaturaLampa();
  float tOtoczenie = dht22.getTemperature();
  float wilgotnosc = dht22.getHumidity();

  lcd.setCursor(0, 0);
  lcd.print(dt.year);   lcd.print("-");
  printDigits(dt.month);  lcd.print("-");
  printDigits(dt.day);
  lcd.setCursor(15, 0);
  printDigits(dt.hour);   lcd.print(":");
  printDigits(dt.minute); 
  lcd.setCursor(0, 1);
  lcd.print("Lampa");
  lcd.setCursor(14, 1);
  lcd.print(tLampa,1);lcd.print((char)223);lcd.print("C");
  lcd.setCursor(0, 2);
  lcd.print("Otoczenie");
  lcd.setCursor(14, 2);
  lcd.print(tOtoczenie,1);
  lcd.print((char)223);lcd.print("C");
  lcd.setCursor(0, 3);
  lcd.print("Wilgotnosc");
  lcd.setCursor(15, 3);
  lcd.print(wilgotnosc,1);
  lcd.print("%");

}

void printDigits(int digits)
{
    if(digits < 10)
        lcd.print('0');
    lcd.print(digits);
}

void nawodenienieNoc(){
  if(digitalRead(woda) == LOW)
    {
      lcd.backlight();
      lcd.clear();
      nawodnienie();
      lcd.clear();
      lcdStandard();
    }
}

float temperaturaLampa()
{
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
}
