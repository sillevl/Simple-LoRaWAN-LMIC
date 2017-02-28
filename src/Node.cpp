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

    //  EV_SCAN_TIMEOUT=1, EV_BEACON_FOUND,
    //  EV_BEACON_MISSED, EV_BEACON_TRACKED, EV_JOINING,
    //  EV_JOINED, EV_RFU1, EV_JOIN_FAILED, EV_REJOIN_FAILED,
    //  EV_TXCOMPLETE, EV_LOST_TSYNC, EV_RESET,
    //  EV_RXCOMPLETE, EV_LINK_DEAD, EV_LINK_ALIVE

    if(eventHandler != NULL){
        eventHandler(event);
    }
}

void Node::setEventHandler(void (*fnc)(ev_t))
{
    eventHandler = fnc;
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
