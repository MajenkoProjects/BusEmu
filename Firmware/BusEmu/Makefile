
# This is what the board identifies as for rebooting
PORT=/dev/ttyACM0

# What the board is
BOARD=chipKIT:pic32:ck-mz-lite
OPT=USB=z_custom

# Targets:
#
#     make          
#         Compile the ino
#
#     make install
#         Compile the ino and upload it to the board
#
#     make watch
#         Wait for changes to the file and compile automatically. Requires
#         `entr` to operate (sudo apt install entr)
#
#     make clean
#         Delete the build folder

SKETCH=$(shell basename $$(pwd))
INO=${SKETCH}.ino
SUBDIR=$(subst :,.,${BOARD})
BIN=bin/${INO}.hex

${BIN}: ${INO} 
	@echo "Sketch is" ${INO}
	@arduino-cli compile -b ${BOARD}:${OPT} --output-dir bin --build-path build ${INO}

install: ${BIN}
	@arduino-cli upload -b ${BOARD}:${OPT} --input-dir bin

clean:
	rm -rf build bin

watch:
	echo ${INO} | entr -c -s 'make'

