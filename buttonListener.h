#ifndef V2THDE_EXAMPLES_BUTTONLISTENER_H
#define V2THDE_EXAMPLES_BUTTONLISTENER_H

#include "runGame.h"

class buttonListener : public rtos::task<>{
private:
    hwlib::target::pin_in trigger  = hwlib::target::pin_in( hwlib::target::pins::d42 );
    runGameControl & runGame;

public:
    /// @brief constructor of the buttonlistener class
    /// @param runGame the buttonListener class needs a reference of runGameControl
    buttonListener(runGameControl & runGame):
            rtos::task<>( "buttonListenerTask"),
            runGame(runGame)
    {}

    /// @brief checks constantly if button is pressed,
    /// when button is pressed calls function buttonPressed from the runGame class
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
