/* The MIT License (MIT)
 *
 * Copyright (c) 2016 Sille Van Landschoot
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SIMPLE_LORAWAN_NODE_H_
#define SIMPLE_LORAWAN_NODE_H_

//#define RFM95_RESET_CONNECTED 1  // define if p15 is connected to RFM95W reset

#include "lmic.h"
#include "stdint.h"

#ifdef RFM95_RESET_CONNECTED
#include "mbed.h"
#endif

namespace SimpleLoRaWAN
{

class Node
{
public:
    Node();
    virtual ~Node();
    void send(char* data, int size, bool acknowledge = false);
    void send(unsigned char port, char* data, int size, bool acknowledge = false);
    void send(uint8_t* data, int size, bool acknowledge = false);
    void send(unsigned char port, uint8_t* data, int size, bool acknowledge = false);
    void onEvent(ev_t event);
    void process();

    void setEventHandler(void (*fnc)(ev_t));
    void setScanTimeoutEventHandler(void (*fnc)());
    void setBeaconFoundEventHandler(void (*fnc)());
    void setBeaconMissedEventHandler(void (*fnc)());
    void setBeaconTrackedEventHandler(void (*fnc)());
    void setJoiningEventHandler(void (*fnc)());
    void setJoinedEventHandler(void (*fnc)());
    void setRfu1EventHandler(void (*fnc)());
    void setJoinFailedEventHandler(void (*fnc)());
    void setRejoinFailedEventHandler(void (*fnc)());
    void setTxCompleteEventHandler(void (*fnc)());
    void setResetEventHandler(void (*fnc)());
    void setRxCompleteEventHandler(void (*fnc)());
    void setLinkDeadEventHandler(void (*fnc)());
    void setLinkAliveEventHandler(void (*fnc)());

    void enableLinkCheck();
    void disableLinkCheck();
    void setLinkCheck(int state);

    void setSpreadFactor(int spreadfactor);


private:
    void init();
    void setLinkCheck();
#ifdef RFM95_RESET_CONNECTED
    DigitalOut rfm95wReset;
#endif

    void (*eventHandler)(ev_t);
    void (*scanTimeoutEventHandler)();
    void (*beaconFoundEventHandler)();
    void (*beaconMissedEventHandler)();
    void (*beaconTrackedEventHandler)();
    void (*joiningEventHandler)();
    void (*joinedEventHandler)();
    void (*rfu1EventHandler)();
    void (*joinFailedEventHandler)();
    void (*rejoinFailedEventHandler)();
    void (*txCompleteEventHandler)();
    void (*lostTsyncEventHandler)();
    void (*resetEventHandler)();
    void (*rxCompleteEventHandler)();
    void (*linkDeadEventHandler)();
    void (*linkAliveEventHandler)();


};

} /* namespace SimpleLoRaWAN */

#endif /* SIMPLE_LORAWAN_NODE_H_ */
