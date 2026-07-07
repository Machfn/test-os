#ifndef MULTIBOOT_H_
#define MULTIBOOT_H_

#define MULTIBOOT1_MAGIC 0x2BADB002 // OS magic

#include <stdint.h>

/**
 * @brief Taken from multiboot 1 specifictaion
 * @link https://web.cecs.pdx.edu/~mpj/llp/references/multiboot.pdf [page 9]
 */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
} __attribute__((packed));

struct multiboot_mmap_entry {
    uint32_t size;
    uint64_t addr;
    uint64_t len;
    uint32_t type;
} __attribute__((packed));

struct smbios_header {
    uint8_t type;
    uint8_t length;
    uint16_t handle;
} __attribute__((packed));

struct smbios_eps {
    uint8_t anchor_string[4]; // We are looking for _SM_
    uint8_t checksum;
    uint8_t length;
    uint8_t major_version;
    uint8_t minor_version;
    uint16_t max_structure_size;
    uint8_t entry_point_revision;
    uint8_t formatted_area[5];
    uint8_t intermediate_anchor[5]; // "DMI"
    uint8_t intermediate_checksum;
    uint16_t structure_table_length;
    uint32_t structure_table_address;
    uint16_t number_of_structures;
    uint8_t bcd_revision;
} __attribute__((packed));

struct smbios_type3_sys_enclosure {
    struct smbios_header header;
    uint8_t manufacturer;
    uint8_t type;
    uint8_t version;
    uint8_t serial_number;
    uint8_t asset_tag_number;
    uint8_t boot_up_state;
    uint8_t power_supply_state;
    uint8_t thermal_state;
    uint8_t security_status;
    uint32_t oem_defined;
    uint8_t height;
    uint8_t number_of_power_cords;
    uint8_t contained_element_count;
    // Add parsing for contained element SKU number
} __attribute__((packed));

struct smbios_type4_cpu {
    struct smbios_header header;
    uint8_t socket_designation;
    uint8_t processor_type;
    uint8_t processor_family;
    uint8_t processor_manufacturer;
    uint64_t processor_id; // Changed from uint64_t
    uint8_t processor_version;
    uint8_t voltage;
    uint16_t external_clock;
    uint16_t max_speed;
    uint16_t current_speed;
    uint8_t status;
    uint8_t processor_upgrade;
    uint16_t l1_cache_handle;
    uint16_t l2_cache_handle;
    uint16_t l3_cache_handle;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t core_count;
    uint8_t core_enabled;
    uint8_t thread_count;
    uint16_t processor_characteristics;
    uint16_t processor_family2;
    uint16_t core_count2;
    uint16_t core_enabled2;
    uint16_t thread_count2;
} __attribute__((packed));

struct smbios_type17_ram {
    struct smbios_header header;
    uint16_t physical_memory_array_handle;
    uint16_t memory_error_information_handle;
    uint16_t total_width;
    uint16_t data_width;
    uint16_t size;              // size of the stick
    uint8_t form_factor;
    uint8_t device_set;
    uint8_t device_locator;
    uint8_t bank_locator;
    uint8_t memory_type;        // (0x1A == DDR4, 0x22 == DDR5)
    uint16_t type_detail;
    uint16_t speed;
    uint8_t manufacturer;
    uint8_t serial_number;
    uint8_t asset_tag;
    uint8_t part_number;
    uint8_t attributes;
    uint32_t extended_size;
    uint16_t configured_memory_speed;
    uint16_t minimum_voltage;
    uint16_t maximum_voltage;
    uint16_t configured_voltage;
    uint8_t memory_technology;
    uint16_t memory_operating_mode_capability;
    uint8_t firmware_version;
    uint16_t module_manufacturer_id;
    uint16_t module_product_id;
    uint16_t memory_subsystem_controller_manufacturer_id;
    uint16_t memory_subsystem_controller_product_id;
    uint64_t non_volative_size;
    uint64_t volatile_size;
    uint64_t cache_size;
    uint64_t logical_size;
    uint32_t extended_speed;
    uint32_t extended_configured_memory_speed;
} __attribute__((packed));

struct os_sys_enc {
    uint8_t initialized;
    const char *manufacturer;
    uint8_t version;
} __attribute__((packed));

struct os_info_cpu {
    uint8_t initialized;
    const char *processor_manufacturer;
    uint8_t voltage;
    uint16_t max_speed;
    uint8_t core_count;
    uint8_t thread_count;
    uint8_t part_number;
} __attribute__((packed));

struct os_info_ram {
    uint8_t initialized;
    uint8_t memory_type;
    uint16_t raw_size;
    uint32_t mb_size;
} __attribute__((packed));;

struct smbios_eps *find_smbios_eps(void);

void parse_smbios_tables(struct smbios_eps *eps);

void print_sys_info(void);

#endif
