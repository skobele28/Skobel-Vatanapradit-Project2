#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

#define PSEAT_PIN  GPIO_NUM_4       // passenger seat button pin 4
#define DSEAT_PIN  GPIO_NUM_5       // driver seat button pin 5
#define PBELT_PIN  GPIO_NUM_6       // passenger belt switch pin 6
#define DBELT_PIN  GPIO_NUM_7       // driver belt switch pin 7
#define IGNITION_BUTTON  GPIO_NUM_8 // ignition button pin 8
#define READY_LED  GPIO_NUM_10      // ready LED pin 10
#define SUCCESS_LED  GPIO_NUM_11    // success LED pin 11
#define ALARM_PIN GPIO_NUM_12       // alarm pin 12
#define HEADLIGHT_LED GPIO_NUM_13   // headlight LED pin 13
#define HIGHBEAM_CONTROL   GPIO_NUM_14  // high beam control (switch) pin 14
#define HIGHBEAM_LED       GPIO_NUM_2   // high beam LED pin 2
#define HEADLIGHT_ADC     ADC_CHANNEL_8 // headlight control (potentiometer) ADC1 channel 8
#define LDR_SENSOR      ADC_CHANNEL_0   // LDR sensor (auto headlight) ADC1 channel 0
#define ADC_ATTEN       ADC_ATTEN_DB_12 // set ADC attenuation
#define BITWIDTH        ADC_BITWIDTH_12 // set ADC bitwidth


bool dseat = false;  //Detects when the driver is seated 
bool pseat = false;  //Detects when the passenger is seated
bool dbelt = false;  //Detects when the driver seatbelt is on
bool pbelt = false;  //Detects when the passenger seatbelt is on
bool ignition = false; //Detects when the ignition is turned on
int executed = 0; //keep track of print statements
int ready_led = 0; //keep track of whether ready_led should be on or off
int ignition_off = 0; // keep track of whether the ignition can be turned off
int lowbeam = 0;    // keep track of low beam status
int highbeam = 0;   // keep track of high beam control

void app_main(void)
{

    int adc_bits;                       // potentiometer ADC reading (bits)
    int adc_mV;                         // potentiometer ADC reading (mV)
    int ldr_adc_bits;                   // LDR ADC reading (bits)
    int ldr_adc_mV;                     // LDR ADC reading (mV)


    // set driver seat pin config to input and internal pullup
    gpio_reset_pin(DSEAT_PIN);
    gpio_set_direction(DSEAT_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(DSEAT_PIN);

    // set passenger seat pin config to input and internal pullup
    gpio_reset_pin(PSEAT_PIN);
    gpio_set_direction(PSEAT_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(PSEAT_PIN);

    // set driver belt pin config to input and internal pullup
    gpio_reset_pin(DBELT_PIN);
    gpio_set_direction(DBELT_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(DBELT_PIN);

    // set passenger belt pin config to input and internal pullup
    gpio_reset_pin(PBELT_PIN);
    gpio_set_direction(PBELT_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(PBELT_PIN);

    // set ignition button config to input and internal pullup
    gpio_reset_pin(IGNITION_BUTTON);
    gpio_set_direction(IGNITION_BUTTON, GPIO_MODE_INPUT);
    gpio_pullup_en(IGNITION_BUTTON);

    // set ready led pin config to output, level 0
    gpio_reset_pin(READY_LED);
    gpio_set_direction(READY_LED, GPIO_MODE_OUTPUT);

    // set success led pin config to output, level 0
    gpio_reset_pin(SUCCESS_LED);
    gpio_set_direction(SUCCESS_LED, GPIO_MODE_OUTPUT);

    // set alarm pin config to output, level 0
    gpio_reset_pin(ALARM_PIN);
    gpio_set_direction(ALARM_PIN, GPIO_MODE_OUTPUT);

    // set headlight pin config to output, level 0
    gpio_reset_pin(HEADLIGHT_LED);
    gpio_set_direction(HEADLIGHT_LED, GPIO_MODE_OUTPUT);

    // set high beam control pin config to input and internal pullup
    gpio_reset_pin(HIGHBEAM_CONTROL);
    gpio_set_direction(HIGHBEAM_CONTROL, GPIO_MODE_INPUT);
    gpio_pullup_en(HIGHBEAM_CONTROL);

    // set high beam led pin config to output, level 0
    gpio_reset_pin(HIGHBEAM_LED);
    gpio_set_direction(HIGHBEAM_LED, GPIO_MODE_OUTPUT);

    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
    };                                                  // Unit configuration
    adc_oneshot_unit_handle_t adc1_handle;              // Unit handle
    adc_oneshot_new_unit(&init_config1, &adc1_handle);  // Populate unit handle

    adc_oneshot_chan_cfg_t config = {
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };                                                  // Channel config
    adc_oneshot_config_channel                          // Configure channel
    (adc1_handle, HEADLIGHT_ADC, &config);

    adc_oneshot_config_channel
    (adc1_handle, LDR_SENSOR, &config);

    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .chan = HEADLIGHT_ADC,
        .atten = ADC_ATTEN,
        .bitwidth = BITWIDTH
    };                                                  // Calibration config
    adc_cali_handle_t adc1_cali_chan_handle;            // Calibration handle
    adc_cali_create_scheme_curve_fitting                // Populate cal handle
    (&cali_config, &adc1_cali_chan_handle);

    while (1){
        
        adc_oneshot_read
        (adc1_handle, HEADLIGHT_ADC, &adc_bits);              // Read ADC bits (potentiometer)
        
        adc_cali_raw_to_voltage
        (adc1_cali_chan_handle, adc_bits, &adc_mV);         // Convert to mV (potentiometer)

        adc_oneshot_read
        (adc1_handle, LDR_SENSOR, &ldr_adc_bits);           // Read ADC bits (LDR)
        
        adc_cali_raw_to_voltage
        (adc1_cali_chan_handle, ldr_adc_bits, &ldr_adc_mV); // Convert to mV (LDR)

        printf("%d\n", ldr_adc_mV);


        // Task Delay to prevent watchdog
        vTaskDelay(10 / portTICK_PERIOD_MS);

        // initialize variables in relation to GPIO pin inputs
        dseat = gpio_get_level(DSEAT_PIN)==0;
        pseat = gpio_get_level(PSEAT_PIN)==0;
        dbelt = gpio_get_level(DBELT_PIN)==0;
        pbelt = gpio_get_level(PBELT_PIN)==0;
        ignition = gpio_get_level(IGNITION_BUTTON)==0;
        highbeam = gpio_get_level(HIGHBEAM_CONTROL)==0;

        // if the driver seat button is pressed, print the welcome message once
        if (dseat){
            if (executed == 0){     // if executed equals 0, print welcome message
                printf("Welcome to enhanced alarm system model 218-W25 \n"); 
                executed = 1;       // set executed = 1 so welcome message only prints once
            }
        }

        // if all of the conditions are met
        if (dseat && pseat && dbelt && pbelt){
            //set ready led to ON
            if (executed == 1 && ready_led == 0){
                gpio_set_level(READY_LED, 1);
                ready_led = 1;
            }
            // if ignition button is pressed while all conditions are met
            if (ignition == true && executed == 1){
                // turn on ignition LED and turn off ready LED
                gpio_set_level(SUCCESS_LED, 1);
                gpio_set_level(READY_LED, 0);
                gpio_set_level(ALARM_PIN, 0);
                // print engine started message once
                printf("Engine started!\n");
                executed = 2;       // set executed = 2 so engine started message only prints once
            }
        }
            
        // otherwise (at least one condition is not satisfied)
        else{
            // set ready LED to OFF and set variable ready_led to 0
            gpio_set_level(READY_LED,0);
            ready_led = 0;
            // if ignition button is pressed while conditions are not satisfied
            if (ignition==true && executed < 2){
                    // turn on alarm buzzer for 5 seconds
                    gpio_set_level(ALARM_PIN, 1);
                    printf("Ignition inhibited.\n");
                    // check which conditions are not met, print corresponding message
                    if (!pseat){
                        printf("Passenger seat not occupied.\n");
                    }
                    if (!dseat){
                        printf("Driver seat not occupied.\n");
                    }
                    if (!pbelt){
                        printf("Passenger seatbelt not fastened.\n");
                    }
                    if (!dbelt){
                        printf("Drivers seatbelt not fastened.\n");
                    }
                
            }
        }

        // if iginition successful, set low beams according to potentiometer
        if(executed == 2){
            // if potentiometer set to off, set low beam leds to off, set lowbeam = 0
            if(adc_mV < 1000){
                gpio_set_level(HEADLIGHT_LED,0);
                // gpio_set_level(HIGHBEAM_LED, 0);
                lowbeam = 0;
            }
            // if potentiometer set to auto, use LDR to determine led high/low
            else if(adc_mV >= 1000 && adc_mV < 2250){
                // if LDR high mV (daylight) for 2s, turn off low beams, set lowbeam = 0
                if (ldr_adc_mV > 2000){
                    vTaskDelay(2000/portTICK_PERIOD_MS);
                    if(ldr_adc_mV > 2000){
                        gpio_set_level(HEADLIGHT_LED, 0);
                        lowbeam = 0;
                    }
                }
                // if LDR low mV (dusk/night) for 1s, turn on low beams, set lowbeam = 1
                else if (ldr_adc_mV < 1550){
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    if (ldr_adc_mV < 1550){
                        gpio_set_level(HEADLIGHT_LED, 1);
                        lowbeam = 1;
                        }
                        
                    }
                }
            // if potentiometer set to on, turn on low beams, set lowbeam = 1
            else if(adc_mV >= 2250){
                gpio_set_level(HEADLIGHT_LED,1);
                lowbeam = 1;
            }
                            
        }

        // if lowbeam on and highbeam switch activated, turn on high beams
        if (lowbeam == 1 && highbeam && executed == 2){
            gpio_set_level(HIGHBEAM_LED, 1);
        }
        // otherwise, turn high beams off
        else{
            gpio_set_level(HIGHBEAM_LED, 0);
        }

        // if ignition is successfully started and then ignition is released, set ignition_off = 1
        if (executed == 2 && ignition == false){
            ignition_off = 1;
        }

        // if ignition_off = 1 and inition is pressed, turn off all LEDs
        if (ignition_off==1 && ignition == true){
            gpio_set_level(SUCCESS_LED,0);          // turn off ignition
            gpio_set_level(HEADLIGHT_LED,0);        // turn off headlight
            gpio_set_level(HIGHBEAM_LED, 0);        // turn off high beams
            executed = 3;                           // set executed = 3 to keep LEDs off
        }
    }
}
