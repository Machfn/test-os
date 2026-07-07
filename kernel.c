/**
 * @file kernel.c
 * @brief Main kernel
 * @author Ben Warkentin
 */

#include "kernel/tty.h"
#include "kernel/io.h"
#include "kernel/kbd.h"
#include "kernel/multiboot.h"
#include "libc/include/string.h"

static char* const VGA_MEMORY = (char *)0xb8000;

static const int VGA_WIDTH = 80;
static const int VGA_HEIGHT = 25;

static char mmap_init = 'n';

void kernel_early(uint32_t magic, uint32_t multiboot_info_physical_ptr)
{
    if (magic != MULTIBOOT1_MAGIC) {
        __asm__ __volatile__("hlt");
        return;
    }

    struct multiboot_info *mbi = (struct multiboot_info *)multiboot_info_physical_ptr;

    if (mbi->flags & (1 << 6)) {
        mmap_init = 'y';
    }

    terminal_initialize();

    struct smbios_eps *eps = find_smbios_eps();
    if (eps != 0) {
        parse_smbios_tables(eps);
    }

    
}

// __attribute__((noreturn))

int main(void)
{
    printf("Benon test OS");
    // printf("\nmmap_init : %c", mmap_init);
    printprompt();
    char buf[256] = {0};
    size_t buf_index = 0;
    while(1) {
        uint8_t byte;
        while (byte = scan()) {
            if (byte == 0x1c) {
                if (strlen(buf) > 0 && (strcmp(buf, "exit") == 0)) {
                    printf("\nGoodbye");
                } else if (strlen(buf) > 0 && (strcmp(buf, "sysinfo") == 0)) {
                    printf("\nSystem Information: \n");
                    print_sys_info();
                }
                printprompt();
                memset(buf, 0, sizeof(buf));
                buf_index = 0;
                break;
            } else {
                char c = normalmap[byte];
                char s[2];
                ctos(s, c);
                printf("%s", s);
                if (c != NO && buf_index < sizeof(buf) - 1) {
                    buf[buf_index++] = c;
                    buf[buf_index] = '\0';
                }
            }
            move_cursor(get_terminal_row(), get_terminal_col());
        }
    }
    return 0;
}

