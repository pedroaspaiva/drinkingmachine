#include <SPI.h>
#include <MFRC522.h>
#include<EEPROM.h>
#include <Ultrasonic.h>



#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define NUMDRINKS 3
// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x3f,2,1,0,4,5,6,7,3, POSITIVE);
int BOTOES[] = {A0,A1,A2}; // Pinos que ativarão as bombas
int qtdes[] = {150,300};
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
int quantia;
const int trigPin = 6;
const int echoPin = 7;
Ultrasonic ultrasonic(trigPin, echoPin);
const float areabase =86.65; 

const float alturarecipiente = 21; 


// defines variables
long duration; 

float distance;

long volume,volume1;

int bomba,quantidade;
float alturaliquido;
/**
   Initialize.
*/
MFRC522::StatusCode status;
void setup() {
  
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  lcd.begin(16, 2);
  pinMode(4, OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);  
  int i;
  for (i = 0; i < 3; i++) {
    pinMode(BOTOES[i], OUTPUT);
  }
  // Prepare the key (used both as key A and as key B)
  // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

}
void loop() {

  byte valor[] = {0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00
                 };
  byte readwrite[16] = {0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00
                 };
  boolean error;
  byte blocosaldo = 4, blocoid = 5,cont=0;

  int drinkdata[3][NUMDRINKS] = {2, 1, 1,
                                 3, 2, 2, 
                                 0, 0, 1 };
                                 
  int decisao, // escolha do menu inicial 
  decisaobb=0, // escolha da bebida a comprar 
  cobrar; // valor a ser cobrado
  int verificaEstouro;
  String idcard = "", idadm = "";
  for(cont = 0;cont<4;cont++){
    idadm.concat(String(EEPROM.read(cont),HEX));
    
    }
    idadm.toUpperCase();
 

  clr();
  lcd.clear();
  lcd.setCursor(6,0);
  lcd.print("BAR");
  lcd.setCursor(5,1);
  lcd.print("DUINO");
 
  
  //decisao = menu(); // 1=comprar  2=recarregar 3= ver saldo




   //exibeBebidas();
   decisaobb = leituraBotao();
  if(decisaobb==15)
    return;
    //    if (status != MFRC522::STATUS_OK) {
    //        Serial.print(F("MIFARE_Read() failed: "));
    //        Serial.println(mfrc522.GetStatusCodeName(status));
    //    }
    cobrar = drinkdata[quantia][decisaobb-1];
   
    error = verificaCartao();
    if(error == true ){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ERRO NA LEITURA");
      delay(1300);
            // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
      return;
      
      }
    
   delay(10);
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blocosaldo, readwrite, 16);
//          if (status != MFRC522::STATUS_OK) {
//    Serial.print(F("PCD_Authenticate() failed: "));
//    Serial.println(mfrc522.GetStatusCodeName(status));
//       lcd.clear();
//      lcd.setCursor(0,0);
//      lcd.print("ERRO NA LEITURA");
//      delay(1300);
//            // Halt PICC
//  mfrc522.PICC_HaltA();
//  // Stop encryption on PCD
//  mfrc522.PCD_StopCrypto1();
//    return ;
//  }
    if (cobrar > readwrite[0]) // Verifica se o valor do saldo (gravado em readwrite[0]) é maior do que o valor a ser cobrado 
    {
      Serial.println("Você não tem créditos para comprar essa bebida");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("SALDO");
      lcd.setCursor(2,1);
      lcd.print("INSUFICIENTE");
      delay(2000);
       mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
      return;
    }
    if(drinkdata[1][decisaobb-1]==1){
      if(readwrite[15]!=255){
        Serial.println("Essa opção só está disponível para usuários maiores de 18 anos");
        lcd.clear();
        lcd.setCursor(2,0);
        lcd.print("COMPRA NEGADA");
        lcd.setCursor(6,1);
        lcd.print("+18");
        delay(2000);
          // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
        return;
        }
      
      }
    readwrite[0]-= cobrar;
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blocosaldo, readwrite, 16);
      if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
       lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ERRO NA LEITURA");
      delay(1300);
            // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
    return ;
  }
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("COMPRA EFETUADA");
    lcd.setCursor(3,1);
    lcd.print("SALDO: ");lcd.print(readwrite[0]);
    delay(1000);
     ativaBomba(bomba);
      // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
    return;
}
void clr() {
  int i;
  for (i = 0; i < 20; i++)
    Serial.println(" ");
}

int menu() {
  int decisao;
  Serial.println("Escolha uma opção:");
  Serial.println("1----- Comprar uma bebida");
  Serial.println("2----- Recarregar cartão");
  Serial.println("3----- Verificar saldo");
  do {
    decisao = Serial.read() - 48;
  } while (decisao != 1 && decisao != 2 && decisao != 3);

  return decisao;
}
int exibeBebidas() {
  int decisaobb;
  Serial.println("1-Coca-Cola --> 3,00");
  Serial.println("2-Guaraná --> 3,00");
  Serial.println("3-Corotinho -->2,00");
  do {
    decisaobb = Serial.read() - 48;
  } while (decisaobb != 1 && decisaobb != 2 && decisaobb != 3);
  return decisaobb;

}
int saida() {
  int sair ;
  Serial.println("Digite s para sair");
  do {
    sair = Serial.read();
  } while (sair != 's');
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
  return;


}
int ativaBomba(int bomba) {
  
  int cont;
  float mediav[5],mediav1[5];
  digitalWrite(bomba,HIGH);
  for(cont=0;cont<5;cont++){
  digitalWrite(trigPin, LOW);
                   //Serial.println("ENTREEI");
                  delayMicroseconds(2);
                  // Sets the trigPin on HIGH state for 10 micro seconds
                  digitalWrite(trigPin, HIGH);
                  delayMicroseconds(10);
                  digitalWrite(trigPin, LOW);
                  // Reads the echoPin, returns the sound wave travel time in microseconds
                  duration = pulseIn(echoPin, HIGH);
                  // Calculating the distance
                  distance=1* duration*0.0343/2;
                  // Prints the distance on the Serial Monitor

                  //Serial.println(distance); 

                  alturaliquido = ((-1)*distance)+ alturarecipiente;

                  mediav[cont] = alturaliquido*areabase;
                  delay(100);
                  }
                  volume = (mediav[0] + mediav[1] + mediav[2]+mediav[3]+mediav[4])/5;
                  do{
                    for(cont=0;cont<5;cont++){
                    //Serial.println("N FOI AINDA");
                    digitalWrite(trigPin, LOW);
                  delayMicroseconds(2);
                  // Sets the trigPin on HIGH state for 10 micro seconds
                  digitalWrite(trigPin, HIGH);
                  delayMicroseconds(10);
                  digitalWrite(trigPin, LOW);
                  // Reads the echoPin, returns the sound wave travel time in microseconds
                  duration = pulseIn(echoPin, HIGH);
                  // Calculating the distance
                  distance=1* duration*0.0343/2;
                  // Prints the distance on the Serial Monitor

                  //Serial.println(distance); 

                  alturaliquido = ((-1)*distance)+ alturarecipiente;

                  mediav1[cont] = alturaliquido*areabase;
                  delay(100);
                    }
                    volume1 = (mediav1[0] + mediav1[1]+ mediav1[2]+mediav1[3]+mediav1[4])/5;
                    //Serial.print(volume);Serial.print(" ");Serial.println(volume1);
                    }while(volume1>(volume-quantidade-40));                  
               Serial.println("SAIUUUU");

  digitalWrite(bomba, LOW);
  return;
}
int verificaCartao() {
  Serial.println("Aproxime o cartao");
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("APROXIME");
  lcd.setCursor(4,1);
  lcd.print("O CARTAO");
  do {
    while ( ! mfrc522.PICC_IsNewCardPresent());
  } while ( ! mfrc522.PICC_ReadCardSerial());

  // Authenticate using key A
  Serial.println(F("Authenticating using key A..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return true;
  }
//   Show the whole sector as it currently is
  Serial.println(F("Current data in sector:"));
  mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, 1);
  Serial.println();
  // Authenticate using key B
  Serial.println(F("Authenticating again using key B..."));
  status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, 7, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return true;
  }
  return false;
}
int leituraBotao(){
 int botao=0;
 Serial.print("APERTA AI POOOOOOOOOOO");
 
 do{ 
  if(analogRead(A0) >= 700){ // coca cola escolhida
   botao=1;
   bomba = 4;
   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("COCA-COLA");
 } 
  else if(analogRead(A1) >=700)
  {
    botao = 2;
    bomba = 3;
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("GUARANA");
  }
  else if(analogRead(A2) >=700){
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("COROTE");
    botao =3;  
    bomba = 5;
  }
  //Serial.println("A0     A1     A2");
  //Serial.print(analogRead(A0));Serial.print("   ");Serial.print(analogRead(A1));Serial.print("   ");Serial.println(analogRead(A2));
  
  }while(!botao);
  delay(1500);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("ESCOLHA A");
  lcd.setCursor(3,1);
  lcd.print("QUANTIDADE");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(qtdes[0]);lcd.print("ML ");lcd.print(qtdes[1]);lcd.print("ML");lcd.print(" SAIR");
  quantidade=0; 
  do{
    if(analogRead(A0)>=700){
      quantia = 0;
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print(qtdes[0]);lcd.print("ML ");
      quantidade = qtdes[0];
      
      }
     else if(analogRead(A1)>=700){
      quantia = 1;
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print(qtdes[1]);lcd.print("ML ");
      quantidade = qtdes[1];
      }
      else if(analogRead(A2)>=700){
        while(analogRead(A2)>=700);
        delay(50);
        return 15;
        
        } 
        
    }while(!quantidade);
  delay(1000);
  
  
  Serial.println("saiu");
return botao;
}
