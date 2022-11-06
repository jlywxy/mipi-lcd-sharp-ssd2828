# MIPI-DSI Interface LCD Driver for Sharp LS050T1SX01 using SSD2828

Caution: this project is in a working process and not been done. Part of the document has not finished.
<br>
```
Current Progress

[ok] LCD with Specification
[ok] SSD2828 SPI Transmission and Initialization
[  ] SSD2828 and LCD MIPI Configuration
[ok] Circuit Schematic
[  ] PCB Layout and MIPI Differential Layout
[  ] First Test and Debug
[  ] Final Test
```
- --
This project is managing to drive Sharp LS050T1SX01 using SSD2828(Solomon Systech).
```
(any)  |  <- RGB interface
       v
       ---->  SSD2828 ------> Sharp LS050T1SX01
       ^                 ^ MIPI DSI
STM32  |  <- SPI
```
Author: jlywxy<br>
Document Version: 1.0.1
- --
## Circuit Schematic Suggestions

1. Use reverse voltage generator IC for LCD AVDD+/-.<br>
Currently using ICL7660 from Renesas.
2. To be continued
- --
## PCB Layout and Manufacturing Suggestions
1. For MIPI Differential Layout
* Do not use LCEDA to tune line length using the tuning function only for single-ended network. Use a more professional tool such as KiCad(in this project) or Altium, then use their tuning function for two lines of the differential, concurrently.
* The MIPI lines must cross the connector on the reverse side of the connector, and make via holes between the pads in the connector.
```
------------------......
| | | | | | |
 ø ø ø ø ø ø ......
ø ø ø ø ø ø 
 | | | | | | | ......
------------------

("|"=pad, "ø"=via-holes)
```
2. For connector soldering and layout
* Do not use heat gun to solder plastic components, even if temperature is lower than 220(C).
* Do not use low temperature soldering tin (accurately 138(C)Bi-Sn), which is not rock-hard then spliting apart and cause <b>rosin joint</b>
* Make connector direction easy for LCD connection.
* Make soldering pad bigger to conveniently solder connector on the board.
3. To be continued
- --
## Overview of the LCM Interface 

### 1. Display Interface

This LCM is using MIPI DSI and DCS interface.
```
Sharp LS050T1SX01

Electrical-Level    | Speed       
--------------------------------------------------
1.2v-330mV-MIPI DSI | max 500 MHz 

Wires                    | VDD Voltage
--------------------------------------------------
1port,4lanes(2CLK+8Data) | 1.8/3.3v(digital), +5v & -5v(analog)
```

### 2. Backlight Interface

This LCM requires 19.8v dual power with each 20mA current without internal backlight driver.

### 3. Connector

The part number of Mating connector is AYF333135 of Panasonic, which has 31pins with crossing 0.3mm interval.

<b>It's recommended to use other brands of connector, such as FH26W-31S from Hirose, which has more sustainability after soldering by iron.</b>

This is the only one connector on the LCM to provide data/control/backlight lines.
- --

## Availability Test
1. Since this LCM need signal converting, a PCB is made, include power management.

< TO BE CONTINUED >

2. The test method of availability is to show color flow gradient animation.

< TO BE CONTINUED >

- --

## Display Workflow(Steps to light up display)

0. Backlight power on.
1. LCM VDD on, XRES.
2. Initialize SSD2828 via SPI, Set LCD characteristics
2. Initialize LCM by sending DCS init commands to SSD2828 via SPI, then SSD2828 routes data via MIPI LP.
3. Start data transmission to SSD2828 via RGB interface, then SSD2828 routes data via MIPI HS. / Or, start SSD2828 BIST(Built-In Self Test) without RGB input.

- --
## Misc

### LCM Optical Characteristics

```
Sharp LS050T1SX01

Pixel-Arrangement   | Panel-Type | Color-Depth
-------------------------------------------------
RGB vertical stripe | IPS        | 8-bit(16.7M)


Contrast | Color-Chromaticity | Backlight
-------------------------------------------------
1000:1   | 72% NTSC(sRGB)     | 450 nits
```

### Knowledge Bases of Concepts
1. MIPI DSI
* The MIPI Alliance defines modern interface of mobile devices like phones, including display, cameras, etc. 

< TO BE CONTINUED >

2. MIPI DPI & DCS


* MIPI-DPI is one of the MIPI display interface series, which is well known as RGB/Parallel/LTDC interface. This interface splits control lines(HSYNC/VSYNC/DE) with data lines(RGB parallel lines). Since it uses single-ended signals(compared to MIPI-DSI), the max speed(clock speed) could be limited, but it can transfer full pixel data in one clock period(compared to serial interfaces). The color depth is configurable as RGB565/RGB666/RGB888 and more, which could also be 'hacked' to leave out some pins or branch some lines(when downsampling color depth, throw away certain LSB; when upsampling, branch certain MSB to LSB or connect certain LSB to GND).
```
RGB888 (typical format of 16.7M color display)
-------------------------------------------------
       RRRRRRRR GGGGGGGG BBBBBBBB (3 bytes)

RGB666 (typical format of 262k color display, and 16.2M color TN panels with FRC)
-------------------------------------------------
             RR RRRRGGGG GGBBBBBB (18 bits)

RGB565
-------------------------------------------------
                RRRRRGGG GGGBBBBB (2 bytes)

RGB101010 (not available in most of the displays, typical format of 1.07B color screen)
-------------------------------------------------
RRRRRR RRRRGGGG GGGGGGBB BBBBBBBB (30 bits)
```

4. LCM

* A abbreviation of Liquid Crystal Module, which includes LCD glass panel and backlight LEDs.

5. Low-Temperature Soldering Tin

* A type of soldering tin which melting point is only 138 celsius high. It has 58% Bismuth(Bi) and 42% Stannum(Sn) to make such a low melting point. This temperature is so important for those not heat-resistant components such as LEDs and MEMS components. However, it's definitely not recommended for connector soldering, which will cause unsoldering and rosin-joint, because this type of soldering tin is fragile.

### Additional Information of this Project
From the start of this project, it was considered to use Chinese chips with more functionability as it said: Lontium LT7911D, LT8918, etc., but what's the most confusing is chips of most Chinese makers always did not open to share their product specifications, but only provide those informations to large conpanies or via unofficial way to give to personal DIY makers. 

Most of the DIY makers that cannot contact the company are not able to do design using these chips, the only way to get datasheets is to find those from unofficial paid document websites or 'BBS which requires download coins', indicating for the project requires correctness and availability to avoid using Chinese chips without official support.

Additionally, some of the Lontium resellers on the e-commerce websites even selling LT7911D DEMO boards (with basic circuits to drive the chip) up to about 30 dollars(400+ Chinese yuan) or more. Consider that SSD2828 was only priced below 1 dollar(8 Chinese yuan) per chip..!

Excepting Solomon-Systech and Lontium solutions, the only left MIPI chip solution provider is Toshiba(東芝). However, it only provide chips in BGA that hard to work with soldering iron and bare hand.