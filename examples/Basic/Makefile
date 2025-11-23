SKETCH = $(shell basename "`pwd`")

FQBN = teensy:avr:teensy40

PORT = /dev/ttyACM0

build: $(SKETCH).ino
	arduino-cli compile \
		--libraries $(HOME)/Documents/Arduino/libraries \
		--libraries ../.. \
		--fqbn $(FQBN) $(SKETCH).ino

flash:
	arduino-cli upload -p $(PORT) --fqbn $(FQBN)

edit:
	vim $(SKETCH).ino

listen:
	miniterm.py $(PORT) 115200 --exit-char 3
