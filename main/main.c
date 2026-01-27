#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#define PSEAT_PIN  GPIO_NUM_4       // passenger seat button pin 4
#define DSEAT_PIN  GPIO_NUM_5       // driver seat button pin 5
#define PBELT_PIN  GPIO_NUM_6       // passenger belt switch pin 6
#define DBELT_PIN  GPIO_NUM_7       // driver belt switch pin 7
#define IGNITION_BUTTON  GPIO_NUM_8 // ignition button pin 8
#define READY_LED  GPIO_NUM_10      // ready LED pin 10
#define SUCCESS_LED  GPIO_NUM_11    // success LED pin 11
#define ALARM_PIN GPIO_NUM_12       // alarm pin 12
#define HEADLIGHT_LED GPIO_NUM_13   // headlight LED pin 13

bool dseat = false;  //Detects when the driver is seated 
bool pseat = false;  //Detects when the passenger is seated
bool dbelt = false;  //Detects when the driver seatbelt is on
bool pbelt = false;  //Detects when the passenger seatbelt is on
bool ignition = false; //Detects when the ignition is turned on
int executed = 0; //keep track of print statements
int ready_led = 0; //keep track of whether ready_led should be on or off
int ignition_off = 0; // keep track of whether the ignition can be turned off

void app_main(void)
{
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

    while (1){
        // Task Delay to prevent watchdog
        vTaskDelay(10 / portTICK_PERIOD_MS);

        // initialize variables in relation to GPIO pin inputs
        dseat = gpio_get_level(DSEAT_PIN)==0;
        pseat = gpio_get_level(PSEAT_PIN)==0;
        dbelt = gpio_get_level(DBELT_PIN)==0;
        pbelt = gpio_get_level(PBELT_PIN)==0;
        ignition = gpio_get_level(IGNITION_BUTTON)==0;

        // if the driver seat button is pressed, print the welcome message once
        if (dseat){
            if (executed == 0){
                printf("Welcome to enhanced alarm system model 218-W25 \n"); 
                executed = 1;
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
                executed = 2;
                gpio_set_level(HEADLIGHT_LED, 1);
            }
        }
            
        // otherwise (at least one condition is not satisfied)
        else{
            // set ready LED to OFF and set variable ready_led to 0
            gpio_set_level(READY_LED,0);
            ready_led = 0;
            // if ignition button is pressed while conditions are not satisfied
            if (ignition==true && executed != 2){
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

        if (executed == 2 && ignition == false){
            ignition_off = 1;
        }

        if (ignition_off==1 && ignition == true){
            gpio_set_level(SUCCESS_LED,0);          // turn off ignition
        }
    }
}
