# Driving MIPI-DSI LCD(Sharp LS050T1SX01) using SSD2828

This project is testing the PCB layout of MIPI-DSI high-speed differential and driving Sharp LS050T1SX01 using SSD2828(Solomon Systech) and STM32F030F4P6 from scratch.

<img src="monazite-logo-lofi.png" width=80><br>
Author: jlywxy (jlywxy@outlook.com)<br>
Document Version: 1.3r1
- --
## Signal Wiring Brief
```
(any)  |  
       | < (parallel RGB)
       v
    SSD2828 ---------> Sharp LS050T1SX01
       ^          ^ (MIPI-DSI)
       | < (SPI)
STM32  |  
```
- --
## Intention of the project
1. A PCB with MIPI-DSI high-speed differential layouts and reverse voltage generator circuits is made and tested.<br>
Checkout <a href="sharp_dsi_pcb">sharp_dsi_pcb</a> directory for PCB(KiCad).

The MIPI-DSI requires 100-Ohm differental impedance (100-Ohm of differential and 50-Ohm of single-ended). This board uses track width 7.1 mils, track spacing 6mils, track to copper filling area 6 mils, with board thickness of 1.6mm.

2. Test method is using SSD2828 BIST mode and LCD bist mode(provided by R63311) to display color at full screen and using the builtin test mode in the LCD. <br>
The testing signal frequency output from SSD2828 is now 992 MHz (0.99GHz), with ideal frame rate at 120 Hz.<br>
<img src="demo2-boardtest1.jpg" width=300/>

- --

## Circuit Schematic Suggestions

1. Use reverse voltage generator IC for LCD AVDD+/-.<br>
Currently using ICL7660 from Renesas.
2. Put KiCad PWR_FLAG at VBUS/GND at USB-C connector.
3. Use 3.3v LDO to VDDIO for LCD, SSD2828 and STM32.
- --
## PCB Layout and Manufacturing Suggestions
1. For MIPI Differential Layout
* Do not use LCEDA to tune line length using the tuning function only for single-ended network. Use a more professional tool such as KiCad(using in this project)/Altium/Allegro, then use their tuning and simulation function for two lines of the differential concurrently.
* The MIPI lines must cross the connector on the reverse side of the board, and make via holes between the pads in the connector.
```
------------------......
| | | | | | |
 ø ø ø ø ø ø ......
ø ø ø ø ø ø 
 | | | | | | | ......
------------------

("|"=pad, "ø"=via-holes)
```
* Connect Differential GND actively to IC GND pins. KiCad won't fill copper region for those very thin unconnected pad with the most near copper region and cause potential EMI, even if DRC and ERC passed.
* It's not very harsh for the differential layout. The line will work as it is if the impedance calculated correctly and lines close enough. The most fault may be impedance unmatching on the connector, which will be a more impact than line layouts. However, such a big problem on the connector will still not bring up the reason making diffenetial signaling into a false situation, since every high speed signal needs connector, no matter they are FPC connector nor even pins extended from chips, which pin pitch and chip packaging are always limited and varied. For example, the FPC connector used in this project has pin extending and chaning pitch in its mechanical structures, making no impedance warranty; some chips are packaged in 0.8mm-pitch BGA or even smaller, which will not make the exact impedance when applying layout for BGA pin fanouts. 
2. For connector soldering and layout
* Do not use heat gun to solder plastic components, even if temperature is lower than 260(C).
* Do not use low temperature soldering tin (accurately 138(C)Bi-Sn), which is not rock-hard then spliting apart and cause <b>rosin joint</b>
* Make connector direction easy for LCD connection.
* Make soldering pad bigger and longer to conveniently solder connector on the board.
* To solder connector on the board using iron, put tin and rosin on the footprint of PCB, then remove them to make a thin layer. Put tin on the iron header, and put connector on the PCB, use iron header to touch connector pin, especially reload the tin in the pad on the PCB. If tin on the pad are gathering together, dip some rosin to the PCB to make them apart, or use copper strip to suck the tin off. After soldering, a visual check should be applied as: tin should connect the pad surface and connector pin, making a small tin curve; no tin is still gathering.
3. For PCB Manufacturing
* Use KiCad to plot Gerber and drill files, then zip the files and send to manufacture.
* It is tested using JLCPCB to manufacture the board.
The gerber settings for JLCPCB manufacture should be modified as follow:

||-Gerber Options-|
|-|-|
|*[yes]|Use Protel filename extensions|
|*[no]|Generate Gerber job file|
|*[yes]|Subtract soldermask from silkscreen|
4.6, unit mm|Coordinate format|
|*[no]|USe extended X2 format(recommended)|
|*[no]|Include netlist attributes|
|[no]|Disable aperture macros(not recommended)|

||-Drill Units-|
|-|-|
|*[yes]|Millimeters|
|[no]|Inches|

Note: *=modified options compared to the default settings

4. If some IC provided suggested circuit PCB layout, follow their instructions.
- --
## Code Packaging For MIPI Packet Transfer
* Functions implemented: 
```
1. setting/reading SSD2828 registers, 
2. writing MIPI short/long DCS/General registers/commands,
3. reading short DCS/General registers.
```
```c
void SSD_SPI_Write(uint8_t *buf,int length){
    HAL_SPI_Transmit(&hspi1,buf,length,HAL_TIMEOUT);
}
void SSD_SPI_Read(int length, uint8_t *r){
    HAL_SPI_Receive(&hspi1,r,length,HAL_TIMEOUT);
}
void SSD_SPI_ReadWrite(uint8_t *buf, int length, uint16_t *r){
    volatile uint16_t bufr[10];
    HAL_SPI_TransmitReceive(&hspi1, buf, (uint8_t *) bufr, length + 2, HAL_TIMEOUT);
    volatile uint8_t byter0=0|bufr[length]>>1;
    volatile uint8_t byter1=0|((bufr[length]&0b000000001)<<7)|(bufr[length+1]>>2);
    r[0]=byter0;
    r[1]=byter1;
}
uint16_t SSD_SPI_ReadReg(uint8_t reg){
    uint16_t tbuf[2];
    tbuf[0]=reg;
    tbuf[1]=0xfa;
    uint16_t rbuf[2];
    SSD_SPI_ReadWrite((uint8_t*)tbuf,2,rbuf);
    return rbuf[0]|rbuf[1]<<8;

}

void SSD_SPI_WriteReg(uint8_t reg,uint16_t data,int len){
    uint16_t buf[3]={reg,1<<8|(data&0xff),1<<8|((data>>8)&0xff)};
    SSD_SPI_Write(buf,len+1);
}

void SSD_MIPI_WriteLongGeneric(uint8_t reg,uint16_t* data,int len){
    SSD_SPI_WriteReg(0xb7,0x0302,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);
    SSD_SPI_WriteReg(0xbc,len,2);
    uint16_t b0[1]={0xbf};
    SSD_SPI_Write(b0,1);
    uint16_t b1[1]={1<<8|reg};
    SSD_SPI_Write(b1,1);
    for(int i=0;i<len-1;i++){
        uint16_t b2[1]={1<<8|data[i]};
        SSD_SPI_Write(b2,1);
    }
}
void SSD_MIPI_WriteLongDCS(uint8_t reg,uint16_t* data,int len){
    SSD_SPI_WriteReg(0xb7,0x0050,2);
    SSD_SPI_WriteReg(0xb8,0x0000,2);
    SSD_SPI_WriteReg(0xbc,len,2);
    uint16_t b0[1]={0xbf};
    SSD_SPI_Write(b0,1);
    uint16_t b1[1]={1<<8|reg};
    SSD_SPI_Write(b1,1);
    for(int i=0;i<len-1;i++){
        uint16_t b2[1]={1<<8|data[i]};
        SSD_SPI_Write(b2,1);
    }

}
void SSD_MIPI_WriteShortGeneric(uint8_t reg,uint16_t data,int len){
    uint16_t b[2]={data&0xff,data>>8};
    SSD_MIPI_WriteLongGeneric(reg,b,len);
}
void SSD_MIPI_WriteShortDCS(uint8_t reg,uint16_t data,int len){
    uint16_t b[2]={data&0xff,data>>8};
    SSD_MIPI_WriteLongDCS(reg,b,len);
}
uint16_t SSD_MIPI_ReadGeneric(uint8_t reg,uint16_t *len, uint16_t *status){
    SSD_SPI_WriteReg(0xb7,0x0382,2);
    SSD_SPI_WriteReg(0xc1,0x0002,2);
    SSD_SPI_WriteReg(0xc0,0x0001,2);

    SSD_SPI_WriteReg(0xbc,0x0001,2);
    SSD_SPI_WriteReg(0xbf,reg,2);
        *len=SSD_SPI_ReadReg(0xc2);
        if((SSD_SPI_ReadReg(0xc6)&1)==0){
            *status=1;
        }else{
            *status=0;
        }
        if(SSD_SPI_ReadReg(0xc3)!=0){
            *status=2;
        }else{
            *status=0;
        }
    return SSD_SPI_ReadReg(0xff);
}
uint16_t SSD_MIPI_ReadDCS(uint8_t reg,uint16_t *len, uint16_t *status){
    SSD_SPI_WriteReg(0xb7,0x03c2,2);
    SSD_SPI_WriteReg(0xc1,0x00ff,2);
    SSD_SPI_WriteReg(0xc0,0x0002,2);

    SSD_SPI_WriteReg(0xbc,0x0001,2);
    SSD_SPI_WriteReg(0xbf,reg,2);
        *len=SSD_SPI_ReadReg(0xc2);
        if((SSD_SPI_ReadReg(0xc6)&1)==0){
            *status=1;
        }else{
            *status=0;
        }
        if(SSD_SPI_ReadReg(0xc3)!=0){
            *status=2;
        }else{
            *status=0;
        }
    return SSD_SPI_ReadReg(0xff);
}
```

- --
## Overview of the LCD Module

### 1. Display Interface

This LCD is using MIPI DSI differential signal interface.


| Signal Electrical-Level | Speed | Driver IC |
|-|-|-|
|1.2v-330mV-MIPI-DSI   | 500 MHz | Renesas R63311

|Signal Wires| Power Supply|
|-|-|
|1-port-MIPI-DSI(4lanes(2 clk, 8 data) | VDD(1.8, 3.3v), Bias(±5v)|


### 2. Backlight Interface

This LCD requires 19.8v dual power with each 20mA current limit without internal backlight driver.

### 3. Connector

The part number of Mating connector is AYF333135 of Panasonic, which has 31pins with crossing 0.3mm interval.

This is the only connector of the LCD to provide data/control/backlight connection.

- --

## Display Workflow(Steps to light up display in Brief)

0. Backlight power on. (Async)
1. LCD/STM32/SSD2828 VDD/MVDD on; ICL7660 V+ on; LCD/SSD2828 XRES.
2. Init SSD2828: set LP mode, PLL, VSYNC/HSYNC, color mode, BIST... 
3. Init LCD via SSD2828: unlock command write, ..., DISPON, SLEEPOFF
4. SSD2828 enters HS Video Mode, starting video transmission.

- --
## Misc

### LCD Module Optical Characteristics


|Pixel-Arrangement   | Panel-Type | Color-Depth|
|---------------------|-------------|---------------|
|RGB vertical stripe | IPS        | 8bit (16.7M)|


|Contrast | Color-Chromaticity | Backlight|
|----------|--------------------|-------------------|
|1000:1   | 72% NTSC           | 450 nits|


### Knowledge Bases of Concepts
1. MIPI DSI
* The MIPI Alliance defines modern interface of mobile devices like phones, including display, cameras, etc. 

<-- To Be Continued -->

2. MIPI DCS: DCS command and Generic command

<-- To Be Continued -->

3. MIPI DPI


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

4. Low-Temperature Soldering Tin

* A type of soldering tin which melting point is only 138 celsius high. It has 58% Bismuth(Bi) and 42% Stannum(Sn) to make such a low melting point. This temperature is so important for those not heat-resistant components such as LEDs and MEMS components. However, it's definitely not recommended for connector soldering, which will cause unsoldering and rosin-joint, because this type of soldering tin is fragile.

### Another performance tests
1. SSD2828 and LCD overclock is viable.(See Availablity part above)
2. Using 3.3v or lower voltage for LCD AVDD+/- may still take the screen 'alive', but with lower brightness.

### Project status
This project works has been done.</b> The document and code comments are still needed to be finshed and polished.
<br>
```
Current Progress

[ok] finding LCD Specification sheet
[ok] finding LCD Driver IC sheet
[ok] SSD2828 SPI Transmission and Initialization setup
[ok] SSD2828 and LCD MIPI configuration setup
[ok] Circuit Schematic design
[ok] PCB Layout and MIPI Differential Layout design
[ok] Test and Debug
(done)
```

### Further Recommendations and Information
1. Use one chip for LCD Backlight(Boost ciruits) and AVDD(Bias), as: TI LM36274, which includes Backlight Boost with Dimming, I2C control, and Bias voltage output.
2. Use the same package of LDOs.
3. Put connector to a more convenient position for LCD connection.
4. Specification datasheets of LCDs, their controllers and standards are not included in this repository, since the author do not have their copyrights and not a member of any alliances(MIPI alliance, VESA, USB-IF, etc). Instead, however, they may be available to the public in some websites, which is the place to get the docs for non-commercial and development purposes.

5. Stay informed of a new project of a USB-C single input (usb-c alternative mode/billboard device, integrated power boost circuits) external display hardware(the "udisplay").

### Document Patch and Errata

* Corrected the fause phrase in the paragraph "Display Workflow": corrected to "Enters HS Mode", rather than formerly "Enters HP Mode". 
* Corrected LCD performance description of the paragraph "Misc".  72% NTSC is not fully equal to 100% sRGB.<br>

patch above: jlywxy@2022.12.18

* Fixed document format problem: charts.<br>

patch above: jlywxy@2023.2.8<br>
