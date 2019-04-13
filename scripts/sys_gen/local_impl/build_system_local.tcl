
################################################################
# This is a generated script based on design: design_1
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2018.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   catch {common::send_msg_id "BD_TCL-109" "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_1_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcvu9p-flga2104-2L-e
   set_property BOARD_PART xilinx.com:vcu118:part0:2.0 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name design_1

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_msg_id "BD_TCL-001" "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_msg_id "BD_TCL-002" "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_msg_id "BD_TCL-003" "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_msg_id "BD_TCL-004" "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

  # Add USER_COMMENTS on $design_name
  set_property USER_COMMENTS.comment_0 "input feature size:
227*227*64 Byte = 3.14MB
output feature size:
55*55*64*3 = 0.55 MB
weight size:
64*121*96=0.71MB" [get_bd_designs $design_name]

common::send_msg_id "BD_TCL-005" "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_msg_id "BD_TCL-114" "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:axi_bram_ctrl:4.0\
xilinx.com:ip:axi_register_slice:2.1\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:ddr4:2.2\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:ip:smartconnect:1.0\
xilinx.com:hls:sub_net_0:1.0\
xilinx.com:hls:sub_net_1:1.0\
xilinx.com:ip:util_ds_buf:2.1\
xilinx.com:ip:xdma:4.1\
xilinx.com:ip:xlconstant:1.1\
"

   set list_ips_missing ""
   common::send_msg_id "BD_TCL-006" "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_msg_id "BD_TCL-115" "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

if { $bCheckIPsPassed != 1 } {
  common::send_msg_id "BD_TCL-1003" "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_msg_id "BD_TCL-100" "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_msg_id "BD_TCL-101" "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set ddr4_sdram_c1 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddr4_rtl:1.0 ddr4_sdram_c1 ]
  set default_250mhz_clk1 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 default_250mhz_clk1 ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {250000000} \
   ] $default_250mhz_clk1
  set pci_express_x16 [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:pcie_7x_mgt_rtl:1.0 pci_express_x16 ]
  set pcie_refclk [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:diff_clock_rtl:1.0 pcie_refclk ]
  set_property -dict [ list \
   CONFIG.FREQ_HZ {100000000} \
   ] $pcie_refclk

  # Create ports
  set pcie_perstn [ create_bd_port -dir I -type rst pcie_perstn ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_LOW} \
 ] $pcie_perstn
  set reset [ create_bd_port -dir I -type rst reset ]
  set_property -dict [ list \
   CONFIG.POLARITY {ACTIVE_HIGH} \
 ] $reset

  # Create instance: axi_bram_ctrl_0, and set properties
  set axi_bram_ctrl_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 axi_bram_ctrl_0 ]
  set_property -dict [ list \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.ECC_TYPE {0} \
   CONFIG.SINGLE_PORT_BRAM {1} \
 ] $axi_bram_ctrl_0

  # Create instance: axi_bram_ctrl_1, and set properties
  set axi_bram_ctrl_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 axi_bram_ctrl_1 ]
  set_property -dict [ list \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.ECC_TYPE {0} \
   CONFIG.SINGLE_PORT_BRAM {1} \
 ] $axi_bram_ctrl_1

  # Create instance: axi_bram_ctrl_2, and set properties
  set axi_bram_ctrl_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_bram_ctrl:4.0 axi_bram_ctrl_2 ]
  set_property -dict [ list \
   CONFIG.DATA_WIDTH {512} \
   CONFIG.ECC_TYPE {0} \
   CONFIG.SINGLE_PORT_BRAM {1} \
 ] $axi_bram_ctrl_2

  # Create instance: axi_interconnect_0, and set properties
  set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {3} \
 ] $axi_interconnect_0

  # Create instance: axi_register_slice_0, and set properties
  set axi_register_slice_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_0 ]

  # Create instance: axi_register_slice_1, and set properties
  set axi_register_slice_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_1 ]

  # Create instance: axi_register_slice_2, and set properties
  set axi_register_slice_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_2 ]

  # Create instance: axi_register_slice_3, and set properties
  set axi_register_slice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_3 ]

  # Create instance: axi_register_slice_4, and set properties
  set axi_register_slice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_4 ]

  # Create instance: axi_register_slice_5, and set properties
  set axi_register_slice_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_5 ]

  # Create instance: axi_register_slice_6, and set properties
  set axi_register_slice_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_6 ]

  # Create instance: axi_register_slice_7, and set properties
  set axi_register_slice_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_7 ]

  # Create instance: axi_register_slice_8, and set properties
  set axi_register_slice_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_8 ]

  # Create instance: axi_register_slice_9, and set properties
  set axi_register_slice_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 axi_register_slice_9 ]

  # Create instance: blk_mem_gen_0, and set properties
  set blk_mem_gen_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_0 ]
  set_property -dict [ list \
   CONFIG.Assume_Synchronous_Clk {true} \
   CONFIG.Byte_Size {9} \
   CONFIG.EN_SAFETY_CKT {false} \
   CONFIG.Enable_32bit_Address {false} \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Operating_Mode_A {NO_CHANGE} \
   CONFIG.Operating_Mode_B {NO_CHANGE} \
   CONFIG.PRIM_type_to_Implement {URAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Read_Width_A {512} \
   CONFIG.Read_Width_B {512} \
   CONFIG.Register_PortA_Output_of_Memory_Core {true} \
   CONFIG.Register_PortA_Output_of_Memory_Primitives {true} \
   CONFIG.Register_PortB_Output_of_Memory_Core {true} \
   CONFIG.Register_PortB_Output_of_Memory_Primitives {true} \
   CONFIG.Use_Byte_Write_Enable {false} \
   CONFIG.Use_REGCEA_Pin {true} \
   CONFIG.Use_REGCEB_Pin {true} \
   CONFIG.Use_RSTA_Pin {false} \
   CONFIG.Write_Depth_A {32768} \
   CONFIG.Write_Width_A {512} \
   CONFIG.Write_Width_B {512} \
   CONFIG.use_bram_block {Stand_Alone} \
 ] $blk_mem_gen_0

  # Create instance: blk_mem_gen_1, and set properties
  set blk_mem_gen_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_1 ]
  set_property -dict [ list \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Use_RSTB_Pin {true} \
 ] $blk_mem_gen_1

  # Create instance: blk_mem_gen_2, and set properties
  set blk_mem_gen_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_2 ]
  set_property -dict [ list \
   CONFIG.Assume_Synchronous_Clk {true} \
   CONFIG.Byte_Size {9} \
   CONFIG.EN_SAFETY_CKT {false} \
   CONFIG.Enable_32bit_Address {false} \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Operating_Mode_A {NO_CHANGE} \
   CONFIG.Operating_Mode_B {NO_CHANGE} \
   CONFIG.PRIM_type_to_Implement {URAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Read_Width_A {512} \
   CONFIG.Read_Width_B {512} \
   CONFIG.Register_PortA_Output_of_Memory_Core {true} \
   CONFIG.Register_PortA_Output_of_Memory_Primitives {true} \
   CONFIG.Register_PortB_Output_of_Memory_Core {true} \
   CONFIG.Register_PortB_Output_of_Memory_Primitives {true} \
   CONFIG.Use_Byte_Write_Enable {false} \
   CONFIG.Use_REGCEA_Pin {true} \
   CONFIG.Use_REGCEB_Pin {true} \
   CONFIG.Use_RSTA_Pin {false} \
   CONFIG.Write_Depth_A {32768} \
   CONFIG.Write_Width_A {512} \
   CONFIG.Write_Width_B {512} \
   CONFIG.use_bram_block {Stand_Alone} \
 ] $blk_mem_gen_2

  # Create instance: blk_mem_gen_3, and set properties
  set blk_mem_gen_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_3 ]
  set_property -dict [ list \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Use_RSTB_Pin {true} \
 ] $blk_mem_gen_3

  # Create instance: blk_mem_gen_4, and set properties
  set blk_mem_gen_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_4 ]
  set_property -dict [ list \
   CONFIG.Assume_Synchronous_Clk {true} \
   CONFIG.Byte_Size {9} \
   CONFIG.EN_SAFETY_CKT {false} \
   CONFIG.Enable_32bit_Address {false} \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Operating_Mode_A {NO_CHANGE} \
   CONFIG.Operating_Mode_B {NO_CHANGE} \
   CONFIG.PRIM_type_to_Implement {URAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Read_Width_A {512} \
   CONFIG.Read_Width_B {512} \
   CONFIG.Register_PortA_Output_of_Memory_Core {true} \
   CONFIG.Register_PortA_Output_of_Memory_Primitives {true} \
   CONFIG.Register_PortB_Output_of_Memory_Core {true} \
   CONFIG.Register_PortB_Output_of_Memory_Primitives {true} \
   CONFIG.Use_Byte_Write_Enable {false} \
   CONFIG.Use_REGCEA_Pin {true} \
   CONFIG.Use_REGCEB_Pin {true} \
   CONFIG.Use_RSTA_Pin {false} \
   CONFIG.Write_Depth_A {32768} \
   CONFIG.Write_Width_A {512} \
   CONFIG.Write_Width_B {512} \
   CONFIG.use_bram_block {Stand_Alone} \
 ] $blk_mem_gen_4

  # Create instance: blk_mem_gen_5, and set properties
  set blk_mem_gen_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 blk_mem_gen_5 ]
  set_property -dict [ list \
   CONFIG.Enable_B {Use_ENB_Pin} \
   CONFIG.Memory_Type {True_Dual_Port_RAM} \
   CONFIG.Port_B_Clock {100} \
   CONFIG.Port_B_Enable_Rate {100} \
   CONFIG.Port_B_Write_Rate {50} \
   CONFIG.Use_RSTB_Pin {true} \
 ] $blk_mem_gen_5

  # Create instance: ddr4_0, and set properties
  set ddr4_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:ddr4:2.2 ddr4_0 ]
  set_property -dict [ list \
   CONFIG.ADDN_UI_CLKOUT1_FREQ_HZ {250} \
   CONFIG.C0.BANK_GROUP_WIDTH {1} \
   CONFIG.C0.DDR4_AxiAddressWidth {31} \
   CONFIG.C0.DDR4_AxiDataWidth {512} \
   CONFIG.C0.DDR4_CLKFBOUT_MULT {6} \
   CONFIG.C0.DDR4_CLKOUT0_DIVIDE {5} \
   CONFIG.C0.DDR4_CasWriteLatency {12} \
   CONFIG.C0.DDR4_DataWidth {64} \
   CONFIG.C0.DDR4_InputClockPeriod {4000} \
   CONFIG.C0.DDR4_MemoryPart {MT40A256M16GE-083E} \
   CONFIG.C0.DDR4_Specify_MandD {false} \
   CONFIG.C0.DDR4_TimePeriod {833} \
   CONFIG.C0_CLOCK_BOARD_INTERFACE {default_250mhz_clk1} \
   CONFIG.C0_DDR4_BOARD_INTERFACE {ddr4_sdram_c1} \
   CONFIG.RESET_BOARD_INTERFACE {reset} \
 ] $ddr4_0

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]

  # Create instance: proc_sys_reset_1, and set properties
  set proc_sys_reset_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_1 ]

  # Create instance: smartconnect_0, and set properties
  set smartconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_0 ]
  set_property -dict [ list \
   CONFIG.NUM_SI {3} \
 ] $smartconnect_0

  # Create instance: smartconnect_2, and set properties
  set smartconnect_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_2 ]
  set_property -dict [ list \
   CONFIG.NUM_SI {2} \
 ] $smartconnect_2

  # Create instance: smartconnect_3, and set properties
  set smartconnect_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_3 ]
  set_property -dict [ list \
   CONFIG.ADVANCED_PROPERTIES {  __view__ { } } \
   CONFIG.NUM_CLKS {3} \
   CONFIG.NUM_MI {2} \
   CONFIG.NUM_SI {4} \
 ] $smartconnect_3

  # Create instance: smartconnect_4, and set properties
  set smartconnect_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:1.0 smartconnect_4 ]
  set_property -dict [ list \
   CONFIG.NUM_SI {2} \
 ] $smartconnect_4

  # Create instance: sub_net_0_0, and set properties
  set sub_net_0_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_0:1.0 sub_net_0_0 ]

  set_property -dict [ list \
   CONFIG.SUPPORTS_NARROW_BURST {0} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.MAX_BURST_LENGTH {1} \
 ] [get_bd_intf_pins /sub_net_0_0/s_axi_CRTL_BUS]

  # Create instance: sub_net_0_1, and set properties
  set sub_net_0_1 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_1:1.0 sub_net_0_1 ]

  set_property -dict [ list \
   CONFIG.SUPPORTS_NARROW_BURST {0} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.MAX_BURST_LENGTH {1} \
 ] [get_bd_intf_pins /sub_net_0_1/s_axi_CRTL_BUS]

  # Create instance: sub_net_0_2, and set properties
  set sub_net_0_2 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_1:1.0 sub_net_0_2 ]

  set_property -dict [ list \
   CONFIG.SUPPORTS_NARROW_BURST {0} \
   CONFIG.NUM_READ_OUTSTANDING {1} \
   CONFIG.NUM_WRITE_OUTSTANDING {1} \
   CONFIG.MAX_BURST_LENGTH {1} \
 ] [get_bd_intf_pins /sub_net_0_2/s_axi_CRTL_BUS]

  # Create instance: util_ds_buf, and set properties
  set util_ds_buf [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_ds_buf:2.1 util_ds_buf ]
  set_property -dict [ list \
   CONFIG.C_BUF_TYPE {IBUFDSGTE} \
   CONFIG.DIFF_CLK_IN_BOARD_INTERFACE {pcie_refclk} \
   CONFIG.USE_BOARD_FLOW {true} \
 ] $util_ds_buf

  # Create instance: xdma_0, and set properties
  set xdma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xdma:4.1 xdma_0 ]
  set_property -dict [ list \
   CONFIG.PCIE_BOARD_INTERFACE {pci_express_x16} \
   CONFIG.PF0_DEVICE_ID_mqdma {903F} \
   CONFIG.PF2_DEVICE_ID_mqdma {903F} \
   CONFIG.PF3_DEVICE_ID_mqdma {903F} \
   CONFIG.SYS_RST_N_BOARD_INTERFACE {pcie_perstn} \
   CONFIG.axi_data_width {512_bit} \
   CONFIG.axilite_master_en {true} \
   CONFIG.axisten_freq {250} \
   CONFIG.cfg_mgmt_if {false} \
   CONFIG.coreclk_freq {500} \
   CONFIG.pcie_id_if {false} \
   CONFIG.pciebar2axibar_axil_master {0x0000000000000000} \
   CONFIG.pf0_device_id {903F} \
   CONFIG.pf0_msix_cap_pba_bir {BAR_1} \
   CONFIG.pf0_msix_cap_table_bir {BAR_1} \
   CONFIG.pf1_bar0_scale {Kilobytes} \
   CONFIG.pf1_bar0_size {128} \
   CONFIG.pf1_bar1_enabled {false} \
   CONFIG.pf1_bar2_64bit {false} \
   CONFIG.pf1_bar2_enabled {false} \
   CONFIG.pf1_bar4_64bit {false} \
   CONFIG.pf1_bar4_enabled {false} \
   CONFIG.pl_link_cap_max_link_speed {8.0_GT/s} \
   CONFIG.pl_link_cap_max_link_width {X16} \
   CONFIG.plltype {QPLL1} \
   CONFIG.xdma_axi_intf_mm {AXI_Memory_Mapped} \
   CONFIG.xdma_rnum_chnl {4} \
   CONFIG.xdma_wnum_chnl {4} \
 ] $xdma_0

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create instance: xlconstant_5, and set properties
  set xlconstant_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_5 ]

  # Create instance: xlconstant_6, and set properties
  set xlconstant_6 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_6 ]

  # Create instance: xlconstant_7, and set properties
  set xlconstant_7 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_7 ]

  # Create instance: xlconstant_8, and set properties
  set xlconstant_8 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_8 ]

  # Create instance: xlconstant_9, and set properties
  set xlconstant_9 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_9 ]

  # Create instance: xlconstant_10, and set properties
  set xlconstant_10 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_10 ]

  # Create interface connections
  connect_bd_intf_net -intf_net axi_bram_ctrl_0_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_1/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_1_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_1/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_3/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_2_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_2/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_5/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_interconnect_0/M00_AXI] [get_bd_intf_pins sub_net_0_0/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net axi_interconnect_0_M01_AXI [get_bd_intf_pins axi_interconnect_0/M01_AXI] [get_bd_intf_pins sub_net_0_1/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net axi_interconnect_0_M02_AXI [get_bd_intf_pins axi_interconnect_0/M02_AXI] [get_bd_intf_pins sub_net_0_2/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net axi_register_slice_0_M_AXI [get_bd_intf_pins axi_register_slice_0/M_AXI] [get_bd_intf_pins smartconnect_3/S01_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_1_M_AXI [get_bd_intf_pins axi_register_slice_1/M_AXI] [get_bd_intf_pins axi_register_slice_7/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_2_M_AXI [get_bd_intf_pins axi_bram_ctrl_0/S_AXI] [get_bd_intf_pins axi_register_slice_2/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_3_M_AXI [get_bd_intf_pins axi_register_slice_3/M_AXI] [get_bd_intf_pins smartconnect_3/S02_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_4_M_AXI [get_bd_intf_pins axi_bram_ctrl_1/S_AXI] [get_bd_intf_pins axi_register_slice_4/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_5_M_AXI [get_bd_intf_pins axi_bram_ctrl_2/S_AXI] [get_bd_intf_pins axi_register_slice_5/M_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_6_M_AXI [get_bd_intf_pins axi_register_slice_6/M_AXI] [get_bd_intf_pins smartconnect_3/S03_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_7_M_AXI [get_bd_intf_pins axi_register_slice_7/M_AXI] [get_bd_intf_pins axi_register_slice_8/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_8_M_AXI [get_bd_intf_pins axi_register_slice_8/M_AXI] [get_bd_intf_pins axi_register_slice_9/S_AXI]
  connect_bd_intf_net -intf_net axi_register_slice_9_M_AXI [get_bd_intf_pins axi_register_slice_9/M_AXI] [get_bd_intf_pins ddr4_0/C0_DDR4_S_AXI]
  connect_bd_intf_net -intf_net ddr4_0_C0_DDR4 [get_bd_intf_ports ddr4_sdram_c1] [get_bd_intf_pins ddr4_0/C0_DDR4]
  connect_bd_intf_net -intf_net default_250mhz_clk1_1 [get_bd_intf_ports default_250mhz_clk1] [get_bd_intf_pins ddr4_0/C0_SYS_CLK]
  connect_bd_intf_net -intf_net pcie_refclk_1 [get_bd_intf_ports pcie_refclk] [get_bd_intf_pins util_ds_buf/CLK_IN_D]
  connect_bd_intf_net -intf_net smartconnect_0_M00_AXI [get_bd_intf_pins axi_register_slice_0/S_AXI] [get_bd_intf_pins smartconnect_0/M00_AXI]
  connect_bd_intf_net -intf_net smartconnect_2_M00_AXI [get_bd_intf_pins axi_register_slice_3/S_AXI] [get_bd_intf_pins smartconnect_2/M00_AXI]
  connect_bd_intf_net -intf_net smartconnect_3_M00_AXI [get_bd_intf_pins axi_register_slice_1/S_AXI] [get_bd_intf_pins smartconnect_3/M00_AXI]
  connect_bd_intf_net -intf_net smartconnect_3_M01_AXI [get_bd_intf_pins axi_register_slice_5/S_AXI] [get_bd_intf_pins smartconnect_3/M01_AXI]
  connect_bd_intf_net -intf_net smartconnect_4_M00_AXI [get_bd_intf_pins axi_register_slice_6/S_AXI] [get_bd_intf_pins smartconnect_4/M00_AXI]
  connect_bd_intf_net -intf_net sub_net_0_0_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_1/BRAM_PORTA] [get_bd_intf_pins sub_net_0_0/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_DATA_IN [get_bd_intf_pins smartconnect_0/S02_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_PARAM_IN [get_bd_intf_pins smartconnect_0/S00_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_WEIGHT_IN [get_bd_intf_pins smartconnect_0/S01_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_WEIGHT_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_3/BRAM_PORTA] [get_bd_intf_pins sub_net_0_1/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_DATA_IN [get_bd_intf_pins axi_register_slice_2/S_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_PARAM_IN [get_bd_intf_pins smartconnect_2/S00_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_WEIGHT_IN [get_bd_intf_pins smartconnect_2/S01_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_WEIGHT_IN]
  connect_bd_intf_net -intf_net sub_net_1_1_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_5/BRAM_PORTA] [get_bd_intf_pins sub_net_0_2/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_1_1_m_axi_DATA_IN [get_bd_intf_pins axi_register_slice_4/S_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_1_1_m_axi_PARAM_IN [get_bd_intf_pins smartconnect_4/S00_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_1_1_m_axi_WEIGHT_IN [get_bd_intf_pins smartconnect_4/S01_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_WEIGHT_IN]
  connect_bd_intf_net -intf_net xdma_0_M_AXI [get_bd_intf_pins smartconnect_3/S00_AXI] [get_bd_intf_pins xdma_0/M_AXI]
  connect_bd_intf_net -intf_net xdma_0_M_AXI_LITE [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins xdma_0/M_AXI_LITE]
  connect_bd_intf_net -intf_net xdma_0_pcie_mgt [get_bd_intf_ports pci_express_x16] [get_bd_intf_pins xdma_0/pcie_mgt]

  # Create port connections
  connect_bd_net -net M02_ACLK_1 [get_bd_pins axi_register_slice_1/aclk] [get_bd_pins axi_register_slice_7/aclk] [get_bd_pins axi_register_slice_8/aclk] [get_bd_pins axi_register_slice_9/aclk] [get_bd_pins ddr4_0/c0_ddr4_ui_clk] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins smartconnect_3/aclk]
  connect_bd_net -net S00_ACLK_1 [get_bd_pins axi_interconnect_0/ACLK] [get_bd_pins axi_interconnect_0/S00_ACLK] [get_bd_pins smartconnect_3/aclk1] [get_bd_pins xdma_0/axi_aclk]
  connect_bd_net -net blk_mem_gen_0_douta [get_bd_pins blk_mem_gen_0/douta] [get_bd_pins sub_net_0_0/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_0_doutb [get_bd_pins blk_mem_gen_0/doutb] [get_bd_pins sub_net_0_0/data_in_1_V_q0]
  connect_bd_net -net blk_mem_gen_2_douta [get_bd_pins blk_mem_gen_2/douta] [get_bd_pins sub_net_0_1/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_2_doutb [get_bd_pins blk_mem_gen_2/doutb] [get_bd_pins sub_net_0_1/data_in_1_V_q0]
  connect_bd_net -net blk_mem_gen_4_douta [get_bd_pins blk_mem_gen_4/douta] [get_bd_pins sub_net_0_2/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_4_doutb [get_bd_pins blk_mem_gen_4/doutb] [get_bd_pins sub_net_0_2/data_in_1_V_q0]
  connect_bd_net -net ddr4_0_addn_ui_clkout1 [get_bd_pins axi_bram_ctrl_0/s_axi_aclk] [get_bd_pins axi_bram_ctrl_1/s_axi_aclk] [get_bd_pins axi_bram_ctrl_2/s_axi_aclk] [get_bd_pins axi_interconnect_0/M00_ACLK] [get_bd_pins axi_interconnect_0/M01_ACLK] [get_bd_pins axi_interconnect_0/M02_ACLK] [get_bd_pins axi_register_slice_0/aclk] [get_bd_pins axi_register_slice_2/aclk] [get_bd_pins axi_register_slice_3/aclk] [get_bd_pins axi_register_slice_4/aclk] [get_bd_pins axi_register_slice_5/aclk] [get_bd_pins axi_register_slice_6/aclk] [get_bd_pins blk_mem_gen_0/clka] [get_bd_pins blk_mem_gen_0/clkb] [get_bd_pins blk_mem_gen_2/clka] [get_bd_pins blk_mem_gen_2/clkb] [get_bd_pins blk_mem_gen_4/clka] [get_bd_pins blk_mem_gen_4/clkb] [get_bd_pins ddr4_0/addn_ui_clkout1] [get_bd_pins proc_sys_reset_1/slowest_sync_clk] [get_bd_pins smartconnect_0/aclk] [get_bd_pins smartconnect_2/aclk] [get_bd_pins smartconnect_3/aclk2] [get_bd_pins smartconnect_4/aclk] [get_bd_pins sub_net_0_0/ap_clk] [get_bd_pins sub_net_0_1/ap_clk] [get_bd_pins sub_net_0_2/ap_clk]
  connect_bd_net -net ddr4_0_c0_ddr4_ui_clk_sync_rst [get_bd_pins ddr4_0/c0_ddr4_ui_clk_sync_rst] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net pcie_perstn_1 [get_bd_ports pcie_perstn] [get_bd_pins xdma_0/sys_rst_n]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins axi_register_slice_1/aresetn] [get_bd_pins axi_register_slice_7/aresetn] [get_bd_pins axi_register_slice_8/aresetn] [get_bd_pins axi_register_slice_9/aresetn] [get_bd_pins ddr4_0/c0_ddr4_aresetn] [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins smartconnect_3/aresetn]
  connect_bd_net -net proc_sys_reset_1_peripheral_aresetn [get_bd_pins axi_bram_ctrl_0/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_1/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_2/s_axi_aresetn] [get_bd_pins axi_interconnect_0/M00_ARESETN] [get_bd_pins axi_interconnect_0/M01_ARESETN] [get_bd_pins axi_interconnect_0/M02_ARESETN] [get_bd_pins axi_register_slice_0/aresetn] [get_bd_pins axi_register_slice_2/aresetn] [get_bd_pins axi_register_slice_3/aresetn] [get_bd_pins axi_register_slice_4/aresetn] [get_bd_pins axi_register_slice_5/aresetn] [get_bd_pins axi_register_slice_6/aresetn] [get_bd_pins proc_sys_reset_1/peripheral_aresetn] [get_bd_pins smartconnect_0/aresetn] [get_bd_pins smartconnect_2/aresetn] [get_bd_pins smartconnect_4/aresetn] [get_bd_pins sub_net_0_0/ap_rst_n] [get_bd_pins sub_net_0_1/ap_rst_n] [get_bd_pins sub_net_0_2/ap_rst_n]
  connect_bd_net -net reset_1 [get_bd_ports reset] [get_bd_pins ddr4_0/sys_rst] [get_bd_pins proc_sys_reset_1/ext_reset_in]
  connect_bd_net -net sub_net_0_0_data_in_1_V_address0 [get_bd_pins blk_mem_gen_0/addrb] [get_bd_pins sub_net_0_0/data_in_1_V_address0]
  connect_bd_net -net sub_net_0_0_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_0/enb] [get_bd_pins sub_net_0_0/data_in_1_V_ce0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_address0 [get_bd_pins blk_mem_gen_0/addra] [get_bd_pins sub_net_0_0/data_out_0_V_address0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_0/ena] [get_bd_pins sub_net_0_0/data_out_0_V_ce0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_d0 [get_bd_pins blk_mem_gen_0/dina] [get_bd_pins sub_net_0_0/data_out_0_V_d0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_we0 [get_bd_pins blk_mem_gen_0/wea] [get_bd_pins sub_net_0_0/data_out_0_V_we0]
  connect_bd_net -net sub_net_1_0_data_in_1_V_address0 [get_bd_pins blk_mem_gen_2/addrb] [get_bd_pins sub_net_0_1/data_in_1_V_address0]
  connect_bd_net -net sub_net_1_0_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_2/enb] [get_bd_pins sub_net_0_1/data_in_1_V_ce0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_address0 [get_bd_pins blk_mem_gen_2/addra] [get_bd_pins sub_net_0_1/data_out_0_V_address0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_2/ena] [get_bd_pins sub_net_0_1/data_out_0_V_ce0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_d0 [get_bd_pins blk_mem_gen_2/dina] [get_bd_pins sub_net_0_1/data_out_0_V_d0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_we0 [get_bd_pins blk_mem_gen_2/wea] [get_bd_pins sub_net_0_1/data_out_0_V_we0]
  connect_bd_net -net sub_net_1_1_data_in_1_V_address0 [get_bd_pins blk_mem_gen_4/addrb] [get_bd_pins sub_net_0_2/data_in_1_V_address0]
  connect_bd_net -net sub_net_1_1_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_4/enb] [get_bd_pins sub_net_0_2/data_in_1_V_ce0]
  connect_bd_net -net sub_net_1_1_data_out_0_V_address0 [get_bd_pins blk_mem_gen_4/addra] [get_bd_pins sub_net_0_2/data_out_0_V_address0]
  connect_bd_net -net sub_net_1_1_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_4/ena] [get_bd_pins sub_net_0_2/data_out_0_V_ce0]
  connect_bd_net -net sub_net_1_1_data_out_0_V_d0 [get_bd_pins blk_mem_gen_4/dina] [get_bd_pins sub_net_0_2/data_out_0_V_d0]
  connect_bd_net -net sub_net_1_1_data_out_0_V_we0 [get_bd_pins blk_mem_gen_4/wea] [get_bd_pins sub_net_0_2/data_out_0_V_we0]
  connect_bd_net -net util_ds_buf_IBUF_DS_ODIV2 [get_bd_pins util_ds_buf/IBUF_DS_ODIV2] [get_bd_pins xdma_0/sys_clk]
  connect_bd_net -net util_ds_buf_IBUF_OUT [get_bd_pins util_ds_buf/IBUF_OUT] [get_bd_pins xdma_0/sys_clk_gt]
  connect_bd_net -net xdma_0_axi_aresetn [get_bd_pins axi_interconnect_0/ARESETN] [get_bd_pins axi_interconnect_0/S00_ARESETN] [get_bd_pins xdma_0/axi_aresetn]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins xdma_0/usr_irq_req] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_10_dout [get_bd_pins blk_mem_gen_4/regceb] [get_bd_pins xlconstant_10/dout]
  connect_bd_net -net xlconstant_5_dout [get_bd_pins blk_mem_gen_0/regcea] [get_bd_pins xlconstant_5/dout]
  connect_bd_net -net xlconstant_6_dout [get_bd_pins blk_mem_gen_0/regceb] [get_bd_pins xlconstant_6/dout]
  connect_bd_net -net xlconstant_7_dout [get_bd_pins blk_mem_gen_2/regcea] [get_bd_pins xlconstant_7/dout]
  connect_bd_net -net xlconstant_8_dout [get_bd_pins blk_mem_gen_2/regceb] [get_bd_pins xlconstant_8/dout]
  connect_bd_net -net xlconstant_9_dout [get_bd_pins blk_mem_gen_4/regcea] [get_bd_pins xlconstant_9/dout]

  # Create address segments
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_PARAM_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_DATA_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x00040000 -offset 0xCA000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_DATA_IN] [get_bd_addr_segs axi_bram_ctrl_0/S_AXI/Mem0] SEG_axi_bram_ctrl_0_Mem0
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_PARAM_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x00040000 -offset 0xCB000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_DATA_IN] [get_bd_addr_segs axi_bram_ctrl_1/S_AXI/Mem0] SEG_axi_bram_ctrl_1_Mem0
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_PARAM_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  create_bd_addr_seg -range 0x10000000 -offset 0x10000000 [get_bd_addr_spaces xdma_0/M_AXI] [get_bd_addr_segs ddr4_0/C0_DDR4_MEMORY_MAP/C0_DDR4_ADDRESS_BLOCK] SEG_ddr4_0_C0_DDR4_ADDRESS_BLOCK
  create_bd_addr_seg -range 0x00001000 -offset 0x00000000 [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs sub_net_0_0/s_axi_CRTL_BUS/Reg] SEG_sub_net_0_0_Reg
  create_bd_addr_seg -range 0x00001000 -offset 0x00010000 [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs sub_net_0_1/s_axi_CRTL_BUS/Reg] SEG_sub_net_1_0_Reg
  create_bd_addr_seg -range 0x00001000 -offset 0x00020000 [get_bd_addr_spaces xdma_0/M_AXI_LITE] [get_bd_addr_segs sub_net_0_2/s_axi_CRTL_BUS/Reg] SEG_sub_net_1_1_Reg

  # Exclude Address Segments
  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_DATA_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_0/Data_m_axi_DATA_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_PARAM_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_0/Data_m_axi_PARAM_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_0/Data_m_axi_WEIGHT_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_PARAM_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_1/Data_m_axi_PARAM_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_1/Data_m_axi_WEIGHT_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_PARAM_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_2/Data_m_axi_PARAM_IN/SEG_axi_bram_ctrl_2_Mem0]

  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  exclude_bd_addr_seg [get_bd_addr_segs sub_net_0_2/Data_m_axi_WEIGHT_IN/SEG_axi_bram_ctrl_2_Mem0]



  # Restore current instance
  current_bd_instance $oldCurInst

  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


