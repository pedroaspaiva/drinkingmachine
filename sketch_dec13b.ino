#include <SPI.h>
#include <MFRC522.h>
#include <Ultrasonic.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
#define NUMDRINKS 3
// Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x3f, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
int BOTOES[] = {A0, A1, A2}; // Pinos que receberão o sinal dos push-bottons
int BOMBAS[] = {0,1 ,2 }; // pinos que ativarão os relés
int qtdes[] = {150, 300};
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;
int quantia;
//medidas do líquido
const int trigPin[] = {8,6,4};
const int echoPin[] = {7,5,3};

Ultrasonic ultrasonic(trigPin[0], echoPin[0]);
Ultrasonic ultrasonic1(trigPin[1], echoPin[1]);
Ultrasonic ultrasonic2(trigPin[2], echoPin[2]);
const float areabase = 86.65;
const float alturarecipiente = 21;
long duration;
float distance;
long volume, volume1;
int bomba, quantidade;
float alturaliquido;
int verificaCartao(){
    lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("APROXIME");
  lcd.setCursor(4, 1);
  lcd.print("O CARTAO");
   Serial.println("ESPERANDO CARTAO");
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
    return 1;
  

}
  return 0;
  
  }

int leituraBotao() {
  int botao = 0;
  Serial.print("APERTA AI POOOOOOOOOOO");
  do {
    if (analogRead(A0) >= 700) { // coca cola escolhida
      botao = 1;
      bomba = BOMBAS[0];
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("COCA-COLA");
    }
    else if (analogRead(A1) >= 700)
    {
      botao = 2;
      bomba = BOMBAS[1];
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("GUARANA");
    }
    else if (analogRead(A2) >= 700) {
      lcd.clear();
      lcd.setCursor(5, 0);
      lcd.print("COROTE");
      botao = 3;
      bomba = BOMBAS[2];
    }
    //Serial.println("A0     A1     A2");
    //Serial.print(analogRead(A0));Serial.print("   ");Serial.print(analogRead(A1));Serial.print("   ");Serial.println(analogRead(A2));

  } while (!botao);
  delay(1500);
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("ESCOLHA A");
  lcd.setCursor(3, 1);
  lcd.print("QUANTIDADE");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(qtdes[0]); lcd.print("ML "); lcd.print(qtdes[1]); lcd.print("ML"); lcd.print(" SAIR");
  quantidade = 0;
  do {
    if (analogRead(A0) >= 700) {
      quantia = 0;
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print(qtdes[0]); lcd.print("ML ");
      quantidade = qtdes[0];

    }
    else if (analogRead(A1) >= 700) {
      quantia = 1;
      lcd.clear();
      lcd.setCursor(6, 0);
      lcd.print(qtdes[1]); lcd.print("ML ");
      quantidade = qtdes[1];
    }
    else if (analogRead(A2) >= 700) {
      while (analogRead(A2) >= 700);
      delay(50);
      return 15;

    }

  } while (!quantidade);
  delay(1000);
  Serial.println("saiu");
  return botao;

}
int ativaBomba(int bomba) {
  
  int cont;
 
  digitalWrite(bomba,HIGH);

  digitalWrite(trigPin[bomba], LOW);
                   //Serial.println("ENTREEI");
                  delayMicroseconds(2);
                  // Sets the trigPin on HIGH state for 10 micro seconds
                  digitalWrite(trigPin[bomba], HIGH);
                  delayMicroseconds(10);
                  digitalWrite(trigPin[bomba], LOW);
                  // Reads the echoPin, returns the sound wave travel time in microseconds
                  duration = pulseIn(echoPin[bomba], HIGH);
                  // Calculating the distance
                  distance=1* duration*0.0343/2;
                  // Prints the distance on the Serial Monitor

                  //Serial.println(distance); 

                  alturaliquido = ((-1)*distance)+ alturarecipiente;

                  volume = alturaliquido*areabase;
                  delay(100);
                  
                  do{
                    
                    //Serial.println("N FOI AINDA");
                    digitalWrite(trigPin[bomba], LOW);
                  delayMicroseconds(2);
                  // Sets the trigPin on HIGH state for 10 micro seconds
                  digitalWrite(trigPin[bomba], HIGH);
                  delayMicroseconds(10);
                  digitalWrite(trigPin[bomba], LOW);
                  // Reads the echoPin, returns the sound wave travel time in microseconds
                  duration = pulseIn(echoPin[bomba], HIGH);
                  // Calculating the distance
                  distance=1* duration*0.0343/2;
                  // Prints the distance on the Serial Monitor

                  //Serial.println(distance); 

                  alturaliquido = ((-1)*distance)+ alturarecipiente;

                  volume1 = alturaliquido*areabase;
                  delay(100);
                    
                  
                    //Serial.print(volume);Serial.print(" ");Serial.println(volume1);
                    }while(volume1>(volume-quantidade-40));                  
               Serial.println("SAIUUUU");

  digitalWrite(bomba, LOW);
  return;
}
  
void setup() {
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  for (int j = 0; j < 3; j++) {
    pinMode(BOTOES[j], INPUT);
    pinMode(BOMBAS[j], OUTPUT);
  }
  lcd.begin(16, 2);

}

void loop() {
    byte valor[] = {0xFF, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00,
                  0x00, 0xCD, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00
                 };
  byte valor1[] = {0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00
                  };
  boolean error;
  byte ende;
  byte cont = 0;
  int drinkdata[3][NUMDRINKS] = {2, 1, 1,
                                 3, 2, 2,
                                 0, 0, 1
                                };
  int decisaobb = 0, // escolha da bebida a comprar
      cobrar; // valor a ser cobrado
        lcd.clear();
  lcd.setCursor(6, 0);
  lcd.print("BAR");
  lcd.setCursor(5, 1);
  lcd.print("DUINO");
  decisaobb = leituraBotao();
  if (decisaobb == 15)
    return;
  cobrar = drinkdata[quantia][decisaobb - 1];
  error = verificaCartao();
  if (error == 1 ) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERROp NA LEITURA");
    delay(700);
  
    return;

  }
  
  (MFRC522::StatusCode) mfrc522.MIFARE_Read(4, valor1, 16);
  Serial.println(valor1[0]);
    if (cobrar > valor1[0]) // Verifica se o valor do saldo (gravado em readwrite[0]) é maior do que o valor a ser cobrado
  {
    Serial.println("Você não tem créditos para comprar essa bebida");
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("SALDO");
    lcd.setCursor(2, 1);
    lcd.print("INSUFICIENTE");
    delay(2000);
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    return;
  }
  valor1[0] -= cobrar;
  status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(4, valor1, 16);
    if (status != MFRC522::STATUS_OK) {
    Serial.print(F("PCD_Authenticate() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ERRO1 NA LEITURA");
    delay(1300);
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    return ;
  }
    lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("COMPRA EFETUADA");
  lcd.setCursor(3, 1);
  lcd.print("SALDO: "); lcd.print(valor1[0]);
  delay(1000);
 
    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    ativaBomba(bomba);
}




