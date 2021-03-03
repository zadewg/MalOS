#ifndef _PCI_H
#define _PCI_H 1

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


// PCI: 8buses>32 devices/ubus > 8 functions/device
// uint8 class id
// uint8 sublass id
// uint8 vendor id
// uint16 device id
// there are standarized memory sections for the data above, so we need an offset to read too


uint32_t read(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg_offset);

void write(uint8_t bus, uint8_t device, uint8_t function, uint8_t reg_offset, uint32_t value);

void pci_explore(void);


#endif
