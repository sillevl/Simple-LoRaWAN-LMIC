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


#include <Node.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
//#include <algorithm>

std::vector<SimpleLoRaWAN::Node*> _nodeInstances;

void onEvent(ev_t event)
{
    for(int i = 0; i != _nodeInstances.size(); i++) {
        _nodeInstances[i]->onEvent(event);
    }
}


namespace SimpleLoRaWAN
{

Node::Node()
{
#ifdef RFM95_RESET_CONNECTED
    rfm95wReset = 0;
    wait_ms(10);
    rfm95wReset = 1;
    wait_ms(10);
#endif
    init();
    _nodeInstances.push_back(this);
}

Node::~Node()
{
    //int index = find(_nodeInstances.begin(), _nodeInstances.end(), this) - _nodeInstances.begin();
    //_nodeInstances.erase(_nodeInstances.begin() +index);
}

void Node::init()
{
    os_init();

    // reset MAC state
    LMIC_reset();
    setSpreadFactor(DR_SF7);
}


void Node::send(char* data, int size)
{
    send((uint8_t*) data, size);
}

void Node::send(uint8_t* data, int size)
{
    memcpy (LMIC.frame, data, size);
    LMIC_setTxData2(15, LMIC.frame, size, 0);
}


void Node::onEvent(ev_t event)
{

    if(eventHandler != NULL){
        eventHandler(event);
    }

    switch(event) {
        case EV_SCAN_TIMEOUT:
            scanTimeoutEventHandler();
            break;
        case EV_BEACON_FOUND:
            beaconFoundEventHandler();
            break;
        case EV_BEACON_MISSED:
            beaconMissedEventHandler();
            break;
        case EV_BEACON_TRACKED:
            beaconTrackedEventHandler();
            break;
        case EV_JOINING:
            joiningEventHandler();
            break;
        case EV_JOINED:
            joinedEventHandler();
            break;
        case EV_RFU1:
            rfu1EventHandler();
            break;
        case EV_JOIN_FAILED:
            joinFailedEventHandler();
            break;
        case EV_REJOIN_FAILED:
            rejoinFailedEventHandler();
            break;
        case EV_TXCOMPLETE:
            txCompleteEventHandler();
            break;
        case EV_LOST_TSYNC:
            lostTsyncEventHandler();
            break;
        case EV_RESET:
            resetEventHandler();
            break;
        case EV_RXCOMPLETE:
            rxCompleteEventHandler();
            break;
        case EV_LINK_DEAD:
            linkDeadEventHandler();
            break;
        case EV_LINK_ALIVE:
            linkAliveEventHandler();
            break;
         default:
            // Unknown event
            break;
    }
}

void Node::setEventHandler(void (*fnc)(ev_t))
{
    eventHandler = fnc;
}

void Node::setScanTimeoutEventHandler(void (*fnc)())
{
    scanTimeoutEventHandler = fnc;
}

void Node::setBeaconFoundEventHandler(void (*fnc)())
{
    beaconFoundEventHandler = fnc;
}

void Node::setBeaconMissedEventHandler(void (*fnc)())
{
    beaconMissedEventHandler = fnc;
}

void Node::setBeaconTrackedEventHandler(void (*fnc)())
{
    beaconTrackedEventHandler = fnc;
}

void Node::setJoiningEventHandler(void (*fnc)())
{
    joiningEventHandler = fnc;
}

void Node::setJoinedEventHandler(void (*fnc)())
{
    joinedEventHandler = fnc;
}

void Node::setRfu1EventHandler(void (*fnc)())
{
    rfu1EventHandler = fnc;
}

void Node::setJoinFailedEventHandler(void (*fnc)())
{
    joinFailedEventHandler = fnc;
}

void Node::setRejoinFailedEventHandler(void (*fnc)())
{
    rejoinFailedEventHandler = fnc;
}

void Node::setTxCompleteEventHandler(void (*fnc)())
{
    txCompleteEventHandler = fnc;
}

void Node::setResetEventHandler(void (*fnc)())
{
    resetEventHandler = fnc;
}

void Node::setRxCompleteEventHandler(void (*fnc)())
{
    rxCompleteEventHandler = fnc;
}

void Node::setLinkDeadEventHandler(void (*fnc)())
{
    linkDeadEventHandler = fnc;
}

void Node::setLinkAliveEventHandler(void (*fnc)())
{
    linkAliveEventHandler = fnc;
}

void Node::process()
{
    os_runloop_once();
}

void Node::enableLinkCheck()
{
    setLinkCheck(true);
}

void Node::disableLinkCheck()
{
    setLinkCheck(false);
}

void Node::setLinkCheck(int state)
{
    LMIC_setLinkCheckMode(state);
}

void Node::setSpreadFactor(int spreadfactor)
{
    LMIC_setDrTxpow(spreadfactor, 14);
}


} /* namespace SimpleLoRaWAN */
