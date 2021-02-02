#include "wumpus.h"

// Sprite Data
#include "sprites.h"

// Character data for scrolling text

/*
 *      Hardware
 */
// LEDs
const uint8_t pin_green = 19;
const uint8_t pin_blue  = 18;

// Piezo Speaker
const uint8_t pin_speaker = 16;
uint slice_num = 0;

// Joystick
const uint8_t pin_y = 9;
const uint8_t pin_x = 8;
const uint8_t pin_button = 3;
const int deadzone = 100;
const int upper_limit = 512 + deadzone; // 512 == centre
const int lower_limit = 512 - deadzone;

// HT16K33 LED Matrix Commands
const int HT16K33_GENERIC_DISPLAY_ON = 0x81;
const int HT16K33_GENERIC_DISPLAY_OFF = 0x80;
const int HT16K33_GENERIC_SYSTEM_ON = 0x21;
const int HT16K33_GENERIC_SYSTEM_OFF = 0x20;
const int HT16K33_GENERIC_DISPLAY_ADDRESS = 0x00;
const int HT16K33_GENERIC_CMD_BRIGHTNESS = 0xE0;
const int HT16K33_GENERIC_CMD_BLINK = 0x81;
const int HT16K33_ADDRESS = 0x70;

// Wumpus World
char hazards [8][8];
bool visited [8][8];
bool stink_layer[8][8];
bool sound_layer[8][8];
bool draft_layer[8][8];
uint8_t player_x = 0;
uint8_t player_y = 0;
uint8_t last_move = 0;
bool in_play = true;
const char text_win[] = "YOU WIN!      ";
const char text_lose[] = "YOU DIED!      ";

// Graphics buffer
uint8_t buffer[8];

uint8_t test_count = 0;

/*
 * I2C Functions
 */

void i2c_write_byte(uint8_t byte) {
    // Convenience function to write a single byte to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, &byte, 1, false);
}

void i2c_write_block(uint8_t *data, uint8_t count) {
    // Convenience function to write a 'count' bytes to the matrix
    i2c_write_blocking(I2C_PORT, HT16K33_ADDRESS, data, count, false);
}


/*
 * HT16K33 LED Matrix Functions
 */

void ht16k33_init() {
    ht16k33_power(ON);
}

void ht16k33_power(uint8_t on) {
    i2c_write_byte(on == ON ? HT16K33_GENERIC_SYSTEM_ON : HT16K33_GENERIC_DISPLAY_OFF);
    i2c_write_byte(on == ON ? HT16K33_GENERIC_DISPLAY_ON : HT16K33_GENERIC_SYSTEM_OFF);
}

void ht16k33_set_brightness(uint8_t brightness) {
    // Set the LED brightness
    if (brightness < 0 || brightness > 15) brightness = 15;
    i2c_write_byte(HT16K33_GENERIC_CMD_BRIGHTNESS | brightness);
}

void ht16k33_clear() {
    // Clear the display buffer
    for (uint8_t i = 0 ; i < 8 ; ++i) buffer[i] = 0;
}

void ht16k33_draw_sprite(const char *sprite) {
    // Write the sprite across the matrix
    // NOTE Assumes the sprite is 8 pixels wide
    for (uint8_t i = 0 ; i < 8 ; ++i) buffer[i] = sprite[i];

    // Send the buffer to the LED matrix
    ht16k33_draw();
}

void ht16k33_draw_sprite_center(const char *sprite) {
    // Write the sprite centred on the screen
    uint8_t width = strlen(sprite);
    uint8_t col = 8 - width;
    col = col >> 1;
    for (uint8_t i = col ; i < width ; ++i) buffer[i] = sprite[i];

    // Send the buffer to the LED matrix
    ht16k33_draw();
}

void ht16k33_plot(uint8_t x, uint8_t y, bool is_set) {
    // Set or unset the specified pixel
    uint8_t col = buffer[x];

    if (is_set) {
        col = col | (1 << y);
    } else {
        col = col & ~(1 << y);
    }

    buffer[x] = col;
}

void ht16k33_draw() {
    // Set up the buffer holding the data to be
    // transmitted to the LED
    uint8_t output_buffer[17];
    for (uint8_t i = 0 ; i < 17 ; ++i) output_buffer[i] = 0;

    // Span the 8 bytes of the graphics buffer
    // across the 16 bytes of the LED's buffer
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        uint8_t a = buffer[i];
        output_buffer[i * 2 + 1] = (a >> 1) + ((a << 7) & 0xFF);
    }

    // Write out the transmit buffer
    i2c_write_block(output_buffer, sizeof(output_buffer));
}

void ht16k33_print(const char *text) {
    // Scroll the supplied text horizontally across the 8x8 matrix

    // Get the length of the text: the number of columns it encompasses
    uint length = 0;
    for (size_t i = 0 ; i < strlen(text) ; ++i) {
        uint8_t asc_val = text[i] - 32;
        length += (asc_val == 0 ? 2: strlen(CHARSET[asc_val]));
        if (asc_val > 0) length++;
    }

    // Make the output buffer to match the required number of columns
    uint8_t src_buffer[length];
    for (uint i = 0 ; i < length ; ++i) src_buffer[i] = 0x00;

    // Write each character's glyph columns into the output buffer
    uint col = 0;
    for (size_t i = 0 ; i < strlen(text) ; ++i) {
        uint8_t asc_val = text[i] - 32;
        if (asc_val == 0) {
            // It's a space, so just add two blank columns
            col += 2;
        } else {
            // Get the character glyph and write it to the buffer
            uint8_t glyph_len = strlen(CHARSET[asc_val]);

            for (uint j = 0 ; j < glyph_len ; ++j) {
                src_buffer[col] = CHARSET[asc_val][j];
                ++col;
            }

            ++col;
        }
    }

    // Finally, animate the line by repeatedly sending 8 columns
    // of the output buffer to the matrix
    uint cursor = 0;
    while (1) {
        uint a = cursor;
        for (uint8_t i = 0 ; i < 8 ; ++i) {
            buffer[i] = src_buffer[a];
            a += 1;
        }

        ht16k33_draw();
        cursor++;
        if (cursor > length - 8) break;
        sleep_ms(100);
    };
}


int irandom(int start, int max) {
    // Generate a PRG between start and max
    return rand() % max + start;
}

bool digitalRead(uint8_t pin) {
    return true;
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {

    unsigned long start = time_us_64();
    while (time_us_64() < start + duration) {
        gpio_put(pin_speaker, true);
        sleep_us(100);
        gpio_put(pin_speaker, false);
        sleep_us(900);
    };

    return;

    uint c = frequency;
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, c);

    // Start the PWM
    pwm_init(slice_num, &config, true);

    // Hold for specified duration
    sleep_ms(duration);

    // Stop PWM
    pwm_set_enabled(slice_num, false);
}


/*
 *      Initiazation Functions
 */
void setup() {
    // Set up the game hardware

    // Set up I2C
    i2c_init(I2C_PORT, I2C_FREQUENCY);
    gpio_set_function(SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_GPIO);
    gpio_pull_up(SCL_GPIO);

    // This example will use I2C0 on GPIO4 (SDA) and GPIO5 (SCL)
    ht16k33_init();
    ht16k33_set_brightness(2);
    ht16k33_clear();
    ht16k33_draw();

    // Set up sense indicator output pins
    gpio_init(pin_green);
    gpio_set_dir(pin_green, GPIO_OUT);
    gpio_put(pin_green, false);

    gpio_init(pin_blue);
    gpio_set_dir(pin_blue, GPIO_OUT);
    gpio_put(pin_blue, false);

    gpio_init(pin_speaker);
    gpio_set_dir(pin_speaker, GPIO_OUT);
    gpio_put(pin_speaker, false);

    // Set up the speaker pin for PWM
    /*
    gpio_set_function(pin_speaker, GPIO_FUNC_PWM);
    slice_num = pwm_gpio_to_slice_num(pin_speaker);
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 128);
    */

    //pinMode(pin_x, INPUT_PULLUP);
    //pinMode(pin_y, INPUT_PULLUP);
    //pinMode(pin_button, INPUT_PULLUP);

    // Use unused analog pin for a floating signal
    // to seed random
    //srand(analogRead(A4));
}

void create_world() {
    // Generate the Wumpus' cave.

    // Initialize world
    for (uint8_t i = 0 ; i < 8 ; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            hazards[i][j] = '#';    // No hazard here
        }
    }
    // Create 1-3 bats
    uint8_t bat_x = 0;
    uint8_t bat_y = 0;
    uint8_t number_bats = irandom(1, 4);
    for (uint8_t i = 0; i < number_bats ; ++i) {
        bat_x = irandom(0, 8);
        bat_y = irandom(0, 8);
        hazards[bat_x][bat_y] = 'b';
    }

    // Create 4-8 pits
    uint8_t pit_x = 0;
    uint8_t pit_y = 0;
    uint8_t number_pits = irandom(4, 9);

    for (uint8_t i = 0; i < number_pits; ++i) {
        pit_x = irandom(0, 8);
        pit_y = irandom(0, 8);
        hazards[pit_x][pit_y] = 'p';
    }

    // Create one wumpus
    // NOTE It's generated last so bats and pits
    //      can't overwrite it by chance
    uint8_t wumpus_x = 0;
    uint8_t wumpus_y = 0;
    while (wumpus_x == 0 && wumpus_y == 0) {
        wumpus_x = irandom(0, 8);
        wumpus_y = irandom(0, 8);
    }

    hazards[wumpus_x][wumpus_y] = 'w';

    // Make sure the start tile is safe to spawn on
    hazards[0][0] = '#';

    // Generate sense data for sounds and LED reactions
    for (uint8_t i = 0; i < 8; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            if (hazards[i][j] == 'w') {
                if (i < 7) stink_layer[i + 1][j] = 1;
                if (i > 0) stink_layer[i - 1][j] = 1;
                if (j < 7) stink_layer[i][j + 1] = 1;
                if (j > 0) stink_layer[i][j - 1] = 1;
            } else if (hazards[i][j] == 'p') {
                if (i < 7) draft_layer[i + 1][j] = 1;
                if (i > 0) draft_layer[i - 1][j] = 1;
                if (j < 7) draft_layer[i][j + 1] = 1;
                if (j > 0) draft_layer[i][j - 1] = 1;
            } else if (hazards[i][j] == 'b') {
                if (i < 7) sound_layer[i + 1][j] = 1;
                if (i > 0) sound_layer[i - 1][j] = 1;
                if (j < 7) sound_layer[i][j + 1] = 1;
                if (j > 0) sound_layer[i][j - 1] = 1;
            }
        }
    }
}


/*
 *      Main Game Loop
 */
void game_loop() {
    // Read the current joystick position.
    // If it's not in the deadzone, then determine
    // which direction it's in (up, down, left or right).
    // If it's in the deadzone, check if the player is trying
    // to fire an arrow.

    while (in_play == true) {
        // Read joystick analog output
        uint8_t x = 0; // analogRead(pin_x);
        uint8_t y = 0; // analogRead(pin_y);

        if (check_joystick(x, y)) {
            // Joystick in a direction
            uint8_t dir = get_direction(x, y);

            if (dir == 0) {
                // Move player up
                if (player_y < 7) {
                    player_y++;
                    last_move = 0;
                }
            } else if (dir == 1) {
                // Move player right
                if (player_x < 7) {
                    player_x++;
                    last_move = 1;
                }
            } else if (dir == 2) {
                // Move player down
                if (player_y > 0) {
                    player_y--;
                    last_move = 2;
                }
            } else {
                // Move player left
                if (player_x > 0) {
                    player_x--;
                    last_move = 3;
                }
            }

            // Record the player's ste[s]
            visited[player_x][player_y] = 1;
        } else {
            // Joystick is in deadzone
            if (!digitalRead(pin_button)) {
                // Shoot arrow
                fire_arrow();

                if (last_move == 0) {
                    if (player_y < 7) {
                        if (hazards[player_x][player_y + 1] == 'w') {
                            kill_wumpus();
                        } else {
                            miss();
                        }
                    }
                } else if (last_move == 1) {
                    if (player_x < 7) {
                        if (hazards[player_x + 1][player_y] == 'w') {
                            kill_wumpus();
                        } else {
                            miss();
                        }
                    }
                } else if (last_move == 2) {
                    if (player_y > 0) {
                        if (hazards[player_x][player_y - 1] == 'w') {
                            kill_wumpus();
                        } else {
                            miss();
                        }
                    }
                } else {
                    if (player_x > 0) {
                        if (hazards[player_x - 1][player_y] == 'w') {
                            kill_wumpus();
                        } else {
                            miss();
                        }
                    }
                }
            }
        }

        // Draw the world then check for smells and hazards
        draw_world();
        sense();
        check_hazard();
    }
}


/*
 *      Environment Functions
 */
void check_hazard() {
    // Check to see if player has run into a bat, pit or Wumpus

    if (hazards[player_x][player_y] == 'b') {
        // Player encountered a bat
        // Play bat animation and set player somewhere random
        grabbed_by_bat();

        // Drop the player at random
        // NOTE May be on another hazard
        player_x = irandom(0, 8);
        player_y = irandom(0, 8);
    } else if (hazards[player_x][player_y] == 'p') {
        // Player fell down a pit -- death!
        plunged_into_pit();

        for (uint i = 0 ; i < 1000 ; ++i) {
            tone(pin_speaker, irandom(200, 1500), 1);
        }

        game_over();
    } else if (hazards[player_x][player_y] == 'w') {
        // Player ran into the Wumpus!
        for (uint8_t j = 0 ; j < 3 ; ++j) {
            wumpus_end();
        }
    }
}

void sense() {
    // Present the environment to the player

    // Set the smell and draft LEDs
    gpio_put(pin_blue, draft_layer[player_x][player_y]);
    gpio_put(pin_green, stink_layer[player_x][player_y]);

    // Play a sound
    if (sound_layer[player_x][player_y]) {
        tone(pin_speaker, 6000, 50);
        sleep_ms(50);
        tone(pin_speaker, 5000, 50);
        sleep_ms(50);
        tone(pin_speaker, 4000, 50);
    }
}

void draw_world() {
    // Draw the world on the 8x8 LED matrix
    // and blink the player's location.

    ht16k33_clear();

    // Draw out the world
    for(uint8_t i = 0 ; i < 8 ; ++i) {
        for (uint8_t j = 0 ; j < 8 ; ++j) {
            ht16k33_plot(i, j, visited[i][7 - j]);
        }
    }

    // Flash the player's location
    ht16k33_plot(player_x, 7 - player_y, true);
    ht16k33_draw();
    sleep_ms(500);

    ht16k33_plot(player_x, 7 - player_y, false);
    ht16k33_draw();
    sleep_ms(500);

    test_count++;
    if (test_count == 8) {
        test_count == 0;
        game_over();
    }
}


/*
 *      Animations
 */
void grabbed_by_bat() {
    // Show the bat then the carried animations

    for (uint8_t i = 0 ; i < 6 ; ++i) {
        ht16k33_draw_sprite(bat_1);
        sleep_ms(100);
        ht16k33_draw_sprite(bat_2);
        sleep_ms(100);
    }

    // Play carry animation
    ht16k33_draw_sprite(carry_1);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_2);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_3);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_4);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_5);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_6);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_7);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_8);
    sleep_ms(100);
    ht16k33_draw_sprite(carry_9);
    sleep_ms(100);
}

void plunged_into_pit() {
    ht16k33_draw_sprite(fall_1);
    tone(pin_speaker, 3000, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_2);
    tone(pin_speaker, 2900, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_3);
    tone(pin_speaker, 2800, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_4);
    tone(pin_speaker, 2700, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_5);
    tone(pin_speaker, 2600, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_6);
    tone(pin_speaker, 2500, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_7);
    tone(pin_speaker, 2400, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_8);
    tone(pin_speaker, 2300, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_9);
    tone(pin_speaker, 2200, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_10);
    tone(pin_speaker, 2100, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_11);
    tone(pin_speaker, 2000, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_12);
    tone(pin_speaker, 1900, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_13);
    tone(pin_speaker, 1800, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_14);
    tone(pin_speaker, 1700, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_15);
    tone(pin_speaker, 1600, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_16);
    tone(pin_speaker, 1500, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(fall_17);
    tone(pin_speaker, 1400, 100);
    sleep_ms(100);
}

/*
 *  Movement control functions
 */
bool check_joystick(int x, int y) {
    // Check to see if joystick is outside of the deadzone

    if (x > upper_limit || x < lower_limit || y > upper_limit || y < lower_limit) {
        return true;
    }

    return false;
}

uint8_t get_direction(int x, int y) {
    // Get player direction from the analog input
    //   0 is up
    //   1 is right
    //   2 is down
    //   3 is left

    if (x < y) {
        if (x > (1024 - y)) {
            return 0;
        } else {
            return 3;
        }
    } else {
        if (x > (1024 - y)) {
            return 1;
        } else {
            return 2;
        }
    }
}


/*
 *      Attack the Wumpus
 */
void fire_arrow(){
    // Attempt to kill the Wumpus

    // Show arrow firing animation
    sleep_ms(500);
    ht16k33_draw_sprite(bow_1);
    tone(pin_speaker, 100, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(bow_2);
    tone(pin_speaker, 200, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(bow_3);
    tone(pin_speaker, 300, 100);
    sleep_ms(1000);
    ht16k33_draw_sprite(bow_2);

    for (uint8_t i = 0 ; i < 50 ; ++i) {
        tone(pin_speaker, irandom(200, 1500), 1);
        sleep_ms(1);
    }

    ht16k33_draw_sprite(bow_1);

    for (uint8_t i = 0 ; i < 25 ; ++i) {
        tone(pin_speaker, irandom(200, 1500), 1);
        sleep_ms(1);
    }

    ht16k33_draw_sprite(bow_4);
    sleep_ms(50);
    ht16k33_draw_sprite(bow_5);
    sleep_ms(100);
}

void miss() {
    // If the player misses the Wumpus,
    // the wumpus finds them.

    ht16k33_clear();
    sleep_ms(1000);
    ht16k33_plot(1, 6, true);
    ht16k33_draw();
    tone(pin_speaker, 80, 100);
    sleep_ms(1000);
    ht16k33_plot(1, 6, false);
    ht16k33_plot(3, 6, true);
    ht16k33_draw();
    tone(pin_speaker, 80, 100);
    sleep_ms(1000);
    ht16k33_plot(3, 6, false);
    ht16k33_plot(5, 6, true);
    ht16k33_draw();
    tone(pin_speaker, 80, 100);
    sleep_ms(1000);

    // The End
    wumpus_end();
    game_over();
}

void kill_wumpus(){
    // The player successfully kills the Wumpus!

    sleep_ms(500);
    ht16k33_draw_sprite(wumpus_1);
    sleep_ms(500);
    ht16k33_draw_sprite(wumpus_3);
    tone(pin_speaker, 900, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_4);
    tone(pin_speaker, 850, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_5);
    tone(pin_speaker, 800, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_6);
    tone(pin_speaker, 750, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_7);
    tone(pin_speaker, 700, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_8);
    tone(pin_speaker, 650, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_9);
    tone(pin_speaker, 600, 100);
    sleep_ms(100);
    ht16k33_draw_sprite(wumpus_10);
    tone(pin_speaker, 550, 100);
    sleep_ms(100);
    ht16k33_clear();
    sleep_ms(1000);

    // Success!
    game_win();
}


/*
 *      Game Outcomes
 */
void game_win() {
    // Give the player a trophy!
    // Reset the game after 5 text scrolls.

    ht16k33_draw_sprite(trophy);
    tone(pin_speaker, 1397, 100); //F6
    ht16k33_set_brightness(8);
    sleep_ms(200);
    tone(pin_speaker, 1397, 100); //F6
    ht16k33_set_brightness(1);
    sleep_ms(200);
    tone(pin_speaker, 1397, 100); //F6
    ht16k33_set_brightness(8);
    sleep_ms(200);
    tone(pin_speaker, 1397, 200); //F6
    ht16k33_set_brightness(1);
    sleep_ms(300);
    tone(pin_speaker, 1175, 200); //D6
    ht16k33_set_brightness(8);
    sleep_ms(300);
    tone(pin_speaker, 1319, 200); //E6
    ht16k33_set_brightness(1);
    sleep_ms(300);
    tone(pin_speaker, 1397, 200); //F6
    ht16k33_set_brightness(8);
    sleep_ms(300);
    tone(pin_speaker, 1319, 150); //E6
    ht16k33_set_brightness(1);
    sleep_ms(150);
    tone(pin_speaker, 1397, 400); //F6
    ht16k33_set_brightness(8);
    sleep_ms(300);
    ht16k33_set_brightness(1);
    sleep_ms(300);
    ht16k33_set_brightness(8);
    sleep_ms(300);
    ht16k33_set_brightness(1);
    sleep_ms(300);
    ht16k33_set_brightness(8);
    sleep_ms(300);
    ht16k33_set_brightness(1);
    sleep_ms(1000);

    for(uint8_t i = 0 ; i < 5 ; ++i) {
        ht16k33_print(text_win);
        sleep_ms(100);
    }

    reset_func();
}

void wumpus_end() {
    // Player gets attacked from the vicious Wumpus!
    // Complete with nightmare-inducing sound
    for (uint8_t j = 0 ; j < 3 ; ++j) {
        ht16k33_draw_sprite(wumpus_1);
        for (uint i = 2000 ; i > 800 ; i -= 2) {
            tone(pin_speaker, i, 1);
            sleep_ms(1);
        }

        ht16k33_draw_sprite(wumpus_2);
        for (uint i = 800 ; i < 2000 ; i += 2) {
            tone(pin_speaker, i, 1);
            sleep_ms(1);
        }
    }

    sleep_ms(500);
    game_over();
}

void game_over(){
    // Give the player a funeral.

    ht16k33_clear();
    ht16k33_draw_sprite(grave);
    sleep_ms(500);
    tone(pin_speaker, 294, 400);
    sleep_ms(500);
    tone(pin_speaker, 294, 400);
    sleep_ms(500);
    tone(pin_speaker, 294, 100);
    sleep_ms(200);
    tone(pin_speaker, 294, 400);
    sleep_ms(500);
    tone(pin_speaker, 349, 400);
    sleep_ms(500);
    tone(pin_speaker, 330, 100);
    sleep_ms(200);
    tone(pin_speaker, 330, 400);
    sleep_ms(500);
    tone(pin_speaker, 294, 100);
    sleep_ms(200);
    tone(pin_speaker, 294, 400);
    sleep_ms(500);
    tone(pin_speaker, 294, 100);
    sleep_ms(200);
    tone(pin_speaker, 294, 800);
    sleep_ms(3000);
    ht16k33_clear();
    ht16k33_draw();

    for (uint8_t i = 0 ; i < 5 ; ++i) {
        ht16k33_print(text_lose);
        sleep_ms(100);
    }

    reset_func();
}


/*
 *  The Game's Introduction
 */
void play_intro_theme(){
    // Callback to the theme played in the
    // version by Gregory Yob in 1975.
    // Also show the player entering the cave.

    //D3
    tone(pin_speaker, 147, 200);
    ht16k33_draw_sprite(begin_1);
    sleep_ms(200);
    //E3
    tone(pin_speaker, 165, 200);
    ht16k33_draw_sprite(begin_2);
    sleep_ms(200);
    //F3
    tone(pin_speaker, 175, 200);
    ht16k33_draw_sprite(begin_3);
    sleep_ms(200);
    //G3
    tone(pin_speaker, 196, 200);
    ht16k33_draw_sprite(begin_4);
    sleep_ms(200);
    //A4
    tone(pin_speaker, 220, 200);
    ht16k33_draw_sprite(begin_5);
    sleep_ms(200);
    //F3
    tone(pin_speaker, 175, 200);
    ht16k33_draw_sprite(begin_6);
    sleep_ms(200);
    //A4
    tone(pin_speaker, 220, 400);
    ht16k33_draw_sprite(begin_7);
    sleep_ms(400);
    //G#3
    tone(pin_speaker, 208, 200);
    ht16k33_draw_sprite(begin_4);
    sleep_ms(200);
    //E#3
    tone(pin_speaker, 175, 200);
    sleep_ms(200);
    //G#3
    tone(pin_speaker, 208, 400);
    sleep_ms(400);
    //G3
    tone(pin_speaker, 196, 200);
    sleep_ms(200);
    //E3
    tone(pin_speaker, 165, 200);
    sleep_ms(200);
    //G3
    tone(pin_speaker, 196, 400);
    sleep_ms(400);

    //D3
    tone(pin_speaker, 147, 200);
    sleep_ms(200);
    //E3
    tone(pin_speaker, 165, 200);
    sleep_ms(200);
    //F3
    tone(pin_speaker, 175, 200);
    sleep_ms(200);
    //G3
    tone(pin_speaker, 196, 200);
    sleep_ms(200);
    //A3
    tone(pin_speaker, 220, 200);
    sleep_ms(200);
    //F3
    tone(pin_speaker, 175, 200);
    sleep_ms(200);
    //A3
    tone(pin_speaker, 220, 200);
    sleep_ms(200);
    //D4
    tone(pin_speaker, 294, 200);
    sleep_ms(200);
    //C4
    tone(pin_speaker, 262, 200);
    sleep_ms(200);
    //A3
    tone(pin_speaker, 220, 200);
    sleep_ms(200);
    //F3
    tone(pin_speaker, 175, 200);
    sleep_ms(200);
    //A3
    tone(pin_speaker, 220, 200);
    sleep_ms(200);
    //C4
    tone(pin_speaker, 262, 400);
    sleep_ms(400);
}


/*
 * Runtime start
 */

int main() {
#ifdef TSDEBUG
    // DEBUG control
    stdio_init_all();
#endif

    // Setup the hardware
    setup();

    // Play the game
    while (1) {
        // Set up a new round...
        // Play Wumpus tune
        play_intro_theme();

        // Set up the environment
        create_world();
        draw_world();

        // ...and start play
        game_loop();
    }

    return 0;
}