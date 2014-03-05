void setup() 
{
  for(uint8_t pin = 8 ; pin < 12 ; pin++)
  {
    pinMode(pin, OUTPUT); 
  }
  for(uint8_t pin = 4 ; pin < 8 ; pin++)
  {
    pinMode(pin, INPUT_PULLUP); 
  }
  for(uint8_t pin = 8 ; pin < 12 ; pin++)
  {
    digitalWrite(pin, HIGH);   
    delay(500);               
    digitalWrite(pin, LOW);     
  }
} 
 
 
void loop() 
{ 
  for(uint8_t pin = 4 ; pin < 8 ; pin++)
  {
    if(digitalRead(pin) == LOW)
    {
      digitalWrite(pin+4, HIGH);
      while(digitalRead(pin) == LOW){}
      digitalWrite(pin+4, LOW);  
    } 
  }  
} 
