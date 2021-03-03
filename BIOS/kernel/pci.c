#include "pci.h"
#include "../libc/stdio.h"
#include "hardware.h"

// in the past we called commands as regs, fix that nomenclature
#define DATA_PORT 	0xCFC
#define COMMAND_PORT 	0xCF8

// PCI Controller (up to):
// 	8 BUSes
// 	32 "devices" (slots) \ bus
// 	8 functions \ slot
//
// CONFIG_ADDRESS register bit format:
// 	31    : enable bit 
// 	32-24 : reserved
// 	23-16 : bus number
// 	15-11 : device number
// 	10-8  : function number
// 	7-0   : register offset


// Construct Configuration Space Access CONFIG_ADDRESS payload
uint32_t construct_identifier(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
	uint32_t lbus  = (uint32_t) bus;
	uint32_t lslot = (uint32_t) slot;
	uint32_t lfunc = (uint32_t) func;

	// create configuration address (identifier)
	uint32_t id = (uint32_t)
		      ( (0x01  << 31  )
		      | (lbus  << 16  ) 
		      | (lslot << 11  )
		      | (lfunc << 8   )
		      | (offset & 0xFC)
		      //| ((uint32_t) 0x80000000)
		      );

	return id;
}

uint32_t read(uint8_t bus, uint8_t slot, uint8_t function, uint8_t reg_offset) {
	// construct configuration space address
	uint32_t id = construct_identifier(bus, slot, function, reg_offset);

	// write data to PCIC command port
	ports_outl(COMMAND_PORT, id);

	// read the result from the data port
	uint32_t result = ports_inl(DATA_PORT);

	return result >> (8*(reg_offset % 4));
}



void write(uint8_t bus, uint8_t slot, uint8_t function, uint8_t reg_offset, uint32_t value) {
	// construct configuration space address 
	uint32_t id = construct_identifier(bus, slot, function, reg_offset);

	// write data to PCIC command port
	ports_outl(COMMAND_PORT, id);
	ports_outl(DATA_PORT, value);
}

//rather than bruteforce every function, first ask each device if they have functions or not 
bool device_multifunction(uint8_t bus, uint8_t slot) {
	return read(bus, slot, 0, 0x0E) & (1<<7); //7th bit tells us 
}

// at some point we want to select drivers based on info we find here
//void select_drivers(drivermanager *manager) {}


struct device_descriptor_struct {
	uint32_t portBase;
	uint32_t interrupt;

	uint16_t bus;
	uint16_t slot;
	uint16_t function;

	uint16_t vendor_id;
	uint16_t device_id;

	uint8_t class_id;
	uint8_t subclass_id;
	uint8_t interface_id;

	uint8_t revision;
}; 
typedef struct device_descriptor_struct device_descriptor_t;

// get device descriptor
device_descriptor_t get_device_descriptor(uint8_t bus, uint8_t slot, uint8_t function) {
		device_descriptor_t result;

		result.bus          = bus;
		result.slot       = slot;
		result.function     = function;

		result.vendor_id    = read(bus, slot, function, 0x00);
		result.device_id    = read(bus, slot, function, 0x02);

		result.class_id     = read(bus, slot, function, 0x0E);
		result.subclass_id  = read(bus, slot, function, 0x0A);
		result.interface_id = read(bus, slot, function, 0x09);

		result.revision     = read(bus, slot, function, 0x08);
		result.interrupt    = read(bus, slot, function, 0x3C);


		return result;
}


// untill we have dynamic mem alloc...
typedef struct {
	uint16_t count;
	device_descriptor_t devices[15]; // assume 15 for now
} pci_descriptor_t;


// for now lets just list
void pci_explore(void) {
	pci_descriptor_t pci_list = {0};

	for (uint8_t bus=0; bus < 8; bus++) {
		for (uint8_t slot=0; slot<32; slot++) {
			// it must have at least one function
			uint8_t number_of_functions= device_multifunction(bus, slot) ? 8:1;

			for (uint8_t function=0; function < number_of_functions; function++) {
				device_descriptor_t dev = get_device_descriptor(bus, slot, function);
					
				// vendor id will be either zero or all 1 if there is no device on that function
				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF)
					continue; 
				
				pci_list.devices[pci_list.count]=dev;
				pci_list.count++;

				printf("\tPCI BUS: %d, SLOT: %d, FUNCTION: %d = VENDOR: %x%x DEVICE: %x%x\n", 
						(bus & 0xFF), (slot & 0xFF), (function & 0xFF), 
						((dev.vendor_id & 0xFF00) >> 8), (dev.vendor_id & 0x00FF), 
						((dev.device_id & 0xFF00) >> 8), (dev.device_id & 0x00FF)
					);
			}
		}
	}
}
