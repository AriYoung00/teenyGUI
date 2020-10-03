//
// Created by ariel on 8/24/20.
//

#include <iostream>

#include "../include/Adafruit_SSD1306_emu.h"

using std::cout;
using std::endl;

Adafruit_SSD1306::Adafruit_SSD1306() {
    m_currentLine = 0;

    m_scaleFactor = 3.3f * ((float) sf::VideoMode::getDesktopMode().height) / 1080.0f; // Constant to magnify, then scale based on 1080p
    m_windowHeight = (int) m_scaleFactor * SSD_HEIGHT;
    m_windowWidth = (int) m_scaleFactor * SSD_WIDTH;

    m_fontSize = (int) (7 * m_scaleFactor);
    m_fontPad = 0.05 * m_fontSize;

    m_window = new sf::RenderWindow(sf::VideoMode(m_windowWidth, m_windowHeight), "Adafruit_SSD1306 display window");
    m_window->setActive();
    m_window->setKeyRepeatEnabled(false);

    m_font = new sf::Font();
    m_font->loadFromFile("res/Inconsolata-Regular.ttf");
}

Adafruit_SSD1306::~Adafruit_SSD1306() {
    delete m_window;

    for (auto&& ptr : m_lines) {
        delete ptr;
    }
    m_lines.clear();
}

void Adafruit_SSD1306::print(const char* str) {
    while (m_currentLine >= m_lines.size()) {
        sf::Text text;
        text.setFont(*m_font);
        text.setCharacterSize(m_fontSize);
        text.setPosition((float) 8*m_fontPad, (float) ((m_lines.size() * (m_fontSize + m_fontPad)) + m_fontPad));

        m_lines.emplace_back(new ChangeAwareLine {
            stringstream(),
            text,
            true
        });
    }

    m_lines[m_currentLine]->data << str;
    m_lines[m_currentLine]->hasChanged = true;
}

void Adafruit_SSD1306::println(const char* str) {
    print(str);
    m_currentLine++;
}

void Adafruit_SSD1306::println() {
    m_currentLine++;
}

void Adafruit_SSD1306::clearDisplay() {
    for (auto&& line : m_lines) {
        line->data.str("");
    }
}

void Adafruit_SSD1306::setCursor(int x, int y) {
    m_currentLine = y;
}

void Adafruit_SSD1306::display() {
    sf::Event event;
    while (m_window->pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            m_window->close();
            exit(EXIT_SUCCESS);
        }

        if (event.type == sf::Event::KeyPressed) {
            cout << "Key press event: " << event.key.code << endl;
            switch (event.key.code) {
                case sf::Keyboard::Up:
                    m_upCallback();
                    break;
                case sf::Keyboard::Down:
                    m_downCallback();
                    break;
                case sf::Keyboard::Return:
                    m_selectCallback();
                    break;
            }
        }
    }

    m_window->clear();
    for (auto&& line : m_lines) {
        if (line->hasChanged) {
            line->sfText.setString(line->data.str());
        }
        m_window->draw(line->sfText);
    }
    m_window->display();
}

void Adafruit_SSD1306::emu_setSelectCallback(voidfunc callback) {
    m_selectCallback = callback;
}

void Adafruit_SSD1306::emu_setUpCallback(voidfunc callback) {
    m_upCallback = callback;
}

void Adafruit_SSD1306::emu_setDownCallback(voidfunc callback) {
    m_downCallback = callback;
}
