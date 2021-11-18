#ifndef V2THDE_EXAMPLES_RECEIVEIR_H
#define V2THDE_EXAMPLES_RECEIVEIR_H

#include "hwlib.hpp"
#include "rtos.hpp"
#include "runGame.h"

class receiveIR : public rtos::task <>{
private:
    uint16_t data = 0x00;
    int timer;
    runGameControl & rungame;
    unsigned int timeout;

    void main(){
        auto pin = hwlib::target::pin_in(hwlib::target::pins::d8);
        enum state_t {IDLE, RECEIVE};
        state_t state = IDLE;
        for (;;) {
            switch (state) {
                case IDLE: {
                    hwlib::wait_us(500);
                    pin.refresh();
                    if (pin.read() == 0) {
                        state = RECEIVE;
                    }
                    break;
                }

                case RECEIVE: {
                    data = 0;
                    for (int i = 0; i < 16; i++) {
                        timeout = hwlib::now_us();
                        for (;;) {
                            hwlib::wait_us(100);
                            timer = 0;
                            if (pin.read() == 0) {
                                timer = hwlib::now_us();
                                while (pin.read() == 0) {hwlib::wait_us(100);}
                                timer = (hwlib::now_us() - timer);
                            }

                            if (timer >= 1200) {
                                data = data | 0x01;
                                if (i < 16) {
                                    data = data << 1;
                                }
                                break;
                            } else if (timer >= 500 && timer < 1200) {
                                if (i < 15) {
                                    data = data << 1;
                                }
                                break;
                            }
                            if(hwlib::now_us()>=(timeout+40000)){
                                state = IDLE;
                                break;
                            }
                        }
                        if(hwlib::now_us()>=(timeout+4000)){break;}
                    }
                    if(hwlib::now_us()>=(timeout+4000)){break;}

                    playerID = (data >> 11) & 0b1111;
                    weaponPower = (data >> 9) & 0b11;
                    playtime = (data >> 4) & 0b11111;

                    rungame.getHit(playerID, weaponPower);
                    state = IDLE;
                }
            }
        }
    }

public:
    uint8_t playerID;
    uint8_t weaponPower;
    uint8_t playtime;
    receiveIR(runGameControl & rungame):
            rtos::task<>("receiveIRTaak"),
            rungame(rungame)
    {}
};

#endif //V2THDE_EXAMPLES_RECEIVEIR_H
