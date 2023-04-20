#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <hidapi.h>

#define MAX_STR 255

#define FT_DATA_SETUP 2
#define FT_SETTLE 3
#define FT_CLOCK_DEL 4
#define FT_CLOCK_LEN 5
#define FT_OP_LEN 6
#define FT_RELEASE 7

void set_feature(hid_device *h, int f, int v) {
	char buf[2];
	buf[0] = f;
	buf[1] = v;
	hid_send_feature_report(h, buf, 2);
}

void write_val(hid_device *h, int a, int v) {
	char buf[3];
	buf[0] = 0x00;
	buf[1] = a;
	buf[2] = v;
	hid_write(h, buf, 3);
}

int read_val(hid_device *h, int a) {
	char buf[3];
	buf[0] = 0x00;
	buf[1] = a;
	hid_write(h, buf, 2);
	hid_read_timeout(h, buf, 1, 1000);
	return buf[0];
}

int main(int argc, char **argv) {

	hid_device *handle;
	int res;
	wchar_t mfg[MAX_STR];
	wchar_t prod[MAX_STR];
	int opt;
	int address;
	int dataval;

	res = hid_init();
	handle = hid_open(0xf055, 0x8088, NULL);
	if (!handle) {
		fprintf(stderr, "Error: unable to open BusEmu device\n");
		return -1;
	}

	hid_get_manufacturer_string(handle, mfg, MAX_STR);
	hid_get_product_string(handle, prod, MAX_STR);

	printf("Found %ls from %ls\n", prod, mfg);


	while ((opt = getopt(argc, argv, "b:a:w:r:C:")) != -1) {
		switch (opt) {
			case 'b':
				if (strcmp(optarg, "8088") == 0) {
					set_feature(handle, 1, 0);
				}
				break;
			case 'C':
				set_feature(handle, FT_CLOCK_DEL, strtol(optarg, NULL, 0));
				break;
			case 'a':
				address = strtol(optarg, NULL, 0);
				break;
			case 'w':
				dataval = strtol(optarg, NULL, 0);
				printf("%02x => %02x\n", dataval, address);
				write_val(handle, address, dataval);
				break;
			case 'r':
				dataval = read_val(handle, address);
				printf("%02 <= %02\n", dataval, address);
				break;
		}
	}

	hid_close(handle);

	res = hid_exit();

	return 0;
}
