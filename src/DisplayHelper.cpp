#include "DisplayHelper.h"

#include <string.h>
#ifdef ARDUINO_ARCH_ESP32
    #include <Wire.h>
#endif

bool GuiElement::isClickable() {
    return m_isClickable;
}

RenderManager::RenderManager(Adafruit_SSD1306& display) : m_display(display) {
    m_activeView = nullptr;
}

void RenderManager::setTopLevelView(RenderableView *topLevelView) {
    m_activeView = topLevelView;
}

void RenderManager::cursorUp() {
    m_activeView->clickUp();
}

void RenderManager::cursorDown() {
    m_activeView->clickDown();
}

void RenderManager::select() {
    m_activeView->clickSelect();
}

void RenderManager::setActiveView(RenderableView* view) {
    if (m_activeView != nullptr) {
        m_parentStack.push(m_activeView);
    }
    m_activeView = view;
}

void RenderManager::viewReturn() {
    if (!m_parentStack.empty()) {
        m_activeView = m_parentStack.top();
        m_parentStack.pop();
    } else {
        throw "Error: View returned, but parent stack is empty";
    }
}

void RenderManager::update() {
//    m_display.clearDisplay();
    m_activeView->renderFullscreen(m_display);
    m_display.display();
}

RenderableView::RenderableView(RenderManager* manager) : m_manager(manager) {
    m_isClickable = true;
}

void RenderableView::onClick() {
    m_manager->setActiveView(this);
}

void RenderableView::returnView() {
    m_manager->viewReturn();
}

Button::Button(const char* text, std::function<void(void*)> callback, void* callbackArgs) 
    : m_callback(callback), m_callbackArgs(callbackArgs) {
    m_isClickable = true;
    strcpy(m_text, text);
}

const char* Button::renderText() {
    return m_text;
}

void Button::onClick() {
    m_callback(m_callbackArgs);
}

void Button::setText(char* text) {
    strcpy(m_text, text);
}

MenuView::MenuView(const char* text, RenderManager* manager)
    : RenderableView(manager), m_cursorPos(0), m_selectTimer(0) {
    m_children.push_back(new Button(
        "<- Back", 
        [this](void* args) { this->returnView(); }
    ));
    strcpy(m_text, text);
}

void MenuView::renderFullscreen(Adafruit_SSD1306& display) {
    //TODO: Add scroll bar rendering if lines overflow past available space
    display.clearDisplay();
    display.setCursor(0, 0);

    for RANGE(m_children.size()) {
        if (i == m_cursorPos && m_selectTimer > 0) {
            display.print(" >");
            m_selectTimer--;
        } else if (i == m_cursorPos && m_selectTimer <= 0) {
            m_selectTimer = 0;
            display.print("> ");
        } else {
            display.print("  ");
        }

        if (m_children[i] != nullptr)
            display.println(m_children[i]->renderText());
        else
            display.println();
    }

    if (m_shouldSelect) {
        m_shouldSelect = false;
        m_children[m_cursorPos]->onClick();
        m_selectTimer = 7;
    }
}

const char* MenuView::renderText() {
    return m_text;
}

void MenuView::clickUp() {
    do {
        _incCursorPos();
    } while (m_children[m_cursorPos] == nullptr || !m_children[m_cursorPos]->isClickable());
}

void MenuView::clickDown() {
    do {
        _decCursorPos();
    } while (m_children[m_cursorPos] == nullptr || !m_children[m_cursorPos]->isClickable());
}

void MenuView::clickSelect() {
    m_shouldSelect = true;
}

void MenuView::addElement(::GuiElement* element) {
    m_children.push_back(element);
}

void MenuView::setElement(::GuiElement* element, int pos) {
    if (pos < m_children.size())
        m_children.push_back(element);
    else
        throw std::out_of_range("Attempted to set nonexistent element");
}

void MenuView::removeElement(int pos) {
    if (pos == m_children.size() - 1)
        m_children.pop_back();
    else if (pos < m_children.size())
        m_children[pos] = nullptr;
    else
        throw std::out_of_range("Attempted to remove nonexistent element");
}

void MenuView::_decCursorPos() {
    if (m_cursorPos > 0)
        m_cursorPos--;
    else
        m_cursorPos = m_children.size() - 1;
}

void MenuView::_incCursorPos() {
    if (m_cursorPos < (m_children.size() - 1))
        m_cursorPos++;
    else
        m_cursorPos = 0;
}
