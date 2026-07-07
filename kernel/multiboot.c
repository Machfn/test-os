#include <stdint.h>
#include "tty.h"
#include "../libc/include/string.h"

#include "multiboot.h"


struct os_info_cpu cpu_info;
struct os_info_ram ram_info;
struct os_sys_enc enc_info;

const char *get_smbios_string(struct smbios_header *header, uint8_t index)
{
    if (index == 0) {
        return "Unknown";
    }

    char *str_ptr = (char *)header + header->length;

    for (uint8_t i = 1; i < index; ++i) {
        while (*str_ptr != '\0') {
            str_ptr++;
        }
        str_ptr++; // Go past null to next string
    }
    return str_ptr;
}

int process_smbios_type(struct smbios_header *header) 
{    
    // printf("\ntype=%d len=%d", header->type, header->length);
    if (header->type == 127) {
        return 1;
    }

    /** @note Header type 3 is enclosure info */
    if (header->type == 3) {
        struct smbios_type3_sys_enclosure *sys_enc = (struct smbios_type3_sys_enclosure *)header;
        enc_info = (struct os_sys_enc) {
            .initialized = 1,
            .manufacturer = get_smbios_string(header, sys_enc->manufacturer),
            .version = sys_enc->version,
        };
    }

    /** @note Header type 4 is cpu info */
    if (header->type == 4) {
        struct smbios_type4_cpu *cpu = (struct smbios_type4_cpu *)header;
        cpu_info = (struct os_info_cpu) {
            .initialized = 1,
            .core_count = cpu->core_count,
            .max_speed = cpu->max_speed,
            .part_number = cpu->part_number,
            .voltage = cpu->voltage,
            .processor_manufacturer = get_smbios_string(header, cpu->processor_manufacturer),
        };
        return 0;
    }
    /** @note Header type 17 is ram INFO */
    if (header->type == 17) {
        struct smbios_type17_ram *ram = (struct smbios_type17_ram *)header;
        ram_info = (struct os_info_ram) {
            .initialized = 1,
            .raw_size = ram->size,
            .memory_type = ram->memory_type,
            .mb_size = (ram->size & 0x7FFF), // If raw_size bit 15 is 0, size is in MB. If 1, size is in KB.
        };
        return 0;
    }
    return 0;
}

struct smbios_eps *find_smbios_eps(void) // Should add a check for safety with mbi
{
    uint8_t *base_addr = (uint8_t *)0xF0000;
    while ((uint32_t)base_addr < 0x100000) {
        if (*(uint32_t *)base_addr == 0x5F4D535F) {
            // Maybe checksum check
            struct smbios_eps *eps = (struct smbios_eps *)base_addr;
            // printf("eps len=%d structs=%d table=%d\n", eps->length, eps->number_of_structures, eps->structure_table_address);
            return eps;
        }
        base_addr += 16;
    }
    return 0;
}

void parse_smbios_tables(struct smbios_eps *eps) 
{
    uint8_t *table_base = (uint8_t *)(uint32_t)eps->structure_table_address;
    uint8_t *current = table_base;

    for (int i = 0; i < eps->number_of_structures; ++i) {
        struct smbios_header *header = (struct smbios_header *)current;

        int err = process_smbios_type(header);
        if (err) {
            break;
        }
     
        uint8_t *scan_ptr = current + header->length;

        while (*(uint16_t *)(scan_ptr) != 0x0000) { // Double null marks end
            scan_ptr++;
        }

        current = scan_ptr + 2;
    }

}

void print_sys_info(void)
{
    printf("\n");
    if (enc_info.initialized) {
        printf("Enclosure Info: ");
        printf("\n Manufacturer: %s", enc_info.manufacturer);
        printf("\n Version: %d", enc_info.version);
        printf("\n");
    }

    if (cpu_info.initialized) {
        printf("Cpu Information: \n");
        printf(" Manufacturer: %s", cpu_info.processor_manufacturer);
        printf("\n Voltage: %d", cpu_info.voltage);
        printf("\n Max Speed: %d", cpu_info.max_speed);
        printf("\n Core Count: %d", cpu_info.core_count);
        printf("\n");
    }
    
    if (ram_info.initialized) {
        printf("Ram Information: \n");
        printf("\n Size: %d Mb", ram_info.mb_size);
        printf("\n");
    }
    
    if (!ram_info.initialized && !cpu_info.initialized && !enc_info.initialized) {
        printf("NO Smbios data found!\n");
    }

}

