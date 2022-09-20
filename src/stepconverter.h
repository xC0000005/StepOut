
#include <arduino.h>

class StepConverter
{
  public: 
    bool enabled;
    bool dir;
    byte move_pattern_index;
    byte coil_mask;
    byte a1_pin;
    byte a2_pin;
    byte b1_pin;
    byte b2_pin;
    byte a1_mask;
    byte a2_mask;
    byte b1_mask;
    byte b2_mask;
    byte step_pin;
    byte enable_pin;
    byte dir_pin;
    byte step_pattern[8];

  public:
    StepConverter() {
      this->dir = 0;
      this->enabled = false;
      this->move_pattern_index = 0;
    };

    // Initialize the step converter
    void init(byte a1_pin, byte b1_pin, byte a2_pin, byte b2_pin, byte enable_pin, byte step_pin, byte dir_pin);
  
    // Runs in a monitoring loop to drive the stepper
    void run_stepper(void);

    // Use to record step patterns that don't match existing.
    void report_pattern(void);
    
    // for diagnosis.
    void monitor_and_step(void);

  private:
    byte get_next_step_pattern(void);
    unsigned short find_dir_and_pattern(byte pattern);
    void perform_stepping(byte pin_status);
};