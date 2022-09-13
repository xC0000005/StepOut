// Stepper driver class.

#include "stepconverter.h"

#define SAMPLE_BUFFER_SIZE 512
#define DIR_ON 0x80
#define DIR_IS_ON(x) (x & DIR_ON)
#define WITHOUT_DIR(x) (x & ~DIR_ON)
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


// AVR has a fastio (as does Marlin)
// but there's no reason to go through that 
// when, once setup, our port, pin and mask will
// be fixed. It' also possible to run this on STM32
// Consider using a HAL macro to split AVR.
// test on bluepill with similar results by manually changing
// GPIO interface.
byte get_pin_mask(byte pin) {
    if (pin < 8 )
        return 1<<(pin -1);
    else if (pin < 14)
        return 1<<((pin -8) -1);
    else if (pin < 22)
        return 1<<((pin -14) -1);
    return 0;
}

void StepConverter::init(byte a1_pin, byte b1_pin, byte a2_pin, byte b2_pin, byte enable_pin, byte step_pin, byte dir_pin) {
    this->step_pin = step_pin;
    this->dir_pin = dir_pin;
    this->enable_pin = enable_pin;

    pinMode(a1_pin, INPUT);
    pinMode(b1_pin, INPUT);
    pinMode(a2_pin, INPUT);
    pinMode(b2_pin, INPUT);

    pinMode(step_pin, OUTPUT);
    pinMode(enable_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);

    // Set up our masks.
    this->a1_mask = get_pin_mask(a1_pin);
    this->a2_mask = get_pin_mask(a2_pin);
    this->b1_mask = get_pin_mask(b1_pin);
    this->b2_mask = get_pin_mask(b2_pin);

    this->coil_mask = this->a1_mask | this->a2_mask | this->b1_mask | this->b2_mask;

    // settup pattern buffers
    // The patterns for forward motion (on my printer) are:
    this->forward_pattern[0] = this->a1_mask | this->a2_mask;                  //  A1    A2   
    this->forward_pattern[1] = this->a1_mask | this->a2_mask | this->b2_mask;  //  A1    A2 B2
    this->forward_pattern[2] = this->a1_mask | this->b2_mask;                  //  A1       B2
    this->forward_pattern[3] = this->a1_mask | this->b1_mask | this->b2_mask;  //  A1 B1    B2
    this->forward_pattern[4] = this->b1_mask | this->b2_mask;                  //     B1    B2
    this->forward_pattern[5] = this->b1_mask | this->a2_mask | this->b2_mask;  //  B1 A2 B2
    this->forward_pattern[6] = this->b1_mask | this->a2_mask;                  //  B1 A2   
    this->forward_pattern[7] = this->a1_mask | this->b1_mask | this->a2_mask;  //  A1 B1 A2   
}

byte StepConverter::find_dir_and_pattern(byte pattern) {
    for (unsigned int i = 0; i < ARRAY_SIZE(this->forward_pattern); i++) {
        if (this->forward_pattern[0] == pattern) {
            return i;
        }
    }

    for (unsigned int i = 0; i < ARRAY_SIZE(this->reverse_pattern); i++) {
        if (this->reverse_pattern[0] == pattern) {
            return i | DIR_ON;
        }
    }

    return 0; // What to do here?
}

void StepConverter::perform_stepping(byte pin_status)
{
    bool set_output = false;
    bool new_dir = 0;
    bool should_step = false;
    bool new_enabled = pin_status != 0;
    byte new_move_pattern_index;

    // For each change, determine if enable is on - any bits set is on, no bits on is disabled.
    // if disabled, there's no point in the rest.
    if (this->enabled != new_enabled)
        set_output = true;

    // For each step, compare the expected momement pattern to the found one.
    // if it matches, we just need to step.
    bool matches_expected = (pin_status == this->dir ? this->forward_pattern[this->move_pattern_index] : this->reverse_pattern[this->move_pattern_index]);
    if (matches_expected) {
        should_step = true;
        set_output = true;
    }
    else {
        // If there's no match, check direction.
        byte dir_pattern = find_dir_and_pattern(pin_status);
        new_dir = DIR_IS_ON(dir_pattern);
        new_move_pattern_index = WITHOUT_DIR(dir_pattern);
        set_output = true;
        if (new_dir != this->dir)
        {
            should_step = true;
        }
    }

    // set the output. The delay should be configurable, eventually.
    if (set_output) {
        digitalWrite(this->enable_pin, new_enabled ? HIGH : LOW);
        digitalWrite(this->dir_pin, new_dir ? HIGH : LOW);
        if (should_step) {
            digitalWrite(this->step_pin, HIGH);
            delayMicroseconds(1);
            digitalWrite(this->step_pin, LOW);
        }
    }

    // set our variables
    this->move_pattern_index = new_move_pattern_index;
    this->enabled = new_enabled;
    this->dir = new_dir;
}

// This function should not return, it runs the eternal step loop.
void StepConverter::run_stepper(void) {
    /* note by setting old pin status this way it will always not match on first read */
    byte pin_status, old_pin_status = -1;
    byte coil_mask = this->coil_mask;

    for (;;) {
        pin_status = /* figure out the right port for config */ PIND & coil_mask;

        if (pin_status != old_pin_status) {
            this->perform_stepping(pin_status);
            old_pin_status = pin_status;
        }
    } 
}

// determines the step patterns for an event.
void StepConverter::report_pattern(void) {
    /* note by setting old pin status this way it will always not match on first read */
    byte pin_status, old_pin_status = -1;
    byte coil_mask = this->coil_mask;
    byte sample_buffer[SAMPLE_BUFFER_SIZE] = {0};
    int next_sample = 0;

    // First run the sampling loop
    for (;;) {
        pin_status = /* figure out the right port for config */ PIND & coil_mask;

        if (pin_status != old_pin_status) {
            sample_buffer[next_sample++] = pin_status;
            old_pin_status = pin_status;
        }

        if (next_sample == ARRAY_SIZE(sample_buffer))
        {
            // Report samples
            Serial.println("Samples recorded:");
            for (unsigned int k = 0; k < ARRAY_SIZE(sample_buffer); k++) {
                byte pin_status = sample_buffer[k];
                char message[128];
                sprintf(message, "%s %s %s %s", (pin_status & this->a1_mask) ? "A1" : "  ", (pin_status & b1_mask) ? "B1" : "  ", (pin_status & a2_mask) ? "A2" : "  ", (pin_status & b2_mask) ? "B2" : "  ");
                Serial.println(message);
            }

            Serial.println("Press any key to begin new sample cycle");
            while (!Serial.available()) {};
            (void)Serial.read();
            for (unsigned int i = 0; i < ARRAY_SIZE(sample_buffer); i++)
                sample_buffer[i] = 0;
            next_sample = 0;
        }
    } 
}