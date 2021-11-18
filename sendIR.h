#ifndef V2THDE_EXAMPLES_SENDIR_H
#define V2THDE_EXAMPLES_SENDIR_H

#include "hwlib.hpp"
#include "rtos.hpp"

class sendIR : public rtos::task<> {
private:
    uint16_t Data;
    rtos::flag shootFlag;

    void main(){
        auto led_1 = hwlib::target::d2_36kHz();
        enum state_t {IDLE, SHOOT};
        state_t state = IDLE;
        for(;;){
            switch(state){
                case IDLE: {
                    auto evt = wait(shootFlag);
                    if(evt == shootFlag){
                        state = SHOOT;
                    }
                    break;
                }

                case SHOOT: {
                    for(unsigned int i = 0; i < 16; i++){
                        bool bit = (Data & (32768 >> i));
                        if(bit){
                            led_1.write(1);
                            led_1.flush();
                            hwlib::wait_us(1600);
                            led_1.write(0);
                            led_1.flush();
                            hwlib::wait_us(800);
                        }else{
                            led_1.write(1);
                            led_1.flush();
                            hwlib::wait_us(800);
                            led_1.write(0);
                            led_1.flush();
                            hwlib::wait_us(1600);
                        }
                    }
                    state = IDLE;
                    break;
                }
            }
        }
    }


public:
    sendIR():
            rtos::task<>("sendIRMessage"),
            shootFlag(this, "shootFlag")
    {}

    void setShoot(uint16_t data){
        Data = data;
        shootFlag.set();
    }

};

#endif //V2THDE_EXAMPLES_SENDIR_H
