gdb_com_port_default := "COM3"

recipe-name:
    echo 'This is a recipe!'

# this is a comment
another-recipe:
    @echo 'This is another recipe.'


compile:
    mbed compile

flash gdb_com_port=gdb_com_port_default: 
    arm-none-eabi-gdb -nx --batch \
        -ex 'target extended-remote \\\\.\\{{gdb_com_port}}' \
        -ex 'mon connect_srst enable' \
        -ex 'monitor swdp_scan' \
        -ex 'attach 1' \
        -ex 'load' \
        -ex 'compare-sections' \
        -ex 'kill' \
        ./BUILD/NUCLEO_L432KC/GCC_ARM/spha-rel12-firmware.elf

test:
    just compile
    just flash
    @echo "Done. Waiting for changes..."

watch:
    # watchexec -w './BUILD/NUCLEO_L432KC/GCC_ARM/spha-rel12-firmware.elf' -s SIGKILL just flash
    watchexec -w src just test


stflash:
    stflash --reset write ./BUILD/NUCLEO_L432KC/GCC_ARM/spha-rel12-firmware.elf 0x08000000
