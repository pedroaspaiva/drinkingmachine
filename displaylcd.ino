//Programa: Teste de Display LCD 16 x 2
//Autor: FILIPEFLOP
 
//Carrega a biblioteca LiquidCrystal
#include <LiquidCrystal.h>
 
//Define os pinos que serão utilizados para ligação ao display
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
 
void setup()
{
  //Define o número de colunas e linhas do LCD
  lcd.begin(16, 2);
}
 
void loop()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("testando ");
  delay(500);
    lcd.setCursor(0,1);
  for(int x=0;x<16;x++){
    
 //   lcd.print("0 ");
    lcd.scrollDisplayLeft();
    delay(300);
    
  
  }
    delay(500);
    lcd.clear();
}
