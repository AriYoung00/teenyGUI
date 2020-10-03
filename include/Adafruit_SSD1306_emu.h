//
// Created by ariel on 8/24/20.
//
#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>

#define SSD_HEIGHT 64
#define SSD_WIDTH 128

using std::string;
using std::stringstream;
using std::vector;
using std::function;

typedef function<void(void)> voidfunc;

class Adafruit_SSD1306 {
public:
    Adafruit_SSD1306();
    ~Adafruit_SSD1306();

    void print(const char*);
    void println(const char*);
    void println();
    void clearDisplay();
    void setCursor(int, int);
    void display();

    void emu_setSelectCallback(voidfunc);
    void emu_setUpCallback(voidfunc);
    void emu_setDownCallback(voidfunc);

    struct ChangeAwareLine {
        stringstream data;
        sf::Text sfText;
        bool hasChanged;
    };

private:
    vector<ChangeAwareLine*> m_lines;
    int m_currentLine;

    voidfunc m_selectCallback;
    voidfunc m_upCallback;
    voidfunc m_downCallback;

    int m_windowHeight;
    int m_windowWidth;
    float m_scaleFactor;

    int m_fontSize;
    int m_fontPad;

    sf::RenderWindow* m_window;
    sf::Font* m_font;
};

