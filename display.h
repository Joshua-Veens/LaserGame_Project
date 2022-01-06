//
// Created by joshu on 4-1-2022.
//

#ifndef V2THDE_EXAMPLES_DISPLAY_H
#define V2THDE_EXAMPLES_DISPLAY_H

#include "hwlib.hpp"
#include "rtos.hpp"

class display : public rtos::task<> {
private:
    enum state_t {IDLE, SHOW};
    state_t state = IDLE;

    rtos::flag displayFlag;
    int playerID;
    int weaponPower;

    hwlib::target::pin_oc scl = hwlib::target::pin_oc( hwlib::target::pins::scl );
    hwlib::target::pin_oc sda = hwlib::target::pin_oc( hwlib::target::pins::sda );
    hwlib::i2c_bus_bit_banged_scl_sda i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
    hwlib::glcd_oled oled = hwlib::glcd_oled(i2c_bus, 0x3c);

    hwlib::window_part_t w1 = hwlib::part(
            oled,
            hwlib::xy( 0, 0 ),
            hwlib::xy( 128, 32));
    hwlib::window_part_t w2 = hwlib::part(
            oled,
            hwlib::xy( 0, 32 ),
            hwlib::xy( 128, 32));

    hwlib::font_default_8x8 f1 = hwlib::font_default_8x8();
    hwlib::terminal_from d1 = hwlib::terminal_from( w1, f1 );

    hwlib::font_default_8x8 f2 = hwlib::font_default_8x8();
    hwlib::terminal_from d2 = hwlib::terminal_from( w2, f2 );

    void update(){
        oled.flush();
    }

    void clearDisplay(){
        d1 << "\f";
        d2 << "\f";
        update();
    }

public:
    display():
        rtos::task<>(1,"displayTask"),
        displayFlag(this, "displayFlag")
    {}

    void hitBy(){
        clearDisplay();
        d1 << "\f"
           << "LASER shooter";
        d2 << "\fHit by: " << playerID
           << "\nWeapon:" << weaponPower;
        update();
    }

    void ingame(){
        clearDisplay();
        d1 << "\f"
           << "LASER shooter";
        d2 << "\fGame is running\n"
           << "==============";
        update();
    }

    void main(){
        ingame();
        for(;;){
            switch(state){
                case IDLE: {
                    auto evt = wait(displayFlag);
                    if(evt == displayFlag){
                        state = SHOW;
                        break;
                    }
                    hwlib::wait_ms(500);
                }
                case SHOW: {
                    hitBy();
                    state = IDLE;
                    break;
                }
            }
        }
    }

    void setDisplayFlag(int & hitByPlayer, int & hitByWeapon){
        playerID = hitByPlayer;
        weaponPower = hitByWeapon;
        displayFlag.set();
    }

};


#endif //V2THDE_EXAMPLES_DISPLAY_H
