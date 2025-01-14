# Bluetoe [![Build Status](https://travis-ci.org/TorstenRobitzki/bluetoe.svg?branch=master)](https://travis-ci.org/TorstenRobitzki/bluetoe) [![Join the chat at https://gitter.im/TorstenRobitzki/bluetoe](https://badges.gitter.im/TorstenRobitzki/bluetoe.svg)](https://gitter.im/TorstenRobitzki/bluetoe?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/TorstenRobitzki/bluetoe.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/TorstenRobitzki/bluetoe/context:cpp)

![Bluetoe Logo](https://github.com/TorstenRobitzki/bluetoe/blob/master/documentation/Logo_Bluetoe.svg?raw=true)

## Putin's Invasion of Ukraine

Please consider donating to one of the funds that help victims of the war in Ukraine:
- https://spendenkonto-nothilfe.de

## Overview

Bluetoe implements a GATT server with a very low memory footprint and a convenience C++ interface. Bluetoe makes easy things easy but gives the opportunity to fiddle with all the low level GATT details if necessary. The main target of Bluetoe is to be implemented on very small microcontrollers. Here is a complete example of a small GATT server, that allows a client to controll an IO pin, running on a nrf52832:

    #include <bluetoe/server.hpp>
    #include <bluetoe/device.hpp>
    #include <nrf.h>

    using namespace bluetoe;

    // LED1 on a nRF52 eval board
    static constexpr int io_pin = 17;

    static std::uint8_t io_pin_write_handler( bool state )
    {
        // on an nRF52 eval board, the pin is connected to the LED's cathode, this inverts the logic.
        NRF_GPIO->OUT = state
            ? NRF_GPIO->OUT & ~( 1 << io_pin )
            : NRF_GPIO->OUT | ( 1 << io_pin );

        return error_codes::success;
    }

    using blinky_server = server<
        service<
            service_uuid< 0xC11169E1, 0x6252, 0x4450, 0x931C, 0x1B43A318783B >,
            characteristic<
                requires_encryption,
                free_write_handler< bool, io_pin_write_handler >
            >
        >
    >;

    blinky_server gatt;

    device< blinky_server > gatt_srv;

    int main()
    {
        // Init GPIO pin
        NRF_GPIO->PIN_CNF[ io_pin ] =
            ( GPIO_PIN_CNF_DRIVE_S0H1 << GPIO_PIN_CNF_DRIVE_Pos ) |
            ( GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos );

        for ( ;; )
            gatt_srv.run( gatt );
    }

## Documentation

http://torstenrobitzki.github.io/bluetoe/

## L2CAP

Bluetoe ships with its own link layer. Currently a link layer based on the nrf52832 is implemented and usable. The link layer implementation will be based and tested on an abstract device, called a scheduled radio and should be easily ported to similar hardware. As Bluetoe is a GATT server implementation, only that parts of the link layer are implemented, that are nessary. Bluetoe can easily adapted to any other existing L2CAP implementation (based on HCI for example).

## Current State

The following table show the list of GATT procedures and there implementation status and there planned implementation status:

Feature | Sub-Procedure | Status
--------|---------------|-------
Server Configuration|Exchange MTU|implemented
Primary Service Discovery|Discover All Primary Services|implemented
<br/> |Discover Primary Service By Service UUID|implemented
Relationship Discovery|Find Included Services|implemented
<br/> |Declare Secondary Services|implemented
Characteristic Discovery|Discover All Characteristic of a Service|implemented
<br/> |Discover Characteristic by UUID|implemented
Characteristic Descriptor Discovery|Discover All Characteristic Descriptors|implemented
Characteristic Value Read|Read Characteristic Value|implemented
<br/> |Read Using Characteristic UUID|implemented
<br/> |Read Long Characteristic Value|implemented
<br/> |Read Multiple Characteristic Values|implemented
Characteristic Value Write| Write Without Response|implemented
<br/> |Signed Write Without Response|not planned
<br/> |Write Characteristic Value|implemented
<br/> |Write Long Characteristic Values|implemented
<br/> |Characteristic Value Reliable Writes|implemented
Characteristic Value Notification|Notifications|implemented
Characteristic Value Indication|Indications|implemented
Characteristic Descriptor Value Read|Read Characteristic Descriptors|implemented
<br/> |Read Long Characteristic Descriptors|implemented
Characteristic Descriptor Value Write|Write Characteristic Descriptors|implemented
<br/> |Write Long Characteristic Descriptors|implemented
Cryptography|Encryption|implemented
<br/> |Authentication|planned

This is the current state of implemented Advertising Data:

Advertising Data|Format|Status
----------------|------|------
Service UUID|Incomplete List of 16-bit Service UUIDs|implemented
<br/> |Complete List of 16-bit Service UUIDs|implemented
<br/> |Incomplete List of 32-bit Service UUIDs|not planned
<br/> |Complete List of 32-bit Service UUIDs|not planned
<br/> |Incomplete List of 128-bit Service UUIDs|implemented
<br/> |Complete List of 128-bit Service UUIDs|implemented
Local Name|Shortened Local Name|implemented
<br/> |Complete Local Name|implemented
Flags|Flags|implemented
Manufacturer Specific Data|Manufacturer Specific Data|planned
TX Power Level|TX Power Level|planned
Secure Simple Pairing Out of Band||not planned
Security Manager Out of Band||not planned
Security Manager TK Value||not planned
Slave Connection Interval Range||not planned
Service Solicitation||not planned
Service Data||not planned
Appearance|Appearance|implemented
LE Role|LE Role|planned

This is the current state of the Link Layer implementation:

Aspect | Feature | Status
-------|---------|--------
Roles|Slave Role|implemented
<br/> |Master Role|not planned
Advertising|connectable undirected advertising|implemented
<br/> |connectable directed advertising|implemented
<br/> |non-connectable undirected advertising|implemented
<br/> |scannable undirected advertising|implemented
Device Filtering||implemented
Connections|Single Connection|implemented
<br/> |Multiple Connection|not planned
Connection|Slave Latency|planned
Feature Support|LE Encryption|implemented
<br/> |Connection Parameters Request Procedure|implemented
<br/> |Extended Reject Indication|planned
<br/> |Slave-initiated Features Exchange|planned
<br/> |LE Ping|implemented
<br/> |LE Data Packet Length Extension|planned
<br/> |LL Privacy|not planned
<br/> |Extended Scanner Filter Policies|not planned

Pullrequests are wellcome.

## Dependencies
- boost for Unittests
- CMake for build
- a decent C++ compiler supporting C++11

# Current Measurements

All measurements done without any traffic and a slave latency of 0.
Average current measured with different connection intervals.

## nRF52840 blinky without encryption

- Calibrated RC Sleep Clock (500ppm) (13608 Bytes binary size)
  * 546µA average at 7.5ms
  * 283µA average at 15ms
  * 155µA average at 30ms

- Crystal Oscilator Sleep Clock (20ppm) (13364 Bytes binary size)
  * 526µA average at 7.5ms
  * 207µA average at 15ms
  * 143µA average at 30ms
  * 22.4µA average at 660ms

## nRF52840 blinky with encryption

- Calibrated RC Sleep Clock (500ppm) (22848 Bytes binary size)
  * 613µA average at 7.5ms
  * 211µA average at 30ms
  * 113µA average at 660ms

- Crystal Oscilator Sleep Clock (20ppm) (22608 Bytes binary size)
  * 589µA average at 7.5ms
  * 197µA average at 30ms

- Synthesized Sleep Clock (40ppm) (22596 Bytes binary size)
  * 1.09mA average at 7.5ms
  * 776µA average at 30ms

- Old radio implementation (40ppm) (22212 Bytes binary size)
  * 1.08mA average at 7.5ms
  * 874µA average at 15ms
  * 770µA average at 30ms
  * 674µA average at 660ms

## nRF52820 blinky with encryption

- Calibrated RC Sleep Clock (500ppm / 1ms HFXO startup time) (21880 Bytes binary size)
  * 347µA average at 15ms
  * 217µA average at 30ms
  * 94µA average at 660ms

- Calibrated RC Sleep Clock (500ppm / 0.3ms HFXO startup time) (21880 Bytes binary size)
  * 316µA average at 15ms
  * 103µA average at 660ms



