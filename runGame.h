#ifndef V2THDE_EXAMPLES_RUNGAME_H
#define V2THDE_EXAMPLES_RUNGAME_H

#include "sendIR.h"
#include "display.h"

/// @brief Run Class
/// @details This is the class that runs the game. It stays in the void main loop until something happends.

class runGameControl : public rtos::task <>{
private:
    enum state_t {NORMAAL, SHOOT};
    state_t state = NORMAAL;
    sendIR & IR;
    int playerID = 8;
    int playtime = 5;
    int weaponPower = 3;
    rtos::pool <int> HitPowerPool;
    rtos::pool <int> HitPlayerPool;
    rtos::flag hitFlag;
    rtos::channel<int, 1> buttonChannel;
    display & scherm;

    ///@brief Main loop this is were the game is if its waiting for a hit or to shoot.
    ///@details This function has two states NORMAAL is were it waits for a hit so it can process it and check by which player and with what weapon you've been hit. And SHOOT this is were you can shoot. It shifts the right data in an uint16_t and then sends it to the shoot class.
    void main() {
        for (;;) {
            switch (state) {
                case NORMAAL: {
                    hwlib::wait_ms(100);
                    auto evt = wait(hitFlag+buttonChannel);
                    if (evt == hitFlag) {
                        int playerIDtmp = HitPlayerPool.read();
                        int powertmp = HitPowerPool.read();
                        scherm.setDisplayFlag(playerIDtmp, powertmp);
                        state = NORMAAL;
                        break;
                    }

                    if (evt == buttonChannel) {
                        if (buttonChannel.read() == 1) {
                            state = SHOOT;
                            break;
                        }
                    }
                }
                case SHOOT: {
                    uint16_t data = (((((1 << 4) | playerID) << 2 | weaponPower) << 5) | playtime) << 4;
                    IR.setShoot(data);
                    state = NORMAAL;
                    break;
                }
            }
        }
    }

public:
    ///@brief constructor of the runGameControl class.
    ///@param IR This is the LED which will send the data.
    ///@param scherm This is the screen on which the data will be displayed.
    runGameControl(sendIR & IR, display & scherm):
            rtos::task<>("RunGameTask"),
            IR(IR),
            HitPowerPool("HitPowerPool"),
            HitPlayerPool("HitPlayerPool"),
            hitFlag(this, "hitFlag"),
            buttonChannel(this, "buttonID"),
            scherm(scherm)
    {}

    ///@brief This function writes a buttonID in the buttonChannel from the buttonListener class.
    void buttonPressed(int buttonID){
        buttonChannel.write(buttonID);
    }


    ///@brief This function gets the weaponPower and playerID and sets the hitFlag. This function gets called from the receiveIR class.
    void getHit(uint8_t playernr, uint8_t power) { HitPowerPool.write(power); HitPlayerPool.write(playernr); hitFlag.set(); }
};

#endif //V2THDE_EXAMPLES_RUNGAME_H
