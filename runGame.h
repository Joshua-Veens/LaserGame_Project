#ifndef V2THDE_EXAMPLES_RUNGAME_H
#define V2THDE_EXAMPLES_RUNGAME_H

#include "sendIR.h"
#include "display.h"

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

    runGameControl(sendIR & IR, display & scherm):
            rtos::task<>("RunGameTask"),
            IR(IR),
            HitPowerPool("HitPowerPool"),
            HitPlayerPool("HitPlayerPool"),
            hitFlag(this, "hitFlag"),
            buttonChannel(this, "buttonID"),
            scherm(scherm)
    {}

    void buttonPressed(int buttonID){
        buttonChannel.write(buttonID);
    }

    void getHit(uint8_t playernr, uint8_t power) { HitPowerPool.write(power); HitPlayerPool.write(playernr); hitFlag.set(); }
};

#endif //V2THDE_EXAMPLES_RUNGAME_H
