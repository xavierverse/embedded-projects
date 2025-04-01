Everything works

## Running
Before running, export esp-idf tools.

To run gdb, change directory to lab2_1, flash the binary in lab2_1 with "openocd -f board/esp32c3-builtin.cfg -c "program_esp lab2_debug.bin 0x10000 verify exit" then run "idf.py openocd" in the same terminal. Open a new terminal and run "riscv32-esp-elf-gdb -x gdbinit lab2_debug.elf" to run gdb and connect to openocd on port :3333.

To display the current temperature and humidity every 2 seconds, change directory to lab2_2 and run "idf.py flash" then "idf.py monitor".
