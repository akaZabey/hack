/*
COMPILE INFO
if debug uncomment "#define DEBUG_MODE"
if release comment "#define DEBUG_MODE"
use: "g++ -c cracker_2.cpp" 
and
if debug use: "g++ cracker_2.o -o cracker_2 -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system"
if release use: "g++ cracker_2.o -o cracker_2 -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system font_memory.h texture_memory.h sound_buffer_memory.h"
*/

#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "file_commander.h"

//=====================DEBUG  SWITCH=====================//
#define DEBUG_MODE
//-------------------------------------------------------//
#ifndef DEBUG_MODE
#define RELEASE
#else
#define DEBUG
#endif
//=======================================================//

#ifdef RELEASE
//========================SOURCE!========================//
#include "font_memory.h"
#include "texture_memory.h"
#include "sound_buffer_memory.h"
//=======================================================//
#endif

//=======================CONSTANTS=======================//
const size_t hash_expected = 13842028284762129553;
const int PASS_CHECK_NAME_SIZE = 20;
const int POS_1 = 0x101b;
const unsigned char REPLACE_BYTE_1 = 0xeb;
const int POS_2 = 0x101c;
const unsigned char REPLACE_BYTE_2 = 0x51;

#ifdef DEBUG
const int FONT_NAME_SIZE    = 20;
const int TEXTURE_NAME_SIZE    = 20;
const int SOUND_NAME_SIZE    = 20;
#endif

const int COLOR_THEME_R = 150;
const int COLOR_THEME_G = 50;
const int COLOR_THEME_B = 230;
const int COLOR_THEME_DELTA = 25;
const int COLOR_THEME_MAX = 255;

const int BUTTON_UPPER_LEFT_CORNER_X = 60;
const int BUTTON_UPPER_LEFT_CORNER_Y = 330;
const int BUTTON_WIDTH  = 600;
const int BUTTON_HEIGHT = 40;
const int BUTTON_OUTLINE = 10;

const int WINDOW_SIZE = 720;

const int TEXT_SIZE = BUTTON_HEIGHT;

const int SOUND_VOLUME = 10;

const double TIME_TO_WAIT = 1.0;
//=======================================================//

class my_button {
public:
    int is_button_pressed;

    my_button(sf::Font &font);

    void get_press_update(sf::Event &event);

    //void graphic_update();

    int update(int is_loading, const char *text, const sf::Color &fill_color);

    void draw(sf::RenderWindow &window);

    ~my_button();
private:
    sf::Text text;
    sf::RectangleShape rectangle;
};

class my_background {
public:
    my_background(sf::Texture &texture);

    void draw(sf::RenderWindow &window);

    ~my_background();
private:
    sf::RectangleShape background;
};

//===================SOURCE  FUNCTIONS===================//
int source_load(sf::Font &font, sf::Texture &texture, sf::SoundBuffer &sound_buffer);
//=======================================================//

//====================SOUND FUNCTIONS====================//
void my_play(sf::Sound &sound, sf::SoundBuffer &sound_buffer, int sound_volume);
//=======================================================//

//====================TIMER FUNCTIONS====================//
int timer_checker(double control_point, double sec_to_wait);
//=======================================================//

//====================HASH  FUNCTIONS====================//
size_t hash_func(unsigned char *word, size_t word_len);
//=======================================================//

//====================HACK  FUNCTIONS====================//
int crack(FILE *code_in, unsigned char *buf);
//=======================================================//

int main() {
    sf::Font font;
    sf::Texture texture;
    sf::SoundBuffer sound_buffer;
    int source_load_res = 0;
    const char* crash_reports[] = {"",
                                   "Crash report: font load error\n",
                                   "Crash report: texture load error\n",
                                   "Crash report: sound load error\n",
                                   ""};
    if (source_load_res = source_load(font, texture, sound_buffer)) {
        printf("%s\n", crash_reports[source_load_res]);
        return 1;
    }

    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "cracker_v0.2");
    my_button *button = new my_button(font);
    my_background *bg = new my_background(texture);
    sf::Sound sound;
    my_play(sound, sound_buffer, SOUND_VOLUME);

    //====================PRE-LOGIC BLOCK====================//
    int button_mode = 7;
    char pass_check[PASS_CHECK_NAME_SIZE] = "break2";
    FILE *code_in = NULL;
    double control_point = 0.0;
    const char* button_texts[] = {"Loading...",
                                  "Program not found!",
                                  "Cracked.",
                                  "Crack failed!",
                                  "Wrong program!",
                                  "Try again?",
                                  "More RAM required!",
                                  "Crack it.",
                                  "UNEXPECTED_ERROR"};
    //=======================================================//

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else button->get_press_update(event);
        }

        //======================LOGIC BLOCK======================//
        switch (button_mode) {
            case 0: code_in = fopen(pass_check, "r+b");
                    if (!code_in) {
                        button_mode = 1; 
                        control_point = (double) clock() / CLOCKS_PER_SEC;
                    }
                    else {
                        size_t code_in_len = file_len(pass_check);
                        unsigned char *buf = (unsigned char *) calloc(code_in_len + 10, sizeof(unsigned char));
                        if (!buf) {
                            button_mode = 6; 
                            control_point = (double) clock() / CLOCKS_PER_SEC;
                        }
                        else {
                            fread(buf, sizeof(unsigned char), code_in_len, code_in);
                            size_t hash = hash_func(buf, code_in_len);
                            if (hash == hash_expected) {
                                if (crack(code_in, buf)) button_mode = 2;
                                else {
                                    button_mode = 3; 
                                    control_point = (double) clock() / CLOCKS_PER_SEC;
                                }
                            }
                            else {
                                button_mode = 4; 
                                control_point = (double) clock() / CLOCKS_PER_SEC;
                            }
                            free(buf);
                            buf = NULL;
                            fclose(code_in);
                        }
                    }
                    break;
            case 1: if (timer_checker(control_point, TIME_TO_WAIT)) button_mode = 5; break;
            case 2:                                                                  break;
            case 3: if (timer_checker(control_point, TIME_TO_WAIT)) button_mode = 5; break;
            case 4: if (timer_checker(control_point, TIME_TO_WAIT)) button_mode = 5; break;
            case 5: if (button->is_button_pressed) button_mode = 0;                  break;
            case 6: if (timer_checker(control_point, TIME_TO_WAIT)) button_mode = 5; break;
            case 7: if (button->is_button_pressed) button_mode = 0;                  break;
            default: printf("Crash report: undefined button_mode = %d\n", button_mode);
        }
        //=======================================================//

        switch (button_mode) {
            case 0: button->update(1, button_texts[0]/*"Loading..."*/, sf::Color::Red);
                    break;
            case 1: button->update(0, button_texts[1]/*"Program not found!"*/, sf::Color::Red);
                    break;
            case 2: button->update(0, button_texts[2]/*"Cracked."*/, sf::Color::Green);
                    break;
            case 3: button->update(0, button_texts[3]/*"Crack failed!"*/, sf::Color::Red);
                    break;
            case 4: button->update(0, button_texts[4]/*"Wrong program!"*/, sf::Color::Red);
                    break;
            case 5: button->update(0, button_texts[5]/*"Try again?"*/, sf::Color(COLOR_THEME_R, COLOR_THEME_G, COLOR_THEME_B));
                    break;
            case 6: button->update(0, button_texts[6]/*"More RAM required!"*/, sf::Color::Red);
                    break;
            case 7: button->update(0, button_texts[7]/*"Crack it."*/, sf::Color(COLOR_THEME_R, COLOR_THEME_G, COLOR_THEME_B));
                    break;
            default:button->update(0, button_texts[8]/*"UNEXPECTED_ERROR"*/, sf::Color::Red);
        }

        window.clear();
        
        bg->draw(window);
        button->draw(window);

        window.display();
    }
    
    delete bg;
    delete button;
    return 0;
}

my_button::my_button(sf::Font &font) {
    rectangle.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    rectangle.setFillColor(sf::Color(COLOR_THEME_R, COLOR_THEME_G, COLOR_THEME_B));
    rectangle.setOutlineThickness(BUTTON_OUTLINE);
    rectangle.setOutlineColor(sf::Color(COLOR_THEME_R + COLOR_THEME_DELTA, COLOR_THEME_G + COLOR_THEME_DELTA, COLOR_THEME_B + COLOR_THEME_DELTA));
    rectangle.setPosition(BUTTON_UPPER_LEFT_CORNER_X, BUTTON_UPPER_LEFT_CORNER_Y);
    text.setFont(font);
    text.setCharacterSize(TEXT_SIZE);
    text.setFillColor(sf::Color(COLOR_THEME_MAX - COLOR_THEME_R, COLOR_THEME_MAX - COLOR_THEME_G, COLOR_THEME_MAX - COLOR_THEME_B));
    text.setPosition(BUTTON_UPPER_LEFT_CORNER_X + BUTTON_OUTLINE, BUTTON_UPPER_LEFT_CORNER_Y - BUTTON_OUTLINE / 2);
    is_button_pressed = 0;
}

void my_button::get_press_update(sf::Event &event) {
    is_button_pressed = 0;
    if ((event.type == sf::Event::MouseButtonPressed && 
         event.mouseButton.button == sf::Mouse::Left) && 
       ((BUTTON_UPPER_LEFT_CORNER_X < event.mouseButton.x && 
         event.mouseButton.x < BUTTON_UPPER_LEFT_CORNER_X + BUTTON_WIDTH) && 
        (BUTTON_UPPER_LEFT_CORNER_Y < event.mouseButton.y && 
         event.mouseButton.y < BUTTON_UPPER_LEFT_CORNER_Y + BUTTON_HEIGHT))) is_button_pressed = 1;
}

int my_button::update(int is_loading, const char *text_str, const sf::Color &fill_color) {
    text.setString(text_str);
    if (is_loading) {
        rectangle.setFillColor(sf::Color(COLOR_THEME_R - COLOR_THEME_DELTA * 2, COLOR_THEME_G - COLOR_THEME_DELTA * 2, COLOR_THEME_B - COLOR_THEME_DELTA * 2));
        rectangle.setOutlineColor(sf::Color(COLOR_THEME_R - COLOR_THEME_DELTA, COLOR_THEME_G - COLOR_THEME_DELTA, COLOR_THEME_B - COLOR_THEME_DELTA));
        return 0;
    }
    rectangle.setFillColor(fill_color);
    rectangle.setOutlineColor(sf::Color(COLOR_THEME_R + COLOR_THEME_DELTA, COLOR_THEME_G + COLOR_THEME_DELTA, COLOR_THEME_B + COLOR_THEME_DELTA));
    return 0;
}

void my_button::draw(sf::RenderWindow &window) {
    window.draw(rectangle);
    window.draw(text);
}

my_button::~my_button() {
    printf("END\n");
}

my_background::my_background(sf::Texture &texture) {
    background.setSize(sf::Vector2f(WINDOW_SIZE, WINDOW_SIZE));
    background.setFillColor(sf::Color::White);
    background.setTexture(&texture);
    background.setTextureRect(sf::IntRect(0, 0, WINDOW_SIZE, WINDOW_SIZE));
}

void my_background::draw(sf::RenderWindow &window) {
    window.draw(background);
}

my_background::~my_background() {
    printf("END\n");
}

int source_load(sf::Font &font, sf::Texture &texture, sf::SoundBuffer &sound_buffer) {
    #ifdef RELEASE
    if (!font.loadFromMemory(FONT_MEM, FONT_MEM_SIZE)) {
        //printf("Crash report: font load error\n"); 
        return 1;
    }
    if (!texture.loadFromMemory(TEXTURE_MEM, TEXTURE_MEM_SIZE)) {
        //printf("Crash report: texture load error\n"); 
        return 2;
    }
    if (!sound_buffer.loadFromMemory(SOUND_BUF_MEM, SOUND_BUF_MEM_SIZE)) {
        //printf("Crash report: sound load error\n"); 
        return 3;
    }
    #endif

    #ifdef DEBUG
    char font_name[FONT_NAME_SIZE] = "FORTSSH_.ttf";
    int is_font_loaded = font.loadFromFile(font_name);
    if (!is_font_loaded) {
        printf("Crash report: font %s not found\n", font_name); 
        return 4;
    }

    char texture_name[TEXTURE_NAME_SIZE] = "texture.png";
    if (!texture.loadFromFile(texture_name)) {
        printf("Crash report: texture %s not found\n", texture_name); 
        return 4;
    }

    char sound_name[SOUND_NAME_SIZE] = "Dubmood.flac";
    if (!sound_buffer.loadFromFile(sound_name)) {
        printf("Crash report: sound %s not found\n", sound_name); 
        return 4;
    }
    #endif
    return 0;
}

void my_play(sf::Sound &sound, sf::SoundBuffer &sound_buffer, int sound_volume) {
    sound.setBuffer(sound_buffer);
    sound.setLoop(true);
    sound.setVolume(sound_volume);
    sound.play();
}

int timer_checker(double control_point, double sec_to_wait) {
    if ((double) clock() / CLOCKS_PER_SEC - control_point >= sec_to_wait) return 1;
    return 0;
}

size_t hash_func(unsigned char *word, size_t word_len) {
    size_t ans = 0;
    size_t word_pos = 0;
    while (word_pos < word_len) {
        ans = left_cycl_shft_m_to_l(ans) ^ *(word + word_pos);
        ++word_pos;
    }
    //printf("%lu\n", ans);
    return ans;
}

int crack(FILE *code_in, unsigned char *buf) {
    fseek(code_in, POS_1, SEEK_SET);
    fputc(REPLACE_BYTE_1, code_in);
    fseek(code_in, POS_2, SEEK_SET);
    fputc(REPLACE_BYTE_2, code_in);
    return 1;
}