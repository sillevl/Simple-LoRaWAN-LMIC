# Simple-LoRaWAN
C++ wrapper for the IBM LMIC LoRaWAN implementation.

Mbed Library: [https://developer.mbed.org/users/sillevl/code/Simple-LoRaWAN/](https://developer.mbed.org/users/sillevl/code/Simple-LoRaWAN/)

## Examples

### OTAA Example on Mbed

```cpp
#include "mbed.h"
#include "Simple-LoRaWAN.h"

using namespace SimpleLoRaWAN;

uint8_t appEui[8]   = { 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00 };
uint8_t devEui[8]   = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[16]  = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

uint8_t port = 10;

OTAA::Node node(appEui, devEui, appKey);

int main(void)
{
    node.send(port, "Hello from Simple-LoRaWAN", 25);
}
```

### ABP Example on Mbed

```cpp
#include "mbed.h"
#include "Simple-LoRaWAN.h"

using namespace SimpleLoRaWAN;

uint32_t devAddr    = 0x12345678;
uint8_t nwksKey[16] = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};
uint8_t appKey[16]  = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

uint8_t port = 10;

ABP::Node node(devAddr, nwksKey, appKey);

int main(void)
{
    node.send(port, "Hello from Simple-LoRaWAN", 25);
}
```

### Sending data with interval

```cpp
#include "mbed.h"
#include "Simple-LoRaWAN.h"

using namespace SimpleLoRaWAN;

uint8_t appEui[8]   = { 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00 };
uint8_t devEui[8]   = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[16]  = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

uint8_t port = 10;

OTAA::Node node(appEui, devEui, appKey);

int main(void)
{
    while(true){
      node.send(port, "Hello from Simple-LoRaWAN", 25);
      Thread::wait(10000);
    }
}
```

### Receiving data (Class A)

```cpp
#include "mbed.h"
#include "Simple-LoRaWAN.h"

using namespace SimpleLoRaWAN;

uint8_t appEui[8]   = { 0x00, 0x00, 0x00, 0xD0, 0x00, 0x00, 0x00, 0x00 };
uint8_t devEui[8]   = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t appKey[16]  = {
    0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
    0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

uint8_t port = 10;

OTAA::Node node(appEui, devEui, appKey);

void receive(uint8_t port, uint8_t* data, uint8_t size)
{
  printf("Data received with size: %d, [", size);
  for (int i = 0; i < size; i++) {
      printf("0x%02X ", data[i]);
  }
  printf("] on port: %d\r\n", port);
}

int main(void)
{
    node.setReceiveHandler(&receive);

    while(true){
      node.send(port, "Hello from Simple-LoRaWAN", 25);
      Thread::wait(10000);
    }
}
```
