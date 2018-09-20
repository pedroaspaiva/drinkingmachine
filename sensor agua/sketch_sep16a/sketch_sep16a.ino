
const int trigPin = 10;
const int echoPin = 11;

const float areabase =166.67; 

const float alturarecipiente = 11; 


// defines variables
long duration; 

float distance;

long volume;


float alturaliquido;



void setup() {


pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input

pinMode(3, OUTPUT);
Serial.begin(9600); // Starts the serial communication

}

void loop() {
  
                 // Clears the trigPin
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

                  volume = alturaliquido*areabase;
                  
                  //Serial.println(alturaliquido);
                  Serial.println(volume);        

delay(100);




}            
