#ifndef V2THDE_EXAMPLES_BUTTONLISTENER_H
#define V2THDE_EXAMPLES_BUTTONLISTENER_H

#include "runGame.h"

class buttonListener : public rtos::task<>{
private:
    hwlib::target::pin_in trigger  = hwlib::target::pin_in( hwlib::target::pins::d42 );
    runGameControl & runGame;

public:
    buttonListener(runGameControl & runGame):
            rtos::task<>( "buttonListenerTask"),
            runGame(runGame)
    {}
    void main() {
        for (;;) {
            if (trigger.read()== 0) {
                runGame.buttonPressed(1);
                hwlib::wait_ms(300);
            }
        }
    }
};

#endif //V2THDE_EXAMPLES_BUTTONLISTENER_H
