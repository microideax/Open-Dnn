<span style="display: inline-block">

# Table of Contents

1. Bitstream of accelerators
2. API functions to call the accelerators
3. Several demos

# Environmental Settings
1. UltraScale+ VU118 board in PCIe mode (xdma driver is required)
2. Pre-installed caffe framework
3. Pre-installed Opencv 
4. Pre-installed Vivado design suits

# How to use
Generate and download the bitstream to the platform
1. cd into a demo in the bitstream/ folder
2. start vivado in tcl mode with 'vivado -mode tcl' command
3. modify the demo name in the make_spi_mcs.tcl file
4. source the modified make_spi_mcs.tcl in vivado
5. source the program_spi.tcl after the .mcs files are generated

Compile and execute the host program 
1. cd into a demo in the demos/ folder
2. make
3. execute the demo with the command provided in the folder
