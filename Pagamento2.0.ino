#include <SPI.h>
#include <MFRC522.h>

constexpr uint8_t RST_PIN = 9;     // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = 10;     // Configurable, see typical pin layout above
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::MIFARE_Key key;
/**
 * Initialize.
 */
void setup(){
    Serial.begin(9600); // Initialize serial communications with the PC
    while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
    SPI.begin();        // Init SPI bus
    mfrc522.PCD_Init(); // Init MFRC522 card

        // Prepare the key (used both as key A and as key B)
    // using FFFFFFFFFFFFh which is the default at chip delivery from the factory
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
  
}
void loop(){

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;
    byte valor[]={0x00,0x00,0x00,0x00,
                  0x00,0x00,0x00,0x00,
                  0x00,0x00,0x00,0x00,
                  0x00,0x00,0x00,0x00};
    byte leitura[16];              
    byte blockaddr = 4;
    int decisao,decisaobb,preco1=3,preco2=3,preco3=2; 
    int verificaEstouro;
        

  MFRC522::StatusCode status;
    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 7, &key, &(mfrc522.uid));
       if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }
             
              // Show the whole sector as it currently is
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
   
    decisao = menu();
  if(decisao==1){
    decisaobb = exibeBebidas();
        status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockaddr, leitura, 16);
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("MIFARE_Read() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//    }
      if(decisaobb==1){
           if(preco1>leitura[0]){
          Serial.print("Você não tem créditos para essa compra, digite s para sair");
          saida();
          clr();
          return;
          }
        valor[0] = leitura[0] - preco1;
        
        }
      else if(decisaobb==2){
           if(preco2>leitura[0]){
          Serial.print("Você não tem créditos para essa compra, digite s para sair");
          saida();
          clr();
          return;
          }
        valor[0] = leitura[0]-preco2;
        
        }
      else if(decisaobb==3){
        if(preco3>leitura[0]){
          Serial.print("Você não tem créditos para essa compra, digite s para sair");
          saida();
          clr();
          return;
          }
        valor[0] = leitura[0]-preco3;
        
        }  
           status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockaddr, valor, 16);
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("MIFARE_Write() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//    }
    Serial.println("Compra Efetuada! Digite s para sair");
    saida();
    clr();
        
    }
    else if(decisao==2){
        Serial.println("Digite um valor para recarregar");
  do{
    valor[0] = Serial.parseInt();
    
    }while(valor[0]<=0);
    
//
     status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockaddr, leitura, 16);
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("MIFARE_Read() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//    }
    verificaEstouro = valor[0];
    verificaEstouro+= leitura[0];
    valor[0]+= leitura[0];
    if(verificaEstouro>255){
      Serial.println("ERRO: O valor nao pode ultrapassar o máximo de 255");
      Serial.print("Digite s para sair");
      saida();
      clr();
      return;
      }
    
    // Write data to the block
   status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockaddr, valor, 16);
//    if (status != MFRC522::STATUS_OK) {
//        Serial.print(F("MIFARE_Write() failed: "));
//        Serial.println(mfrc522.GetStatusCodeName(status));
//    }
    Serial.println();
      
      
    }
  else if(decisao==3){
      status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockaddr, leitura, 16);
  
    Serial.print("Seu saldo é de R$: "); Serial.println(leitura[0]);    
    
    
    }

    
  
        
Serial.println("Concluído, digite s para sair");

saida();
clr();
     // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();


    
  }
 void clr(){
  int i;
  for(i=0;i<20;i++)
    Serial.println(" ");
  }

  int menu(){
    int decisao;
    Serial.println("Escolha uma opção:");
    Serial.println("1----- Comprar uma bebida");
    Serial.println("2----- Recarregar cartão");
    Serial.println("3----- Verificar saldo");
    do{
      decisao = Serial.read()-48;
      }while(decisao!=1 && decisao!=2 &&decisao!=3);

    return decisao;
   }
int exibeBebidas(){
  int decisaobb;
  Serial.println("1-Coca-Cola --> 3,00");
  Serial.println("2-Guaraná --> 3,00");
  Serial.println("3-Corotinho -->2,00");
  do{
    decisaobb = Serial.read()-48;
    }while(decisaobb!=1 && decisaobb!=2 && decisaobb!=3);
    return decisaobb;

  }
  int saida(){
  int sair ;
    
    do{
  sair = Serial.read();
  }while(sair!='s');
    
    
    
    }

   
