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

#include "mbed.h"

extern Serial pc;

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
    pc.baud(115200);
    log = new LogIt(&pc);
    log->setLevel(LogIt::DEBUG);

    eventHandler = NULL;
    scanTimeoutEventHandler = NULL;
    beaconFoundEventHandler = NULL;
    beaconMissedEventHandler = NULL;
    beaconTrackedEventHandler = NULL;
    joiningEventHandler = NULL;
    joinedEventHandler = NULL;
    rfu1EventHandler = NULL;
    joinFailedEventHandler = NULL;
    rejoinFailedEventHandler = NULL;
    txCompleteEventHandler = NULL;
    lostTsyncEventHandler = NULL;
    resetEventHandler = NULL;
    rxCompleteEventHandler = NULL;
    linkDeadEventHandler = NULL;
    linkAliveEventHandler = NULL;
    receiveHandler = NULL;

    log->debug("Creating Simple-LoRaWAN node");

    processThread = new Thread(processTask, this);
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


void Node::send(char* data, int size, bool acknowledge)
{
    send(1, (uint8_t*) data, size, acknowledge);
}

void Node::send(unsigned char port, char* data, int size, bool acknowledge)
{
    send(port, (uint8_t*) data, size, acknowledge);
}

void Node::send(uint8_t* data, int size, bool acknowledge)
{
    send(1, data, size, acknowledge);
}

void Node::send(unsigned char port, uint8_t* data, int size, bool acknowledge)
{
    log->debug("Sending data with length %d, on port %d and acknowledge is %d", size, port, acknowledge);
    memcpy (LMIC.frame, data, size);
    LMIC_setTxData2(port, LMIC.frame, size, acknowledge);
}


void Node::onEvent(ev_t event)
{
    if(eventHandler != NULL){
        log->info("Event: %d", event);
        eventHandler(event);
    }

    switch(event) {
        case EV_SCAN_TIMEOUT:
            log->info("Scan timeout event");
            if(scanTimeoutEventHandler != NULL){
                scanTimeoutEventHandler();
            }
            break;
        case EV_BEACON_FOUND:
            log->info("Beacon found event");
            if(beaconFoundEventHandler != NULL){
                beaconFoundEventHandler();
            }
            break;
        case EV_BEACON_MISSED:
            log->info("Beacon missed event");
            if(beaconMissedEventHandler != NULL){
                beaconMissedEventHandler();
            }
            break;
        case EV_BEACON_TRACKED:
            log->info("Beacon tracked event");
            if(beaconTrackedEventHandler != NULL){
                beaconTrackedEventHandler();
            }
            break;
        case EV_JOINING:
            log->info("Joining event");
            if(joiningEventHandler != NULL){
                joiningEventHandler();
            }
            break;
        case EV_JOINED:
            log->info("Joined event");
            if(joinedEventHandler != NULL){
                joinedEventHandler();
            }
            break;
        case EV_RFU1:
            log->info("RFU1 event");
            if(rfu1EventHandler != NULL){
                rfu1EventHandler();
            }
            break;
        case EV_JOIN_FAILED:
            log->info("Join failed event");
            if(joinFailedEventHandler != NULL){
                joinFailedEventHandler();
            }
            break;
        case EV_REJOIN_FAILED:
            log->info("Rejoin failed event");
            if(rejoinFailedEventHandler != NULL){
                rejoinFailedEventHandler();
            }
            break;
        case EV_TXCOMPLETE:
            log->info("Transmit complete event");
            if (LMIC.txrxFlags & TXRX_ACK){             // needs ACK and gets ACK
              log->debug("need ACK and got ACK");
            } else if(LMIC.txrxFlags & TXRX_NACK) {     // needs ACK and gets NO ACK
              log->debug("need ACK and got NO ACK");
            } else {                                    // needs no ACK
              log->debug("NO ACK needed");
            }

            if (LMIC.dataLen) {
              uint8_t buffer[MAX_LEN_FRAME];
              memcpy (buffer, LMIC.frame + LMIC.dataBeg, LMIC.dataLen);
              uint8_t port = (LMIC.txrxFlags & TXRX_PORT) ? LMIC.frame[LMIC.dataBeg-1] : 0;
              log->info("Data payload received");
              log->debug("Received %d bytes of payload on port %d", LMIC.dataLen, port);
              receiveHandler(port, buffer, LMIC.dataLen);
            } else {
              log->debug("No data received");
            }

            if(txCompleteEventHandler != NULL){
                txCompleteEventHandler();
            }
            break;
        case EV_LOST_TSYNC:
            log->info("Lost tsync event");
            if(lostTsyncEventHandler != NULL){
                lostTsyncEventHandler();
            }
            break;
        case EV_RESET:
            log->info("Reset event");
            if(resetEventHandler != NULL){
                resetEventHandler();
            }
            break;
        case EV_RXCOMPLETE:
            log->info("Receive complete event");
            if(rxCompleteEventHandler != NULL){
                rxCompleteEventHandler();
            }
            break;
        case EV_LINK_DEAD:
            log->info("Link dead event");
            if(linkDeadEventHandler != NULL){
                linkDeadEventHandler();
            }
            break;
        case EV_LINK_ALIVE:
            log->info("Link alive event");
            if(linkAliveEventHandler != NULL){
                linkAliveEventHandler();
            }
            break;
         default:
            // Unknown event
            break;
    }
}

void Node::setEventHandler(void (*fnc)(ev_t))
{
    log->debug("Setting eventhandler");
    eventHandler = fnc;
}

void Node::setScanTimeoutEventHandler(void (*fnc)())
{
    log->debug("Setting scan timeout eventhandler");
    scanTimeoutEventHandler = fnc;
}

void Node::setBeaconFoundEventHandler(void (*fnc)())
{
    log->debug("Setting beacon found eventhandler");
    beaconFoundEventHandler = fnc;
}

void Node::setBeaconMissedEventHandler(void (*fnc)())
{
    log->debug("Setting beacon missed eventhandler");
    beaconMissedEventHandler = fnc;
}

void Node::setBeaconTrackedEventHandler(void (*fnc)())
{
    log->debug("Setting beacon tracked eventhandler");
    beaconTrackedEventHandler = fnc;
}

void Node::setJoiningEventHandler(void (*fnc)())
{
    log->debug("Setting joining eventHandler");
    joiningEventHandler = fnc;
}

void Node::setJoinedEventHandler(void (*fnc)())
{
    log->debug("Setting joined eventhandler");
    joinedEventHandler = fnc;
}

void Node::setRfu1EventHandler(void (*fnc)())
{
    log->debug("Setting RFU1 eventhandler");
    rfu1EventHandler = fnc;
}

void Node::setJoinFailedEventHandler(void (*fnc)())
{
    log->debug("Setting join failed eventhanlder");
    joinFailedEventHandler = fnc;
}

void Node::setRejoinFailedEventHandler(void (*fnc)())
{
    log->debug("Setting rejoin failed eventhandler");
    rejoinFailedEventHandler = fnc;
}

void Node::setTxCompleteEventHandler(void (*fnc)())
{
    log->debug("Setting transmit complete eventhandler");
    txCompleteEventHandler = fnc;
}

void Node::setLostTSyncEventHandler(void (*fnc)())
{
    log->debug("Lost TSync eventhandler");
    lostTsyncEventHandler = fnc;
}

void Node::setResetEventHandler(void (*fnc)())
{
    log->debug("Setting reset eventhanlder");
    resetEventHandler = fnc;
}

void Node::setRxCompleteEventHandler(void (*fnc)())
{
    log->debug("Setting received complete eventhandler");
    rxCompleteEventHandler = fnc;
}

void Node::setLinkDeadEventHandler(void (*fnc)())
{
    log->debug("Setting link dead eventhandler");
    linkDeadEventHandler = fnc;
}

void Node::setLinkAliveEventHandler(void (*fnc)())
{
    log->debug("Setting link alive eventhanlder");
    linkAliveEventHandler = fnc;
}

void Node::setReceiveHandler(void (*fnc)(uint8_t, uint8_t*, uint8_t))
{
   log->debug("Setting receive eventhanlder");
    receiveHandler = fnc;
}

void Node::process()
{
    os_runloop_once();
}

void Node::processTask(void const *argument)
{
    Node* self = (Node*)argument;

    while(true)
    {
        self->process();
        Thread::yield();
    }
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

int Node::timeUntilNextSend()
{
    log->debug("Time: %d\r\n", LMIC.globalDutyAvail);
    return LMIC.globalDutyAvail;
}


} /* namespace SimpleLoRaWAN */
