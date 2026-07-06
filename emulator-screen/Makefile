TARGET:= emulator-screen # name of the final executable
include ../ch32v003fun/ch32v003fun.mk

# Custom clean rule to remove additional files generated during the build process
clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).bin $(TARGET).map $(TARGET).lst \
		$(TARGET).o $(TARGET).d $(TARGET).srec $(TARGET).sym $(TARGET).lss
# step 1: clean the emulator build
	$(MAKE) -f Makefile-emulator clean

# Custom rule name
emulator:
# step 1: clean the emulator build
	$(MAKE) -f Makefile-emulator clean 
# step 2: build the emulator
	$(MAKE) -f Makefile-emulator
