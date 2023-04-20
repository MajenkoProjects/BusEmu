USBHS usbDevice;
USBManager USB(usbDevice, 0xf055, 0x8088, "Majenko Technologies", "Bus Emulator");
HID_Raw HID;

/*
 Output report format:
 
 One or two bytes.
 If one byte, read operation is started at address
 specified in first byte.

 If two bytes, write operation is started using data in
 second byte, to address in first byte.

 "Feature" list (control parameters)

 1: Bus format
    0: 8088 / 8086
	1: Z80
	2: 6502
	3: 6800
 2: Data Setup Time (uS)
 3: Settle Time (uS)
 4: Clock Delay (uS)
 5: Clock Length (uS)
 6: Operation Length (uS)
 7: Release Delay (uS)
*/

/* 
 8088 bus cycle

 4 clock cycles:
 1. Set up address
 2. Set up data
 3. Perform write
 4. Release bus

 Pins:

 0-7 = D0-7
 8-11 = A0-A3
 12 = CLK
 13 = WR
 14 = RD
*/

#define FT_DATA_SETUP 2
#define FT_SETTLE 3
#define FT_CLOCK_DEL 4
#define FT_CLOCK_LEN 5
#define FT_OP_LEN 6
#define FT_RELEASE 7

#define PIN_D0 44
#define PIN_D1 43
#define PIN_D2 42
#define PIN_D3 41
#define PIN_D4 40
#define PIN_D5 39
#define PIN_D6 38
#define PIN_D7 37
#define PIN_A0 36
#define PIN_A1 35
#define PIN_A2 34
#define PIN_8088_CLK 33
#define PIN_8088_WR 32
#define PIN_8088_RD 31

void write_8088(uint8_t addr, uint8_t data) {
	for (int i = 31; i <= 44; i++) {
		pinMode(i, OUTPUT);
	}
	digitalWrite(PIN_8088_CLK, HIGH);
	digitalWrite(PIN_8088_WR, HIGH);
	digitalWrite(PIN_8088_RD, HIGH);


	// Clock 1 - set the address
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_A0, addr & 0x01 ? HIGH : LOW);
	digitalWrite(PIN_A1, addr & 0x02 ? HIGH : LOW);
	digitalWrite(PIN_A2, addr & 0x04 ? HIGH : LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 2 - set the data
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_D0, data & 0x01 ? HIGH : LOW);
	digitalWrite(PIN_D1, data & 0x02 ? HIGH : LOW);
	digitalWrite(PIN_D2, data & 0x04 ? HIGH : LOW);
	digitalWrite(PIN_D3, data & 0x08 ? HIGH : LOW);
	digitalWrite(PIN_D4, data & 0x10 ? HIGH : LOW);
	digitalWrite(PIN_D5, data & 0x20 ? HIGH : LOW);
	digitalWrite(PIN_D6, data & 0x40 ? HIGH : LOW);
	digitalWrite(PIN_D7, data & 0x80 ? HIGH : LOW);
	digitalWrite(PIN_8088_WR, LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 3 - Perform the write
	digitalWrite(PIN_8088_CLK, LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 4 - release
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_8088_WR, HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	for (int i = PIN_D7; i <= PIN_D0; i++) {
		pinMode(i, INPUT_PULLUP);
	}
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
}

uint8_t read_8088(uint8_t addr) {
	uint8_t val = 0;
	for (int i = 31; i <= 36; i++) {
		pinMode(i, OUTPUT);
	}
	for (int i = 37; i <= 44; i++) {
		pinMode(i, INPUT_PULLUP);
	}
	digitalWrite(PIN_8088_CLK, HIGH);
	digitalWrite(PIN_8088_WR, HIGH);
	digitalWrite(PIN_8088_RD, HIGH);

	// Clock 1 - set the address
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_A0, addr & 0x01 ? HIGH : LOW);
	digitalWrite(PIN_A1, addr & 0x02 ? HIGH : LOW);
	digitalWrite(PIN_A2, addr & 0x04 ? HIGH : LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 2 - set second part of address (nothing here)
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_8088_RD, LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK,HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 3 - do the read
	digitalWrite(PIN_8088_CLK, LOW);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK, HIGH);
	val |= (digitalRead(PIN_D0) ? 0x01 : 0x00);
	val |= (digitalRead(PIN_D1) ? 0x02 : 0x00);
	val |= (digitalRead(PIN_D2) ? 0x04 : 0x00);
	val |= (digitalRead(PIN_D3) ? 0x08 : 0x00);
	val |= (digitalRead(PIN_D4) ? 0x10 : 0x00);
	val |= (digitalRead(PIN_D5) ? 0x20 : 0x00);
	val |= (digitalRead(PIN_D6) ? 0x40 : 0x00);
	val |= (digitalRead(PIN_D7) ? 0x80 : 0x00);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	// Clock 4 - release
	digitalWrite(PIN_8088_CLK, LOW);
	digitalWrite(PIN_8088_RD, HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));
	digitalWrite(PIN_8088_CLK, HIGH);
	delayMicroseconds(HID.getFeature(FT_CLOCK_DEL));

	return val;
}



void outputReportReceived(uint8_t *data, uint32_t len) {
	uint8_t v;
	if (len == 1) {
		// Read(data[0]);
		switch (HID.getFeature(1)) {
			case 0: // 8088
				v = read_8088(data[0]);
				HID.sendReport(&v, 1);
				break;
		}
	} else if (len == 2) {
		// Write(data[0], data[1]);
		switch (HID.getFeature(1)) {
			case 0: // 8088
				write_8088(data[0], data[1]);
				break;
		}
	}
}

void setup() {
	USB.addDevice(HID);
	USB.begin();

	HID.onOutputReport(outputReportReceived);
}

void loop() {
}

