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
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;

const int trigPin = 6;
const int echoPin = 7;
Ultrasonic ultrasonic(trigPin, echoPin);
const float areabase =66.67; 

const float alturarecipiente = 11; 


// defines variables
long duration; 

float distance;

long volume,volume1;

int bomba;
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
  pinMode(3, OUTPUT);

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
  byte readwrite[16];
  byte blocosaldo = 4, blocoid = 5,cont=0;

  int drinkdata[2][NUMDRINKS] = {3, 3, 2,
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
  lcd.setCursor(4,0);
  lcd.print("DRINKING");
  lcd.setCursor(4,1);
  lcd.print("MACHINE");
  
  //decisao = menu(); // 1=comprar  2=recarregar 3= ver saldo

 decisao=1;
  
  if (decisao == 1) {


   //exibeBebidas();
   decisaobb = leituraBotao();
  
    //    if (status != MFRC522::STATUS_OK) {
    //        Serial.print(F("MIFARE_Read() failed: "));
    //        Serial.println(mfrc522.GetStatusCodeName(status));
    //    }
    cobrar = drinkdata[0][decisaobb-1];
    delay(1500);
    verificaCartao();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blocosaldo, readwrite, 16);
    if (cobrar > readwrite[0]) // Verifica se o valor do saldo (gravado em readwrite[0]) é maior do que o valor a ser cobrado 
    {
      Serial.println("Você não tem créditos para comprar essa bebida");
      lcd.clear();
      lcd.setCursor(5,0);
      lcd.print("SALDO");
      lcd.setCursor(2,1);
      lcd.print("INSUFICIENTE");
      delay(2000);
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
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("COMPRA EFETUADA");
    lcd.setCursor(3,1);
    lcd.print("SALDO: ");lcd.print(readwrite[0]);
    delay(2000);
     ativaBomba(bomba);
      // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
    return;

  }
  else if (decisao == 2) {
    Serial.println("Aproxime o cartão de administrador para ter acesso a esse procedimento");
    verificaCartao();
     for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     
    idcard.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
    idcard.toUpperCase();
    if(idcard.substring(1) != idadm.substring(1)){
      Serial.println("Apenas o cartão do administrador poderá efetuar recargas");
      saida();
      return;
      }
        // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
      Serial.print("Cartão ADM identificado: ");
    Serial.println("Digite um valor para recarregar");
    do {
      valor[0] = Serial.parseInt();

    } while (valor[0] <= 0);
    Serial.println("Aproxime o cartão do usuário que receberá a recarga");
    verificaCartao();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blocosaldo, readwrite, 16);
    //    if (status != MFRC522::STATUS_OK) {
    //        Serial.print(F("MIFARE_Read() failed: "));
    //        Serial.println(mfrc522.GetStatusCodeName(status));
    //    }
    verificaEstouro = (int)(valor[0]);
    verificaEstouro += (int)( readwrite[0]);
    valor[0] += readwrite[0];
    valor[16] = readwrite[16];
    if (verificaEstouro > 255) {
      Serial.println("ERRO: O valor nao pode ultrapassar o máximo de 255");

      saida();
      clr();
      return;
    }

    // Write data to the block
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blocosaldo, valor, 16);
    //    if (status != MFRC522::STATUS_OK) {
    //        Serial.print(F("MIFARE_Write() failed: "));
    //        Serial.println(mfrc522.GetStatusCodeName(status));
    //    }
    Serial.print("Concluído, seu novo saldo é R$:"); Serial.println(valor[0]);
    saida();
    return;


  }
  else if (decisao == 3) {
    verificaCartao();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blocosaldo, readwrite, 16);

    Serial.print("Seu saldo é de R$: "); Serial.println(readwrite[0]);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SALDO : "); lcd.print(readwrite[0]);
    saida();
    return;


  }

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
  digitalWrite(bomba, HIGH);
digitalWrite(trigPin, LOW);
                   Serial.println("ENTREEI");
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

                  volume = alturaliquido*areabase;
                  delay(100);
                  do{
                    Serial.println("N FOI AINDA");
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

                  volume1 = alturaliquido*areabase;
                  delay(100);
                    
                    Serial.print(volume);Serial.print(volume1);
                    }while(volume1>(volume-300));                  
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
    return;
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
    return;
  }
}
int leituraBotao(){
 int botao=0;
 Serial.print("APERTA AI POOOOOOOOOOO");
 
 do{ 
  if(analogRead(A1) >= 700){ // coca cola escolhida
   botao=1;
   bomba = 4;
   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("COCA-COLA");
 } 
  else if(analogRead(A0) >=700)
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

  Serial.println("saiu");
return botao;
}
