#pragma once

#include <functional>
#include <stack>

#ifdef ARDUINO_ARCH_ESP32
    #include <Adafruit_SSD1306.h>
    #include <Vector.h>
#else
    #include "Adafruit_SSD1306_emu.h"
    #include <vector>
    #define Vector std::vector
#endif


#define RANGE(a) (int i = 0; i < a; i++)

#define NUM_LINES 8

// Forward declaration
class RenderManager;

class GuiElement {
public:
    virtual const char* renderText() = 0;
    virtual void onClick() = 0;
    bool isClickable();
protected:
    bool m_isClickable;
};

class RenderableView : public GuiElement {
public:
    RenderableView(RenderManager* manager);
    void onClick() override;

    virtual void renderFullscreen(Adafruit_SSD1306& display) = 0;
    virtual void clickUp() = 0;
    virtual void clickDown() = 0;
    virtual void clickSelect() = 0;

protected:
    void returnView();

private:
    RenderManager* m_manager;
};


class Button : public GuiElement {
public:
    Button(const char* text, std::function<void(void*)> callback, void* callbackArgs = nullptr);

    const char* renderText() override;
    void onClick() override ;

    void setText(char* text);

private:
    char m_text[64];
    std::function<void(void*)> m_callback;
    void* m_callbackArgs;
};


class MenuView : public RenderableView {
public:
    MenuView(const char* text, RenderManager* manager);

    void renderFullscreen(Adafruit_SSD1306& display) override;
    const char* renderText() override;
    void clickUp() override;
    void clickDown() override;
    void clickSelect() override;

    void addElement(::GuiElement* element);
    void setElement(::GuiElement* element, int pos);
    void removeElement(int pos);

private:
    void _decCursorPos();
    void _incCursorPos();

    Vector<::GuiElement*> m_children;
    char m_text[64];
    int m_cursorPos;
    int m_selectTimer;

    bool m_shouldSelect = false;
};

class RenderManager {
public: 
    explicit RenderManager(Adafruit_SSD1306& display);

    void setTopLevelView(RenderableView* topLevelView);
    void cursorUp();
    void cursorDown();
    void select();
    void update();

private:
    void setActiveView(RenderableView* view);
    void viewReturn();
    Adafruit_SSD1306& m_display;
    RenderableView* m_activeView;
    std::stack<RenderableView*> m_parentStack;

friend class RenderableView;
};