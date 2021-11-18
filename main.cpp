#include "hwlib.hpp"
#include "rtos.hpp"
//#include "Logger.h"
//Logger* pLogger = nullptr;

#include "sendIR.h"
#include "runGame.h"
#include "receiveIR.h"
#include "buttonListener.h"


int main( void ){
    // wait for the PC console to start
    hwlib::wait_ms( 500 );

    auto irSend = sendIR();
    auto runGame = runGameControl(irSend);
    auto receive = receiveIR(runGame);
    auto buttons = buttonListener(runGame);

    rtos::run();
}
