class Pad 
{
  public:
    Pad() 
    : mLedPin(0), mButtonPin(0)
    { 
    }
    
    void setup(uint8_t iLedPin, uint8_t iButtonPin) 
    {
      mLedPin = iLedPin;
      mButtonPin = iButtonPin;
      pinMode(mLedPin, OUTPUT);
      pinMode(mButtonPin, INPUT_PULLUP);  
    }

    void pulse(unsigned long iDuration)
    {
      digitalWrite(mLedPin, HIGH);   
      delay(iDuration);               
      digitalWrite(mLedPin, LOW);  
    }
    
    bool ckeckPressed()
    {
      if(digitalRead(mButtonPin) == LOW)
      {
        unsigned long start = millis();
        while(digitalRead(mButtonPin) == LOW){}
        return true;  
      }
      return false;   
    }
    
  private:
    uint8_t mLedPin;
    uint8_t mButtonPin;  
};


class I_Engine;

class I_Stage 
{
  public:
    virtual ~I_Stage(){}
    virtual I_Stage& process(I_Engine& iEngine) = 0;
};

class I_Engine 
{
  public:
    enum {NUMBER_OF_PADS = 4};
    typedef Pad Pads[NUMBER_OF_PADS];
  
    enum {SEQUENCE_LENGHT = 100};
    typedef uint8_t Sequence[SEQUENCE_LENGHT]; 
  
    virtual ~I_Engine(){}
    virtual void setupSequence() = 0;
    virtual void incrementStep() = 0;
    
    virtual Pads& getPads() = 0;
    virtual const Sequence& getSequence() = 0;   
    virtual size_t getStep() = 0; 
    virtual unsigned long getSpeed() = 0;
    
    virtual I_Stage& getStart()  = 0;
    virtual I_Stage& getShowSequence()  = 0;
    virtual I_Stage& getCheckSequence()  = 0;
    
    
};

class StartStage : public I_Stage 
{
  public:
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      for(size_t i = 0 ; i < I_Engine::NUMBER_OF_PADS ; i++)
      {
        Serial.println(i);
        if (iEngine.getPads()[i].ckeckPressed())
        {
          iEngine.setupSequence();  
          return iEngine.getShowSequence();
        }  
      }
      return *this;     
    }  
};

class ShowSequenceStage : public I_Stage 
{
  public:
  
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      Serial.print("show step="); Serial.print(iEngine.getStep()); Serial.print(" speed="); Serial.println(iEngine.getSpeed());
      for(size_t i = 0 ; i < iEngine.getStep(); i++)
      {
        Serial.print("  => "); Serial.print(i); Serial.print(" "); Serial.println(iEngine.getSequence()[i]); 
        iEngine.getPads()[iEngine.getSequence()[i]].pulse(iEngine.getSpeed());
        delay(100);  
      } 
      return iEngine.getCheckSequence();  
    }
};

class CheckSequenceStage : public I_Stage 
{
  public:
  
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      for(size_t i = 0 ; i < I_Engine::NUMBER_OF_PADS ; i++)
      {
        if(iEngine.getPads()[i].ckeckPressed())
        {
          iEngine.incrementStep();
          return iEngine.getShowSequence();
        }
        return *this;  
      }       
    }
};


class Engine : public I_Engine 
{
  public: 
    
    
    void setup(uint8_t iLedStartPin, uint8_t iButtonStartPin)
    {
      // seed the pseudo-random number generator with a floating analog input
      randomSeed(analogRead(0));
      
      for(size_t i = 0; i < NUMBER_OF_PADS; i++) 
      {
        mPads[i].setup(iLedStartPin + i, iButtonStartPin + i);  
      }   
    }
  
    void run()
    {
      I_Stage* stage = &mStartStage;
      while(true)
      {
        stage = &stage->process(*this);  
      }    
    }  
  
    virtual void setupSequence()
    {
      for(size_t i = 0; i < SEQUENCE_LENGHT; i++)
      {
        mSequence[i] = random(4);
      }
      mStep = 3;
    }
    
    virtual void incrementStep() 
    {
      mStep++;  
    }
    
    virtual Pads& getPads() {return mPads;}
    virtual const Sequence& getSequence() {return mSequence;}
    virtual size_t getStep() {return mStep;}
    virtual unsigned long getSpeed() {return 500;}
    
    virtual I_Stage& getStart() {return mStartStage; }
    virtual I_Stage& getShowSequence() {return mShowSequenceStage; }
    virtual I_Stage& getCheckSequence() {return mCheckSequenceStage; }
  
  private:
    Pads mPads;
  
    StartStage mStartStage;
    ShowSequenceStage mShowSequenceStage;
    CheckSequenceStage mCheckSequenceStage;
    
    Sequence mSequence;
    size_t mStep;
    
};

Engine sEngine;

void setup() 
{
  Serial.begin(9600);
  sEngine.setup(8, 4); 
} 
 
 
void loop() 
{ 
  sEngine.run();
} 
