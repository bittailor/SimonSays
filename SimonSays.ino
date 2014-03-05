// ----------------------------------------------------------------------------

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

    void flash(unsigned long iDuration)
    {
      unsigned long start = millis();
      while((millis() - start) < iDuration)
      {
        digitalWrite(mLedPin, HIGH);   
        delay(100);               
        digitalWrite(mLedPin, LOW);   
      }
    }
    
    bool checkPressed()
    {
      if(digitalRead(mButtonPin) == LOW)
      {
        while(digitalRead(mButtonPin) == LOW){}
        return true;  
      }
      return false;   
    }
    
  private:
    uint8_t mLedPin;
    uint8_t mButtonPin;  
};

// ----------------------------------------------------------------------------

class I_Engine;

// ----------------------------------------------------------------------------

class I_Stage 
{
  public:
    virtual ~I_Stage(){}
    virtual I_Stage& process(I_Engine& iEngine) = 0;
};

// ----------------------------------------------------------------------------

class I_Engine 
{
  public:
   
    virtual ~I_Engine(){}

    virtual uint8_t waitForPressed() = 0;
 
    virtual void setupSequence() = 0;
    virtual void showSequence() = 0;
    virtual bool checkSequence() = 0;
    virtual void incrementStep() = 0;
     
    virtual I_Stage& getStart()  = 0;
    virtual I_Stage& getShowSequence()  = 0;
    virtual I_Stage& getCheckSequence()  = 0;
    
    
};

// ----------------------------------------------------------------------------

class StartStage : public I_Stage 
{
  public:
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      iEngine.waitForPressed();
      iEngine.setupSequence();  
      return iEngine.getShowSequence();      
    }  
};

// ----------------------------------------------------------------------------

class ShowSequenceStage : public I_Stage 
{
  public:
  
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      iEngine.showSequence();
      return iEngine.getCheckSequence();  
    }
};


// ----------------------------------------------------------------------------

class CheckSequenceStage : public I_Stage 
{
  public:
  
    virtual I_Stage& process(I_Engine& iEngine) 
    {
      if(!iEngine.checkSequence())
      {
        return iEngine.getStart();
      }
      iEngine.incrementStep();
      return iEngine.getShowSequence();    
    }
};

// ----------------------------------------------------------------------------

class Engine : public I_Engine 
{
  public:
    enum {NUMBER_OF_PADS = 4};
    enum {NUMBER_OF_LEVELS = 100};
       
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

    virtual uint8_t waitForPressed()
    {
      while(true)
      {
        for(size_t i = 0 ; i < NUMBER_OF_PADS ; i++)
        {
          if (mPads[i].checkPressed())
          {
            return i;
          }  
        }    
      }
    }

    virtual void setupSequence()
    {
      for(size_t i = 0; i < NUMBER_OF_LEVELS; i++)
      {
        mSequence[i] = random(4);
      }
      mLevel = 3;
    }
    
    virtual void showSequence()
    {
      Serial.print("show step="); Serial.print(mLevel); Serial.print(" speed="); Serial.println(speed());
      for(size_t i = 0 ; i < mLevel; i++)
      {
        Serial.print("  => "); Serial.print(i); Serial.print(" "); Serial.println(mSequence[i]); 
        mPads[mSequence[i]].pulse(speed());
        delay(100);  
      }   
    }

    virtual bool checkSequence()
    {
      Serial.print("check step="); Serial.print(mLevel); Serial.print(" speed="); Serial.println(speed());
      for(size_t i = 0 ; i < mLevel; i++)
      {
        Serial.print("  => "); Serial.print(i); Serial.print(" "); Serial.println(mSequence[i]); 
        uint8_t pressedPad = waitForPressed();        
        if (pressedPad != mSequence[i]);
        {
          mPads[mSequence[i]].flash(1000);
          return false;     
        }
        return true;
      }   
    }
    
    virtual void incrementStep() 
    {
      mLevel++;  
    }
    
    virtual I_Stage& getStart() {return mStartStage; }
    virtual I_Stage& getShowSequence() {return mShowSequenceStage; }
    virtual I_Stage& getCheckSequence() {return mCheckSequenceStage; }
  
  private:

    unsigned long speed() {return 700 - (mLevel * 10); }

    Pad mPads[NUMBER_OF_PADS];
    uint8_t mSequence[NUMBER_OF_LEVELS];
    StartStage mStartStage;
    ShowSequenceStage mShowSequenceStage;
    CheckSequenceStage mCheckSequenceStage;
    size_t mLevel;
    
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
