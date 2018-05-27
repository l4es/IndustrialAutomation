CAN example

Example description
This example describes how to use CAN peripheral on LPC17xx and LPC40xx chip.
It requires a CAN analyser or, another CAN device connected to CAN_1 pins.
After initializing, the device will setup the AF Look-up table which defines
acceptable IDs. Then, it transmits first standard message with ID 0x200 on bus,
a remote message with ID 0x300 and an extended message with ID 0x10000200.
The device always listens message on the bus. Whenever a message whose ID exists in
the AF Look-up Table comes, the device will reply back a message with ID 0x200. The
data in replied message is the same with the one of received message.

AF Lookup Table:
	Full CAN Section: includes 0x03, 0x04, 0x07, 0x09.
Individual Standard ID Section: includes 0x30, 0x40, 0x50, 0x90, 0xB0.
Standard ID Range Section: includes 0x300~0x400, 0x400~0x500, 0x700~0x780.
Extended ID Section: includes 0x803, 0x804, 0x807, 0x809.
Extended ID Range Section: includes 0xB00~0xC00, 0xC00~0xD00, 0xF00~0xF80.

Special connection requirements
- Embedded Artists' LPC1788 Developer's Kit:
- Embedded Artists' LPC4088 Developer's Kit:
Set jumpers as follows:
	JP14: 3-4 ON
	J16-7 (CAN1-L) is connected to CAN-L (Pin 2) of CAN Analyser.
	J16-8 (CAN1-H) is connected to CAN-H (Pin 7) of CAN Analyser.
	GND pin is connected to GND (pin 3) of CAN Analyser.
- LPCXpresso LPC1769:
Need to connect with base board for using RS232/UART port and CAN transceiver.
	J15-7 (CAN-L) of base board is connected to CAN-L (Pin 2) of CAN Analyser.
	J15-8 (CAN-H) of base board is connected to CAN-H (Pin 7) of CAN Analyser.
	GND pin is connected to GND (pin 3) of CAN Analyser.

Build procedures:
Visit the LPCOpen quickstart guides at link "http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-v200-quickstart-guides"
to get started building LPCOpen projects.

