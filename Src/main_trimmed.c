
/* AM32- multi-purpose brushless controller firmware for the stm32f051 */

//===========================================================================
//=============================== Changelog =================================
//===========================================================================
/*
 * 1.54 Changelog;
 * --Added firmware name to targets and firmware version to main
 * --added two more dshot to beacons 1-3 currently working
 * --added KV option to firmware, low rpm power protection is based on KV
 * --start power now controls minimum idle power as well as startup strength.
 * --change default timing to 22.5
 * --Lowered default minimum idle setting to 1.5 percent duty cycle, slider
range from 1-2.
 * --Added dshot commands to save settings and reset ESC.
 *
 *1.56 Changelog.
 * -- added check to stall protection to wait until after 40 zero crosses to fix
high startup throttle hiccup.
 * -- added TIMER 1 update interrupt and PWM changes are done once per pwm
period
 * -- reduce commutation interval averaging length
 * -- reduce false positive filter level to 2 and eliminate threshold where
filter is stopped.
 * -- disable interrupt before sounds
 * -- disable TIM1 interrupt during stepper sinusoidal mode
 * -- add 28us delay for dshot300
 * -- report 0 rpm until the first 10 successful steps.
 * -- move serial ADC telemetry calculations and desync check to 10Khz
interrupt.
 *
 * 1.57
 * -- remove spurious commutations and rpm data at startup by polling for longer
interval on startup
 *
 * 1.58
 * -- move signal timeout to 10khz routine and set armed timeout to one quarter
second 2500 / 10000
 * 1.59
 * -- moved comp order definitions to target.h
 * -- fixed update version number if older than new version
 * -- cleanup, moved all input and output to IO.c
 * -- moved comparator functions to comparator.c
 * -- removed ALOT of useless variables
 * -- added siskin target
 * -- moved pwm changes to 10khz routine
 * -- moved basic functions to functions.c
 * -- moved peripherals setup to periherals.c
 * -- added crawler mode settings
 *
 * 1.60
 * -- added sine mode hysteresis
 * -- increased power in stall protection and lowered start rpm for crawlers
 * -- removed onehot125 from crawler mode
 * -- reduced maximum startup power from 400 to 350
 * -- change minimum duty cycle to DEAD_TIME
 * -- version and name moved to permanent spot in FLASH memory, thanks mikeller
 *
 * 1.61
 * -- moved duty cycle calculation to 10khz and added max change option.
 * -- decreased maximum interval change to 25%
 * -- reduce wait time on fast acceleration (fast_accel)
 * -- added check in interrupt for early zero cross
 *
 * 1.62
 * --moved control to 10khz loop
 * --changed condition for low rpm filter for duty cycle from || to &&
 * --introduced max deceleration and set it to 20ms to go from 100 to 0
 * --added configurable servo throttle ranges
 *
 *
 *1.63
 *-- increase time for zero cross error detection below 250us commutation
interval
 *-- increase max change a low rpm x10
 *-- set low limit of throttle ramp to a lower point and increase upper range
 *-- change desync event from full restart to just lower throttle.

 *1.64
 * --added startup check for continuous high signal, reboot to enter bootloader.
 *-- added brake on stop from eeprom
 *-- added stall protection from eeprom
 *-- added motor pole divider for sinusoidal and low rpm power protection
 *-- fixed dshot commands, added confirmation beeps and removed blocking
behavior
 *--
 *1.65
 *-- Added 32 millisecond telemetry output
 *-- added low voltage cutoff , divider value and cutoff voltage needs to be
added to eeprom
 *-- added beep to indicate cell count if low voltage active
 *-- added current reading on pa3 , conversion factor needs to be added to
eeprom
 *-- fixed servo input capture to only read positive pulse to handle higher
refresh rates.
 *-- disabled oneshot 125.
 *-- extended servo range to match full output range of receivers
 *-- added RC CAR style reverse, proportional brake on first reverse , double
tap to change direction
 *-- added brushed motor control mode
 *-- added settings to EEPROM version 1
 *-- add gimbal control option.
 *--
 *1.66
 *-- move idwg init to after input tune
 *-- remove reset after save command -- dshot
 *-- added wraith32 target
 *-- added average pulse check for signal detection
 *--
 *1.67
 *-- Rework file structure for multiple MCU support
 *-- Add g071 mcu
 *--
 *1.68
 *--increased allowed average pulse length to avoid double startup
 *1.69
 *--removed line re-enabling comparator after disabling.
 *1.70 fix dshot for Kiss FC
 *1.71 fix dshot for Ardupilot / Px4 FC
 *1.72 Fix telemetry output and add 1 second arming.
 *1.73 Fix false arming if no signal. Remove low rpm throttle protection below
300kv *1.74 Add Sine Mode range and drake brake strength adjustment *1.75
Disable brake on stop for PWM_ENABLE_BRIDGE Removed automatic brake on stop on
neutral for RC car proportional brake. Adjust sine speed and stall protection
speed to more closely match makefile fixes from Cruwaller Removed gd32 build,
until firmware is functional *1.76 Adjust g071 PWM frequency, and startup power
to be same frequency as f051. Reduce number of polling back emf checks for g071
 *1.77 increase PWM frequency range to 8-48khz
 *1.78 Fix bluejay tunes frequency and speed.
           Fix g071 Dead time
           Increment eeprom version
 *1.79 Add stick throttle calibration routine
           Add variable for telemetry interval
 *1.80 -Enable Comparator blanking for g071 on timer 1 channel 4
           -add hardware group F for Iflight Blitz
           -adjust parameters for pwm frequency
           -add sine mode power variable and eeprom setting
           -fix telemetry rpm during sine mode
           -fix sounds for extended pwm range
           -Add adjustable braking strength when driving
 *1.81 -Add current limiting PID loop
           -fix current sense scale
           -Increase brake power on maximum reverse ( car mode only)
           -Add HK and Blpwr targets
           -Change low kv motor throttle limit
           -add reverse speed threshold changeover based on motor kv
           -doubled filter length for motors under 900kv
*1.82  -Add speed control pid loop.
*1.83  -Add stall protection pid loop.
           -Improve sine mode transition.
           -decrease speed step re-entering sine mode
           -added fixed duty cycle and speed mode build option
           -added rpm_controlled by input signal ( to be added to config tool )
*1.84  -Change PID value to int for faster calculations
           -Enable two channel brushed motor control for dual motors
           -Add current limit max duty cycle
*1.85  -fix current limit not allowing full rpm on g071 or low pwm frequency
                -remove unused brake on stop conditional
*1.86  - create do-once in sine mode instead of setting pwm mode each time.
*1.87  - fix fixed mode max rpm limits
*1.88  - Fix stutter on sine mode re-entry due to position reset
*1.89  - Fix drive by rpm mode scaling.
           - Fix dshot px4 timings
*1.90  - Disable comp interrupts for brushed mode
           - Re-enter polling mode after prop strike or desync
           - add G071 "N" variant
           - add preliminary Extended Dshot
*1.91  - Reset average interval time on desync only after 100 zero crosses
*1.92  - Move g071 comparator blanking to TIM1 OC5
           - Increase ADC read frequency and current sense filtering
           - Add addressable LED strip for G071 targets
*1.93  - Optimization for build process
       - Add firmware file name to each target hex file
       -fix extended telemetry not activating dshot600
       -fix low voltage cuttoff timeout
*1.94  - Add selectable input types
*1.95  - reduce timeout to 0.5 seconds when armed
*1.96  - Improved erpm accuracy dshot and serial telemetry, thanks Dj-Uran
             - Fix PID loop integral.
                 - add overcurrent low voltage cuttoff to brushed mode.
*1.97    - enable input pullup
*1.98    - Dshot erpm rounding compensation.
*1.99    - Add max duty cycle change to individual targets ( will later become
an settings option)
                 - Fix dshot telemetry delay f4 and e230 mcu
*2.00    - Cleanup of target structure
*2.01    - Increase 10khztimer to 20khz, increase max duty cycle change.
*2.02	 - Increase startup power for inverted output targets.
*2.03    - Move chime from dshot direction change commands to save command.
*2.04    - Fix current protection, max duty cycle not increasing
                 - Fix double startup chime
                 - Change current averaging method for more precision
                 - Fix startup ramp speed adjustment
*2.05		 - Fix ramp tied to input frequency
*2.06    - fix input pullups
         - Remove half xfer insterrupt from servo routine
                                 - update running brake and brake on stop
*2.07    - Dead time change f4a
*2.08		 - Move zero crosss timing
*2.09    - filter out short zero crosses
*2.10    - Polling only below commutation intverval of 1500-2000us
				 - fix tune frequency again
*2.11    - RC-Car mode fix
*2.12    - Reduce Advance on hard braking
*2.13    - Remove Input capture filter for dshot2400
         - Change dshot 300 speed detection threshold 
*2.14    - Reduce G071 zero cross checks
         - Assign all mcu's duty cycle resolution 2000 steps
*2.15    - Enforce 1/2 commutation interval as minimum for g071
         - Revert timing change on braking
				 - Add per target over-ride option to max duty cycle change.
				 - todo fix signal detection
*2.16    - add L431 
				 - add variable auto timing
				 - add droneCAN
*/
#include "main.h"
#include "ADC.h"
#include "IO.h"
#include "common.h"
#include "comparator.h"
#include "dshot.h"
#include "eeprom.h"
#include "functions.h"
#include "peripherals.h"
#include "phaseouts.h"
#include "serial_telemetry.h"
#include "kiss_telemetry.h"
#include "signal.h"
#include "sounds.h"
#include "targets.h"
#include <stdint.h>
#include <string.h>
#include <assert.h>



#include <version.h>

void zcfoundroutine(void);

// firmware build options !! fixed speed and duty cycle modes are not to be used
// with sinusoidal startup !!

//#define FIXED_DUTY_MODE  // bypasses signal input and arming, uses a set duty
// cycle. For pumps, slot cars etc 
//#define FIXED_DUTY_MODE_POWER 100     //
// 0-100 percent not used in fixed speed mode

// #define FIXED_SPEED_MODE  // bypasses input signal and runs at a fixed rpm
// using the speed control loop PID 
//#define FIXED_SPEED_MODE_RPM  1000  //
// intended final rpm , ensure pole pair numbers are entered correctly in config
// tool.

// #define BRUSHED_MODE         // overrides all brushless config settings,
// enables two channels for brushed control 
//#define GIMBAL_MODE     // also
// sinusoidal_startup needs to be on, maps input to sinusoidal angle.

//===========================================================================
//=============================  Defaults =============================
//===========================================================================



EEprom_t eepromBuffer;
volatile uint32_t polling_mode_changeover;
volatile uint8_t ramp_divider;
volatile uint8_t max_ramp_startup = RAMP_SPEED_STARTUP; // 0.0001 ramp per second in 20 kz loop
volatile uint8_t max_ramp_low_rpm = RAMP_SPEED_LOW_RPM; // 0.0003 ramp per second in 20 kz loop
volatile uint8_t max_ramp_high_rpm = RAMP_SPEED_HIGH_RPM; // 0.0008 ramp per second in 20 kz loop
char send_esc_info_flag;
uint32_t eeprom_address = EEPROM_START_ADD; 
uint16_t prop_brake_duty_cycle = 0;
uint16_t ledcounter = 0;
uint16_t ramp_count;
uint32_t process_time = 0;
uint32_t start_process = 0;
uint16_t one_khz_loop_counter = 0;
uint16_t target_e_com_time_high;
uint16_t target_e_com_time_low;
uint8_t compute_dshot_flag = 0;
uint8_t crsf_input_channel = 1;
uint8_t crsf_output_PWM_channel = 2;
uint8_t telemetry_interval_ms = 30;
uint8_t temp_advance;
uint16_t motor_kv = 2000;
uint8_t dead_time_override = DEAD_TIME;
uint16_t stall_protect_target_interval = TARGET_STALL_PROTECTION_INTERVAL;
uint16_t enter_sine_angle = 180;
char do_once_sinemode = 0;
uint8_t auto_advance_level;

//============================= Servo Settings ==============================
uint16_t servo_low_threshold = 1100; // anything below this point considered 0
uint16_t servo_high_threshold = 1900; // anything above this point considered 2000 (max)
uint16_t servo_neutral = 1500;
uint8_t servo_dead_band = 100;

//========================= Battery Cuttoff Settings ========================
char LOW_VOLTAGE_CUTOFF = 0; // Turn Low Voltage CUTOFF on or off
uint16_t low_cell_volt_cutoff = 330; // 3.3volts per cell

//=========================== END EEPROM Defaults ===========================

const char filename[30] __attribute__((section(".file_name"))) = FILE_NAME;
_Static_assert(sizeof(FIRMWARE_NAME) <=13,"Firmware name too long");   // max 12 character firmware name plus NULL 

// move these to targets folder or peripherals for each mcu
uint16_t ADC_CCR = 30;
uint16_t current_angle = 90;
uint16_t desired_angle = 90;
char return_to_center = 0;
uint16_t target_e_com_time = 0;
int16_t Speed_pid_output;
char use_speed_control_loop = 0;
int32_t input_override = 0;
int16_t use_current_limit_adjust = 2000;
char use_current_limit = 0;
int32_t stall_protection_adjust = 0;
uint32_t MCU_Id = 0;
uint32_t REV_Id = 0;

uint16_t armed_timeout_count;
uint16_t reverse_speed_threshold = 1500;
uint8_t desync_happened = 0;
char maximum_throttle_change_ramp = 1;

char crawler_mode = 0; // no longer used //
uint16_t velocity_count = 0;
uint16_t velocity_count_threshold = 75;

char low_rpm_throttle_limit = 1;

uint16_t low_voltage_count = 0;
uint16_t telem_ms_count;

uint16_t VOLTAGE_DIVIDER = TARGET_VOLTAGE_DIVIDER; // 100k upper and 10k lower resistor in divider
uint16_t
    battery_voltage; // scale in volts * 10.  1260 is a battery voltage of 12.60
char cell_count = 0;
char brushed_direction_set = 0;

uint16_t tenkhzcounter = 0;
int32_t consumed_current = 0;
int32_t smoothed_raw_current = 0;
int16_t actual_current = 0;

char lowkv = 0;

uint16_t min_startup_duty = 120;
uint16_t sin_mode_min_s_d = 120;
char bemf_timeout = 10;

char startup_boost = 50;
char reversing_dead_band = 1;

uint16_t low_pin_count = 0;

uint8_t max_duty_cycle_change = 2;
char fast_accel = 1;
char fast_deccel = 0;
uint16_t last_duty_cycle = 0;
uint16_t duty_cycle_setpoint = 0;
char play_tone_flag = 0;

typedef enum { GPIO_PIN_RESET = 0U,
    GPIO_PIN_SET } GPIO_PinState;

uint16_t startup_max_duty_cycle = 200;
uint16_t minimum_duty_cycle = DEAD_TIME;
uint16_t stall_protect_minimum_duty = DEAD_TIME;
char desync_check = 0;
char low_kv_filter_level = 20;

uint16_t tim1_arr = TIM1_AUTORELOAD; // current auto reset value
uint16_t TIMER1_MAX_ARR = TIM1_AUTORELOAD; // maximum auto reset register value
uint16_t duty_cycle_maximum = 2000; // restricted by temperature or low rpm throttle protect
uint16_t low_rpm_level = 20; // thousand erpm used to set range for throttle resrictions
uint16_t high_rpm_level = 70; //
uint16_t throttle_max_at_low_rpm = 400;
uint16_t throttle_max_at_high_rpm = 2000;

uint16_t commutation_intervals[6] = { 0 };
uint32_t average_interval = 0;
uint32_t last_average_interval;
int e_com_time;

uint16_t ADC_smoothed_input = 0;
uint8_t degrees_celsius;
int16_t converted_degrees;
uint8_t temperature_offset;
uint16_t ADC_raw_temp;
uint16_t ADC_raw_volts;
uint16_t ADC_raw_current;
uint16_t ADC_raw_input;
uint16_t ADC_raw_ntc;
uint8_t PROCESS_ADC_FLAG = 0;
char send_telemetry = 0;
char telemetry_done = 0;
char prop_brake_active = 0;

char dshot_telemetry = 0;

uint8_t last_dshot_command = 0;
char old_routine = 1;
uint16_t adjusted_input = 0;

uint16_t smoothedcurrent = 0;
const uint8_t numReadings = 50; // the readings from the analog input
uint8_t readIndex = 0; // the index of the current reading
uint32_t total = 0;
uint16_t readings[50];

uint8_t bemf_timeout_happened = 0;
uint8_t changeover_step = 5;
uint8_t filter_level = 5;
uint8_t running = 0;
uint16_t advance = 0;
uint8_t advancedivisor = 6;
char rising = 1;

// int sin_divider = 2;
int16_t phase_A_position;
int16_t phase_B_position;
int16_t phase_C_position;
uint16_t step_delay = 100;
char stepper_sine = 0;
char forward = 1;
uint16_t gate_drive_offset = DEAD_TIME;

uint8_t stuckcounter = 0;
uint16_t k_erpm;
uint16_t e_rpm; // electrical revolution /100 so,  123 is 12300 erpm

uint16_t adjusted_duty_cycle;

uint8_t bad_count = 0;
uint8_t bad_count_threshold = CPU_FREQUENCY_MHZ / 24;
uint8_t dshotcommand;
uint16_t armed_count_threshold = 1000;

char armed = 0;
uint16_t zero_input_count = 0;

uint16_t input = 0;
uint16_t newinput = 0;
char inputSet = 0;
char dshot = 0;
char servoPwm = 0;
uint32_t zero_crosses;

uint8_t zcfound = 0;

uint8_t bemfcounter;
uint8_t min_bemf_counts_up = TARGET_MIN_BEMF_COUNTS;
uint8_t min_bemf_counts_down = TARGET_MIN_BEMF_COUNTS;

uint16_t lastzctime;
uint16_t thiszctime;

uint16_t duty_cycle = 0; // range from 0-2000
char step = 1;
uint32_t commutation_interval = 12500;
uint16_t waitTime = 0;
uint16_t signaltimeout = 0;
uint8_t ubAnalogWatchdogStatus = RESET;



void loadEEpromSettings()
{
    if(eepromBuffer.eeprom_version < EEPROM_VERSION){
      
        // max_ramp is later divided by 10 so this is effectively 16
        eepromBuffer.max_ramp = 160;    // 0.1% per ms to 25% per ms 
        // Also set to default 4 in DroneCAN.c
        eepromBuffer.minimum_duty_cycle = 1; // 0.2% to 51 percent


    }



    if(eepromBuffer.minimum_duty_cycle < 51 && eepromBuffer.minimum_duty_cycle > 0){
        minimum_duty_cycle = eepromBuffer.minimum_duty_cycle * 10;
    }else{
        minimum_duty_cycle = 0;
    }

    if (eepromBuffer.startup_power < 151 && eepromBuffer.startup_power > 49) {
        min_startup_duty = minimum_duty_cycle + eepromBuffer.startup_power;
    } else {
        min_startup_duty = minimum_duty_cycle;
    }
    
    startup_max_duty_cycle = minimum_duty_cycle + 400;

    motor_kv = (eepromBuffer.motor_kv * 40) + 20;

    
    if (eepromBuffer.eeprom_version > 0) { // these commands weren't introduced until eeprom version 1.



        if (eepromBuffer.driving_brake_strength == 0 || eepromBuffer.driving_brake_strength > 9) { // motor brake 1-9
            eepromBuffer.driving_brake_strength = 10;
        }

        
        
        if(eepromBuffer.max_ramp < 10){
          ramp_divider = 9;
          max_ramp_startup = eepromBuffer.max_ramp;
          max_ramp_low_rpm = eepromBuffer.max_ramp;
          max_ramp_high_rpm = eepromBuffer.max_ramp;
        }else{
          ramp_divider = 0;
          if((eepromBuffer.max_ramp / 10) < max_ramp_startup){
            max_ramp_startup = eepromBuffer.max_ramp / 10;
          }
          if((eepromBuffer.max_ramp / 10) < max_ramp_low_rpm){
            max_ramp_low_rpm = eepromBuffer.max_ramp / 10;
          }
          if((eepromBuffer.max_ramp / 10) < max_ramp_high_rpm){
            max_ramp_high_rpm = eepromBuffer.max_ramp / 10;
          }
        }
        
        if (motor_kv < 300) {
            low_rpm_throttle_limit = 0;
        }
        low_rpm_level = motor_kv / 100 / (32 / eepromBuffer.motor_poles); // in e rpm
        high_rpm_level = motor_kv / 12 / (32 / eepromBuffer.motor_poles);				
    }

}


void setInput()
{
 
    input = newinput;


    if (input >= 47) {
        if (running == 0) {

            running = 1;
            last_duty_cycle = min_startup_duty;
        }


        duty_cycle_setpoint = map(input, 47, 2047, minimum_duty_cycle, 2000);

    }

    // if (input >= 47 && (zero_crosses < (uint32_t)(30 >> eepromBuffer.stall_protection))) {
    //     if (duty_cycle_setpoint < min_startup_duty) {
    //         duty_cycle_setpoint = min_startup_duty;
    //     }
    //     if (duty_cycle_setpoint > startup_max_duty_cycle) {
    //         duty_cycle_setpoint = startup_max_duty_cycle;
    //     }
    // }

    if (duty_cycle_setpoint > duty_cycle_maximum) {
        duty_cycle_setpoint = duty_cycle_maximum;
    }


}

void tenKhzRoutine()
{ // 20khz as of 2.00 to be renamed
    duty_cycle = duty_cycle_setpoint;
    
    ramp_count++;
    
    
    if (ramp_count > ramp_divider) {
        ramp_count = 0;

        if (zero_crosses < 150 || last_duty_cycle < 150) {
            max_duty_cycle_change = max_ramp_startup;
        } else {
            // rpm < 571.43
            if (average_interval > 500) { // 571.43 rpm if accounting for 14 motor poles
                max_duty_cycle_change = max_ramp_low_rpm;
            } else {
                max_duty_cycle_change = max_ramp_high_rpm;
            }
        }
        
        // duty_cycle - last_duty_cycle will need to be divided by delta time to get duty cycle change per second
        if ((duty_cycle - last_duty_cycle) > max_duty_cycle_change) {
            duty_cycle = last_duty_cycle + max_duty_cycle_change;

        }
        if ((last_duty_cycle - duty_cycle) > max_duty_cycle_change) {
            duty_cycle = last_duty_cycle - max_duty_cycle_change;
        }
    } else {
        duty_cycle = last_duty_cycle;
    }

    if ((armed && running) && input > 47) {

        adjusted_duty_cycle = ((duty_cycle * tim1_arr) / 2000) + 1;

    }


    last_duty_cycle = duty_cycle;

    SET_DUTY_CYCLE_ALL(adjusted_duty_cycle);

}




int main(void)
{

    while (1) {
        e_com_time = ((commutation_intervals[0] + commutation_intervals[1] + commutation_intervals[2] + commutation_intervals[3] + commutation_intervals[4] + commutation_intervals[5]) + 4) >> 1; // COMMUTATION INTERVAL IS 0.5US INCREMENTS


        average_interval = e_com_time / 3;



        e_rpm = (600000 / e_com_time); // in tens of rpm
        k_erpm = e_rpm / 10; // ecom time is time for one electrical revolution in microseconds

        if (low_rpm_throttle_limit) { // some hardware doesn't need this, its on
                                        // by default to keep hardware / motors
                                        // protected but can slow down the response
                                        // in the very low end a little.
            duty_cycle_maximum = map(k_erpm, low_rpm_level, high_rpm_level, throttle_max_at_low_rpm,
                throttle_max_at_high_rpm); // for more performance lower the
                                            // high_rpm_level, set to a
                                            // consvervative number in source.
        }else{
            duty_cycle_maximum = 2000;
        }


    }
}
