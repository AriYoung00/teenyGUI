#include <iostream>
#include <chrono>
#include <thread>

#include "DisplayHelper.h"
#include "Adafruit_SSD1306_emu.h"

using std::cout;
using std::endl;

int main() {
    Adafruit_SSD1306 display;
    RenderManager manager(display);
    MenuView topView("Top-level view", &manager);
    MenuView cascadeView("Cascading view click me", &manager);

    display.emu_setDownCallback(std::bind(&RenderManager::cursorUp, &manager));
    display.emu_setUpCallback(std::bind(&RenderManager::cursorDown, &manager));
    display.emu_setSelectCallback(std::bind(&RenderManager::select, &manager));

    topView.addElement(&cascadeView);
    char buf[64];
    for (int i = 0; i < 5; i++) {
        sprintf(buf, "Hello world %d", i);
        topView.addElement(new Button(
                buf,
                [i](void* args) { cout << "Hello world callback " << i << endl; }
        ));
    }

    for (int i = 0; i < 5; i++) {
        sprintf(buf, "Cascade button %d", i);
        cascadeView.addElement(new Button(
                buf,
                [i](void* args) { cout << "Cascade callback " << i << endl; }
        ));
    }

    manager.setTopLevelView(&topView);

    while (true) {
        manager.update();
        std::this_thread::sleep_for(std::chrono::milliseconds(13));
    }
}
