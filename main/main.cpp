#include <cstdio>
#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "driver/gpio.h"

#include "esp_random.h"


// Defining Outputs
constexpr gpio_num_t LED_READY = GPIO_NUM_5;
constexpr gpio_num_t LED_SET = GPIO_NUM_6;
constexpr gpio_num_t LED_GO = GPIO_NUM_15;

constexpr gpio_num_t BUZZER = GPIO_NUM_11;

constexpr gpio_num_t BUTTON = GPIO_NUM_8;


// State Declaration and Enumeration
enum class State {
    WAIT,
    PREPARE,
    START,
    FAIL,
};

State currentState = State::WAIT;



// Function for Buzzing Sound *NOT USED
void buzzer_sound(int time) {
    gpio_set_level(BUZZER, 1);
    vTaskDelay(pdMS_TO_TICKS(time));
    gpio_set_level(BUZZER, 0);
}

// Function to return a randon int using ESP_RANDOM()
int random_int(int min, int max) {
    return min + (esp_random() % (max - min + 1));
    //  (max - min + 1) counts how many possible values
    // are in a range; INCLUSIVE.
    // esp_random() % [# of values in range] allows for a value
    // from 0 to the number of values in the range. 
    // min + [value from 0 to number of vals in range] sets the lower
    // bound as min and the upper bound as max
}

// For LED Debugging
void led_test() {
    gpio_set_level(LED_READY, 1);
    gpio_set_level(LED_SET, 0);
    gpio_set_level(LED_GO, 0);
    
    vTaskDelay(pdMS_TO_TICKS(5000));


    gpio_set_level(LED_READY, 0);
    gpio_set_level(LED_SET, 1);
    gpio_set_level(LED_GO, 0);

    vTaskDelay(pdMS_TO_TICKS(3000));

    gpio_set_level(LED_READY, 0);
    gpio_set_level(LED_SET, 0);
    gpio_set_level(LED_GO, 1);

    vTaskDelay(pdMS_TO_TICKS(3000));
}

// Function for setting up the pins
void setup_pins() {
    gpio_set_direction(LED_READY, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_SET, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GO, GPIO_MODE_OUTPUT);

    gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);

    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLDOWN_ONLY);
}


// The main game function
void led_reaction() {
    while(true) {
        switch (currentState) {
            case State::WAIT: {
                // Static variables are local variables that are remembered
                // between calls or loop iterations.
                static bool ledsOn = false;
                // TickType_t is FreeRTOS's type for storing time in ticks
                static TickType_t lastToggle = 0;

                // xTaskGetTickCount returns how many ticks have
                // elapsed since the ESP32 started.
                TickType_t now = xTaskGetTickCount();

                // Check the button every loop
                if(gpio_get_level(BUTTON)) {
                    vTaskDelay(pdMS_TO_TICKS(1000));

                    gpio_set_level(LED_READY, 0);
                    gpio_set_level(LED_SET, 0);
                    gpio_set_level(LED_GO, 0);

                    vTaskDelay(pdMS_TO_TICKS(500));
                    
                    currentState = State::PREPARE;
                    break;
                }
                
                // Comparing the last toggle to last instance of now.
                // Now is set every 10ms and lastToggle is set every 500ms.
                if (now - lastToggle >= pdMS_TO_TICKS(500)) {
                    ledsOn = !ledsOn;

                    gpio_set_level(LED_READY, ledsOn);
                    gpio_set_level(LED_SET, ledsOn);
                    gpio_set_level(LED_GO, ledsOn);

                    lastToggle = now;
                }
                vTaskDelay(pdMS_TO_TICKS(10));

            break;
            }
            case State::PREPARE: {

                static int stage = 0;
                static TickType_t lastToggle = 0;
                TickType_t now = xTaskGetTickCount();

                if(gpio_get_level(BUTTON)) {
                    stage = 0;
                    lastToggle = xTaskGetTickCount();
                    currentState = State::FAIL;
                    break;
                }

                // check every 2000ms
                if (now - lastToggle >= pdMS_TO_TICKS(2000)) {
                    if (stage == 0) {
                        gpio_set_level(LED_READY, 1);
                        gpio_set_level(LED_SET, 1);
                        gpio_set_level(LED_GO, 1);

                        vTaskDelay(pdMS_TO_TICKS(1000));

                        gpio_set_level(LED_READY, 0);
                        gpio_set_level(LED_SET, 0);
                        gpio_set_level(LED_GO, 0); 
                    }
                    else if (stage == 1) {
                        gpio_set_level(LED_READY, 1);
                        gpio_set_level(LED_SET, 0);
                        gpio_set_level(LED_GO, 0);

                        gpio_set_level(BUZZER, 1);

                        // A way to create a delay while performing other functions
                        for (int wait = 0; wait < 500; wait += 10) {                            
                            if(gpio_get_level(BUTTON)) {
                                stage = 0;
                                lastToggle = xTaskGetTickCount();
                                currentState = State::FAIL;
                                break;
                            }
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }

                        gpio_set_level(BUZZER, 0);
                        
                        if (currentState == State::FAIL) {
                            break;
                        }
                        //buzzer_sound(500);
                    }
                    else if (stage == 2) {
                        gpio_set_level(LED_READY, 0);
                        gpio_set_level(LED_SET, 1);
                        gpio_set_level(LED_GO, 0);

                        gpio_set_level(BUZZER, 1);

                        for (int wait = 0; wait < 500; wait += 10) {
                            
                            if(gpio_get_level(BUTTON)) {
                                stage = 0;
                                lastToggle = xTaskGetTickCount();
                                currentState = State::FAIL;
                                break;
                            }
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }

                        gpio_set_level(BUZZER, 0);
                        
                        if (currentState == State::FAIL) {
                            break;
                        }

                        //buzzer_sound(500);
                    }
                    else if (stage == 3) {
                        int ran_val = random_int(1000, 5000);
                        
                        for (int wait = 0; wait < ran_val; wait += 10) {
                            if(gpio_get_level(BUTTON)) {
                                stage = 0;
                                lastToggle = xTaskGetTickCount();
                                currentState = State::FAIL;
                                break;
                            }
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }

                        if (currentState == State::FAIL) {
                            break;
                        }
                
                        //vTaskDelay(pdMS_TO_TICKS(ran_val));

                        gpio_set_level(LED_READY, 0);
                        gpio_set_level(LED_SET, 0);
                        gpio_set_level(LED_GO, 1);

                        stage = 0;
                        currentState = State::START;
                        break;
                    }

                    stage++;
                    lastToggle = xTaskGetTickCount();
                }

                vTaskDelay(pdMS_TO_TICKS(10));

                /*
                // ADD A WAY TO JUMP TO FAIL STATE

                int ran_val = random_int(1000, 5000);

                gpio_set_level(LED_READY, 1);
                gpio_set_level(LED_SET, 1);
                gpio_set_level(LED_GO, 1);

                vTaskDelay(pdMS_TO_TICKS(1000));

                gpio_set_level(LED_READY, 0);
                gpio_set_level(LED_SET, 0);
                gpio_set_level(LED_GO, 0);

                vTaskDelay(pdMS_TO_TICKS(1000));

                gpio_set_level(LED_READY, 1);
                gpio_set_level(LED_SET, 0);
                gpio_set_level(LED_GO, 0);

                buzzer_sound(500);

                vTaskDelay(pdMS_TO_TICKS(1000));

                gpio_set_level(LED_READY, 0);
                gpio_set_level(LED_SET, 1);
                gpio_set_level(LED_GO, 0);

                buzzer_sound(500);

                vTaskDelay(pdMS_TO_TICKS(ran_val));

                gpio_set_level(LED_READY, 0);
                gpio_set_level(LED_SET, 0);
                gpio_set_level(LED_GO, 1);

                currentState = State::START;
                */

            break;
            }
            case State::FAIL: {
                std::printf("Too early!\n");

                vTaskDelay(pdMS_TO_TICKS(200));

                gpio_set_level(LED_READY, 0);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 0);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 1);
                buzzer_sound(100);

                gpio_set_level(LED_READY, 0);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 1);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 0);
                buzzer_sound(100);

                gpio_set_level(LED_READY, 1);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 0);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 0);
                buzzer_sound(100);

                gpio_set_level(LED_READY, 0);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 0);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 1);
                buzzer_sound(100);

                gpio_set_level(LED_READY, 0);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 1);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 0);
                buzzer_sound(100);

                gpio_set_level(LED_READY, 1);
                buzzer_sound(100);
                gpio_set_level(LED_SET, 0);
                buzzer_sound(100);
                gpio_set_level(LED_GO, 0);
                buzzer_sound(100);

                vTaskDelay(pdMS_TO_TICKS(2000));

                currentState = State::WAIT;
            break;
            }
            case State::START: {

                TickType_t timerZero = xTaskGetTickCount();

                int timerMs = 0;

                while(!gpio_get_level(BUTTON)) {
                    TickType_t now = xTaskGetTickCount();
                    
                    timerMs = pdTICKS_TO_MS(now - timerZero);

                    gpio_set_level(BUZZER, 1);

                    vTaskDelay(pdMS_TO_TICKS(5));
                }

                gpio_set_level(BUZZER, 0);

                //int timerSec = timerMs / 1000;

                gpio_set_level(LED_READY, 0);
                gpio_set_level(LED_SET, 0);
                gpio_set_level(LED_GO, 0);

                vTaskDelay(pdMS_TO_TICKS(1000));

                // note that timerMs is an int, and %d is a placeholder for that int
                std::printf("Your time was %d milliseconds\n", timerMs);

                vTaskDelay(pdMS_TO_TICKS(2000));

                currentState = State::WAIT;

            break;
            }
        }
    }
}


extern "C" void app_main(void)
{
    setup_pins(); // Setting up pins

    led_reaction(); // Starting game
}

