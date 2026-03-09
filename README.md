# MULTEC MONITOR

Materials related to FSO Polonez ("Ponton") ECUs, primarily the 1.6L MPI Delphi/Multec ECU.

This repository contains:
- A design for an alternative diagnostic cable for the Multec Monitor software.
- A COM port tester program for MS-DOS.
- An Arduino-based emulator for the Multec ECU signal.
- A COM port latency checker for Linux PCs.
- A fix for the Polonez Bosch MA1.7 ECU that enables it to be diagnosed using VCDS.

![MONITOR_RUNNING](PHOTOS/MONITOR_RUNNING.jpg)

![MULTEC_DIAG](PHOTOS/MULTEC_DIAG.jpg)

# Disclaimer

I do not consent to the use of all or part of the project for commercial purposes!

Nie wyrażam zgody na wykorzystanie całości bądź części projektu w celach zarobkowych!

# License

Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg

# Directories organization

- **software** - PC and Arduino software written in C.
- **PCB** - The diagnostic cable PCB design, created in KiCad.
- **BOSCH_ECU** - A Python-based fix for the Polonez Bosch MA1.7 ECU that enables diagnostics using VCDS.

# Developer cable

Compared to the simple resistor-based version, this cable has the following advantages:

- The use of an optocoupler allows diagnostics to be performed on most Polonez ("Ponton") vehicles where the Check Engine (CEL) indicator has been removed. The ECU’s 160-baud signal output is an open-collector (OC) type, so without proper biasing the signal disappears together with the bulb.

- The MAX232 IC ensures correct RS-232 voltage levels at the computer connector, regardless of whether the PC provides a sufficiently low voltage on the RTS pin, and regardless of the condition of the vehicle battery.

**PIN** | **SIGNAL** | **WIRE COLOR**
:---: | :---: | :---:
1 | VCC | Red
2 | CTS | White
3 | #CTS | Yellow
4 | TX-RX | Green
5 | GND | Brown

![DEV_UNIT_TOP](PHOTOS/DEV_UNIT_TOP.jpg)

![DEV_UNIT_BOTTOM](PHOTOS/DEV_UNIT_BOTTOM.jpg)
