
################################################################
# This is a generated script based on design: cl
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
set scripts_vivado_version 2018.2
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
# source cl_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xcvu9p-flgb2104-2-i
   set_property BOARD_PART xilinx.com:f1_cl:part0:1.0 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name cl

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
xilinx.com:ip:aws:1.0\
xilinx.com:ip:axi_bram_ctrl:4.0\
xilinx.com:ip:blk_mem_gen:8.4\
xilinx.com:ip:proc_sys_reset:5.0\
xilinx.com:hls:sub_net_0:1.0\
xilinx.com:hls:sub_net_1:1.0\
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
  set S_SH [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:aws_f1_sh1_rtl:1.0 S_SH ]

  # Create ports

  # Create instance: aws_0, and set properties
  set aws_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:aws:1.0 aws_0 ]
  set_property -dict [ list \
   CONFIG.AUX_PRESENT {1} \
   CONFIG.BAR1_PRESENT {0} \
   CONFIG.CLOCK_A0_FREQ {125000000} \
   CONFIG.CLOCK_A1_FREQ {62500000} \
   CONFIG.CLOCK_A2_FREQ {187500000} \
   CONFIG.CLOCK_A3_FREQ {250000000} \
   CONFIG.CLOCK_A_RECIPE {0} \
   CONFIG.DDR_A_PRESENT {0} \
   CONFIG.DDR_B_PRESENT {0} \
   CONFIG.DDR_C_PRESENT {1} \
   CONFIG.DDR_D_PRESENT {0} \
   CONFIG.DEVICE_ID {0xF000} \
   CONFIG.NUM_A_CLOCKS {4} \
   CONFIG.NUM_STAGES_STATS {1} \
   CONFIG.OCL_PRESENT {1} \
   CONFIG.PCIS_PRESENT {1} \
   CONFIG.SDA_PRESENT {0} \
 ] $aws_0

  # Create instance: aws_0_axi_periph_1, and set properties
  set aws_0_axi_periph_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 aws_0_axi_periph_1 ]
  set_property -dict [ list \
   CONFIG.NUM_MI {3} \
   CONFIG.S00_HAS_REGSLICE {4} \
 ] $aws_0_axi_periph_1

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

  # Create instance: axi_mem_intercon, and set properties
  set axi_mem_intercon [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_mem_intercon ]
  set_property -dict [ list \
   CONFIG.M00_HAS_DATA_FIFO {0} \
   CONFIG.M00_HAS_REGSLICE {4} \
   CONFIG.M01_HAS_DATA_FIFO {0} \
   CONFIG.M01_HAS_REGSLICE {4} \
   CONFIG.M02_HAS_DATA_FIFO {0} \
   CONFIG.M02_HAS_REGSLICE {4} \
   CONFIG.M03_HAS_DATA_FIFO {0} \
   CONFIG.M03_HAS_REGSLICE {4} \
   CONFIG.M04_HAS_DATA_FIFO {0} \
   CONFIG.M04_HAS_REGSLICE {4} \
   CONFIG.NUM_MI {5} \
   CONFIG.NUM_SI {8} \
   CONFIG.S00_HAS_DATA_FIFO {0} \
   CONFIG.S00_HAS_REGSLICE {4} \
   CONFIG.S01_HAS_DATA_FIFO {0} \
   CONFIG.S01_HAS_REGSLICE {4} \
   CONFIG.S02_HAS_DATA_FIFO {0} \
   CONFIG.S02_HAS_REGSLICE {4} \
   CONFIG.S03_HAS_DATA_FIFO {0} \
   CONFIG.S03_HAS_REGSLICE {4} \
   CONFIG.S04_HAS_DATA_FIFO {0} \
   CONFIG.S04_HAS_REGSLICE {4} \
   CONFIG.S05_HAS_DATA_FIFO {0} \
   CONFIG.S05_HAS_REGSLICE {4} \
   CONFIG.S06_HAS_DATA_FIFO {0} \
   CONFIG.S06_HAS_REGSLICE {4} \
   CONFIG.S07_HAS_DATA_FIFO {0} \
   CONFIG.S07_HAS_REGSLICE {4} \
 ] $axi_mem_intercon

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
   CONFIG.Pipeline_Stages {3} \
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
   CONFIG.Pipeline_Stages {3} \
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
   CONFIG.Pipeline_Stages {3} \
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

  # Create instance: proc_sys_reset_0, and set properties
  set proc_sys_reset_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 proc_sys_reset_0 ]

  # Create instance: sub_net_0_0, and set properties
  set sub_net_0_0 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_0:1.0 sub_net_0_0 ]

  # Create instance: sub_net_0_1, and set properties
  set sub_net_0_1 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_1:1.0 sub_net_0_1 ]

  # Create instance: sub_net_0_2, and set properties
  set sub_net_0_2 [ create_bd_cell -type ip -vlnv xilinx.com:hls:sub_net_1:1.0 sub_net_0_2 ]

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]

  # Create instance: xlconstant_1, and set properties
  set xlconstant_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_1 ]

  # Create instance: xlconstant_2, and set properties
  set xlconstant_2 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_2 ]

  # Create instance: xlconstant_3, and set properties
  set xlconstant_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_3 ]

  # Create instance: xlconstant_4, and set properties
  set xlconstant_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_4 ]

  # Create instance: xlconstant_5, and set properties
  set xlconstant_5 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_5 ]

  # Create interface connections
  connect_bd_intf_net -intf_net S_SH_1 [get_bd_intf_ports S_SH] [get_bd_intf_pins aws_0/S_SH]
  connect_bd_intf_net -intf_net aws_0_M_AXI_OCL [get_bd_intf_pins aws_0/M_AXI_OCL] [get_bd_intf_pins aws_0_axi_periph_1/S00_AXI]
  connect_bd_intf_net -intf_net aws_0_M_AXI_PCIS [get_bd_intf_pins aws_0/M_AXI_PCIS] [get_bd_intf_pins axi_mem_intercon/S00_AXI]
  connect_bd_intf_net -intf_net aws_0_axi_periph_1_M00_AXI [get_bd_intf_pins aws_0_axi_periph_1/M00_AXI] [get_bd_intf_pins sub_net_0_0/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net aws_0_axi_periph_1_M01_AXI [get_bd_intf_pins aws_0_axi_periph_1/M01_AXI] [get_bd_intf_pins sub_net_0_1/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net aws_0_axi_periph_1_M02_AXI [get_bd_intf_pins aws_0_axi_periph_1/M02_AXI] [get_bd_intf_pins sub_net_0_2/s_axi_CRTL_BUS]
  connect_bd_intf_net -intf_net axi_bram_ctrl_0_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_0/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_1/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_1_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_1/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_3/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_bram_ctrl_2_BRAM_PORTA [get_bd_intf_pins axi_bram_ctrl_2/BRAM_PORTA] [get_bd_intf_pins blk_mem_gen_5/BRAM_PORTB]
  connect_bd_intf_net -intf_net axi_mem_intercon_M02_AXI [get_bd_intf_pins aws_0/S_AXI_DDRC] [get_bd_intf_pins axi_mem_intercon/M02_AXI]
  connect_bd_intf_net -intf_net axi_mem_intercon_M04_AXI [get_bd_intf_pins axi_bram_ctrl_2/S_AXI] [get_bd_intf_pins axi_mem_intercon/M04_AXI]
  connect_bd_intf_net -intf_net sub_net_0_0_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_1/BRAM_PORTA] [get_bd_intf_pins sub_net_0_0/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_DATA_IN [get_bd_intf_pins axi_mem_intercon/S03_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_PARAM_IN [get_bd_intf_pins axi_mem_intercon/S01_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_0_0_m_axi_WEIGHT_IN [get_bd_intf_pins axi_mem_intercon/S02_AXI] [get_bd_intf_pins sub_net_0_0/m_axi_WEIGHT_IN]
  connect_bd_intf_net -intf_net sub_net_0_1_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_3/BRAM_PORTA] [get_bd_intf_pins sub_net_0_1/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_0_1_m_axi_DATA_IN [get_bd_intf_pins axi_bram_ctrl_0/S_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_0_1_m_axi_PARAM_IN [get_bd_intf_pins axi_mem_intercon/S04_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_0_1_m_axi_WEIGHT_IN [get_bd_intf_pins axi_mem_intercon/S05_AXI] [get_bd_intf_pins sub_net_0_1/m_axi_WEIGHT_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_data_out_1_V_PORTA [get_bd_intf_pins blk_mem_gen_5/BRAM_PORTA] [get_bd_intf_pins sub_net_0_2/data_out_1_V_PORTA]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_DATA_IN [get_bd_intf_pins axi_bram_ctrl_1/S_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_DATA_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_PARAM_IN [get_bd_intf_pins axi_mem_intercon/S06_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_PARAM_IN]
  connect_bd_intf_net -intf_net sub_net_1_0_m_axi_WEIGHT_IN [get_bd_intf_pins axi_mem_intercon/S07_AXI] [get_bd_intf_pins sub_net_0_2/m_axi_WEIGHT_IN]

  # Create port connections
  connect_bd_net -net S00_ACLK_1 [get_bd_pins aws_0/clk_main_a0_out] [get_bd_pins aws_0_axi_periph_1/S00_ACLK] [get_bd_pins axi_mem_intercon/M00_ACLK] [get_bd_pins axi_mem_intercon/M01_ACLK] [get_bd_pins axi_mem_intercon/M02_ACLK] [get_bd_pins axi_mem_intercon/M03_ACLK] [get_bd_pins axi_mem_intercon/S00_ACLK]
  connect_bd_net -net aws_0_clk_extra_a3_out [get_bd_pins aws_0/clk_extra_a2_out] [get_bd_pins aws_0_axi_periph_1/ACLK] [get_bd_pins aws_0_axi_periph_1/M00_ACLK] [get_bd_pins aws_0_axi_periph_1/M01_ACLK] [get_bd_pins aws_0_axi_periph_1/M02_ACLK] [get_bd_pins axi_bram_ctrl_0/s_axi_aclk] [get_bd_pins axi_bram_ctrl_1/s_axi_aclk] [get_bd_pins axi_bram_ctrl_2/s_axi_aclk] [get_bd_pins axi_mem_intercon/ACLK] [get_bd_pins axi_mem_intercon/M04_ACLK] [get_bd_pins axi_mem_intercon/S01_ACLK] [get_bd_pins axi_mem_intercon/S02_ACLK] [get_bd_pins axi_mem_intercon/S03_ACLK] [get_bd_pins axi_mem_intercon/S04_ACLK] [get_bd_pins axi_mem_intercon/S05_ACLK] [get_bd_pins axi_mem_intercon/S06_ACLK] [get_bd_pins axi_mem_intercon/S07_ACLK] [get_bd_pins blk_mem_gen_0/clka] [get_bd_pins blk_mem_gen_0/clkb] [get_bd_pins blk_mem_gen_2/clka] [get_bd_pins blk_mem_gen_2/clkb] [get_bd_pins blk_mem_gen_4/clka] [get_bd_pins blk_mem_gen_4/clkb] [get_bd_pins proc_sys_reset_0/slowest_sync_clk] [get_bd_pins sub_net_0_0/ap_clk] [get_bd_pins sub_net_0_1/ap_clk] [get_bd_pins sub_net_0_2/ap_clk]
  connect_bd_net -net aws_0_rst_main_n_out [get_bd_pins aws_0/rst_main_n_out] [get_bd_pins aws_0_axi_periph_1/S00_ARESETN] [get_bd_pins axi_mem_intercon/M00_ARESETN] [get_bd_pins axi_mem_intercon/M01_ARESETN] [get_bd_pins axi_mem_intercon/M02_ARESETN] [get_bd_pins axi_mem_intercon/M03_ARESETN] [get_bd_pins axi_mem_intercon/S00_ARESETN] [get_bd_pins proc_sys_reset_0/ext_reset_in]
  connect_bd_net -net blk_mem_gen_0_douta [get_bd_pins blk_mem_gen_0/douta] [get_bd_pins sub_net_0_0/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_0_doutb [get_bd_pins blk_mem_gen_0/doutb] [get_bd_pins sub_net_0_0/data_in_1_V_q0]
  connect_bd_net -net blk_mem_gen_2_douta [get_bd_pins blk_mem_gen_2/douta] [get_bd_pins sub_net_0_1/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_2_doutb [get_bd_pins blk_mem_gen_2/doutb] [get_bd_pins sub_net_0_1/data_in_1_V_q0]
  connect_bd_net -net blk_mem_gen_4_douta [get_bd_pins blk_mem_gen_4/douta] [get_bd_pins sub_net_0_2/data_out_0_V_q0]
  connect_bd_net -net blk_mem_gen_4_doutb [get_bd_pins blk_mem_gen_4/doutb] [get_bd_pins sub_net_0_2/data_in_1_V_q0]
  connect_bd_net -net proc_sys_reset_0_interconnect_aresetn [get_bd_pins aws_0_axi_periph_1/ARESETN] [get_bd_pins axi_mem_intercon/ARESETN] [get_bd_pins proc_sys_reset_0/interconnect_aresetn]
  connect_bd_net -net proc_sys_reset_0_peripheral_aresetn [get_bd_pins aws_0_axi_periph_1/M00_ARESETN] [get_bd_pins aws_0_axi_periph_1/M01_ARESETN] [get_bd_pins aws_0_axi_periph_1/M02_ARESETN] [get_bd_pins axi_bram_ctrl_0/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_1/s_axi_aresetn] [get_bd_pins axi_bram_ctrl_2/s_axi_aresetn] [get_bd_pins axi_mem_intercon/M04_ARESETN] [get_bd_pins axi_mem_intercon/S01_ARESETN] [get_bd_pins axi_mem_intercon/S02_ARESETN] [get_bd_pins axi_mem_intercon/S03_ARESETN] [get_bd_pins axi_mem_intercon/S04_ARESETN] [get_bd_pins axi_mem_intercon/S05_ARESETN] [get_bd_pins axi_mem_intercon/S06_ARESETN] [get_bd_pins axi_mem_intercon/S07_ARESETN] [get_bd_pins proc_sys_reset_0/peripheral_aresetn] [get_bd_pins sub_net_0_0/ap_rst_n] [get_bd_pins sub_net_0_1/ap_rst_n] [get_bd_pins sub_net_0_2/ap_rst_n]
  connect_bd_net -net sub_net_0_0_data_in_1_V_address0 [get_bd_pins blk_mem_gen_0/addrb] [get_bd_pins sub_net_0_0/data_in_1_V_address0]
  connect_bd_net -net sub_net_0_0_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_0/enb] [get_bd_pins sub_net_0_0/data_in_1_V_ce0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_address0 [get_bd_pins blk_mem_gen_0/addra] [get_bd_pins sub_net_0_0/data_out_0_V_address0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_0/ena] [get_bd_pins sub_net_0_0/data_out_0_V_ce0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_d0 [get_bd_pins blk_mem_gen_0/dina] [get_bd_pins sub_net_0_0/data_out_0_V_d0]
  connect_bd_net -net sub_net_0_0_data_out_0_V_we0 [get_bd_pins blk_mem_gen_0/wea] [get_bd_pins sub_net_0_0/data_out_0_V_we0]
  connect_bd_net -net sub_net_0_1_data_in_1_V_address0 [get_bd_pins blk_mem_gen_2/addrb] [get_bd_pins sub_net_0_1/data_in_1_V_address0]
  connect_bd_net -net sub_net_0_1_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_2/enb] [get_bd_pins sub_net_0_1/data_in_1_V_ce0]
  connect_bd_net -net sub_net_0_1_data_out_0_V_address0 [get_bd_pins blk_mem_gen_2/addra] [get_bd_pins sub_net_0_1/data_out_0_V_address0]
  connect_bd_net -net sub_net_0_1_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_2/ena] [get_bd_pins sub_net_0_1/data_out_0_V_ce0]
  connect_bd_net -net sub_net_0_1_data_out_0_V_d0 [get_bd_pins blk_mem_gen_2/dina] [get_bd_pins sub_net_0_1/data_out_0_V_d0]
  connect_bd_net -net sub_net_0_1_data_out_0_V_we0 [get_bd_pins blk_mem_gen_2/wea] [get_bd_pins sub_net_0_1/data_out_0_V_we0]
  connect_bd_net -net sub_net_1_0_data_in_1_V_address0 [get_bd_pins blk_mem_gen_4/addrb] [get_bd_pins sub_net_0_2/data_in_1_V_address0]
  connect_bd_net -net sub_net_1_0_data_in_1_V_ce0 [get_bd_pins blk_mem_gen_4/enb] [get_bd_pins sub_net_0_2/data_in_1_V_ce0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_address0 [get_bd_pins blk_mem_gen_4/addra] [get_bd_pins sub_net_0_2/data_out_0_V_address0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_ce0 [get_bd_pins blk_mem_gen_4/ena] [get_bd_pins sub_net_0_2/data_out_0_V_ce0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_d0 [get_bd_pins blk_mem_gen_4/dina] [get_bd_pins sub_net_0_2/data_out_0_V_d0]
  connect_bd_net -net sub_net_1_0_data_out_0_V_we0 [get_bd_pins blk_mem_gen_4/wea] [get_bd_pins sub_net_0_2/data_out_0_V_we0]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins blk_mem_gen_0/regcea] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlconstant_1_dout [get_bd_pins blk_mem_gen_0/regceb] [get_bd_pins xlconstant_1/dout]
  connect_bd_net -net xlconstant_2_dout [get_bd_pins blk_mem_gen_2/regcea] [get_bd_pins xlconstant_2/dout]
  connect_bd_net -net xlconstant_3_dout [get_bd_pins blk_mem_gen_2/regceb] [get_bd_pins xlconstant_3/dout]
  connect_bd_net -net xlconstant_4_dout [get_bd_pins blk_mem_gen_4/regcea] [get_bd_pins xlconstant_4/dout]
  connect_bd_net -net xlconstant_5_dout [get_bd_pins blk_mem_gen_4/regceb] [get_bd_pins xlconstant_5/dout]

  # Create address segments
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces aws_0/M_AXI_PCIS] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x00040000 -offset 0xC0000000 [get_bd_addr_spaces aws_0/M_AXI_PCIS] [get_bd_addr_segs axi_bram_ctrl_2/S_AXI/Mem0] SEG_axi_bram_ctrl_2_Mem0
  create_bd_addr_seg -range 0x00001000 -offset 0x00000000 [get_bd_addr_spaces aws_0/M_AXI_OCL] [get_bd_addr_segs sub_net_0_0/s_axi_CRTL_BUS/Reg] SEG_sub_net_0_0_Reg
  create_bd_addr_seg -range 0x00001000 -offset 0x00001000 [get_bd_addr_spaces aws_0/M_AXI_OCL] [get_bd_addr_segs sub_net_0_1/s_axi_CRTL_BUS/Reg] SEG_sub_net_0_1_Reg
  create_bd_addr_seg -range 0x00001000 -offset 0x00002000 [get_bd_addr_spaces aws_0/M_AXI_OCL] [get_bd_addr_segs sub_net_0_2/s_axi_CRTL_BUS/Reg] SEG_sub_net_1_0_Reg
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_PARAM_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_0/Data_m_axi_DATA_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_PARAM_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x00040000 -offset 0xCA000000 [get_bd_addr_spaces sub_net_0_1/Data_m_axi_DATA_IN] [get_bd_addr_segs axi_bram_ctrl_0/S_AXI/Mem0] SEG_axi_bram_ctrl_0_Mem0
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_PARAM_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x10000000 -offset 0xD0000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_WEIGHT_IN] [get_bd_addr_segs aws_0/S_AXI_DDRC/Mem_DDRC] SEG_aws_0_Mem_DDRC
  create_bd_addr_seg -range 0x00040000 -offset 0xCB000000 [get_bd_addr_spaces sub_net_0_2/Data_m_axi_DATA_IN] [get_bd_addr_segs axi_bram_ctrl_1/S_AXI/Mem0] SEG_axi_bram_ctrl_1_Mem0

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


