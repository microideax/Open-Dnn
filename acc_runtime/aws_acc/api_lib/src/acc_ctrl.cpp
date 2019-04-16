#include "acc_ctrl.h"
using namespace std;
acc_ctrl::acc_ctrl( uint32_t para_offset_addr,
		    uint32_t weight_offset_addr,
                    uint32_t data_in_offset_addr,
		    off_t    ctrl_addr
		 			)
{
	this->para_offset_addr = para_offset_addr;
	this->weight_offset_addr = weight_offset_addr;
	this->data_in_offset_addr = data_in_offset_addr;
	this->ctrl_addr = ctrl_addr;
}

void acc_ctrl::write_weight(	short int weight[][32],
				int weight_length)
{
	cout <<"write weight start" << endl;
	int write_fd;
        int rc;
        int slot_id = 0;
        write_fd = fpga_dma_open_queue( FPGA_DMA_XDMA, slot_id,/*channel*/ 0, /*is_read*/ false);
        rc = fpga_dma_burst_write(write_fd, (uint8_t*)weight,weight_length,weight_offset_addr);
        if(write_fd >= 0)
            close(write_fd);
	cout <<"write weight finish" << endl;
}

void acc_ctrl::write_para(	int* para_list,
				int para_length)
{
	cout <<"write para start" << endl;
	int write_fd;
        int rc;
        int slot_id = 0;
        write_fd = fpga_dma_open_queue( FPGA_DMA_XDMA, slot_id,/*channel*/ 0, /*is_read*/ false);
        rc = fpga_dma_burst_write(write_fd, (uint8_t*)para_list,para_length,para_offset_addr);
        if(write_fd >= 0)
            close(write_fd);
	cout <<"write para finish" << endl;
}

void acc_ctrl::write_data(	short int feature[][32],
				int feature_length)
{
	cout <<"write data start"<<endl;

	int write_fd;
        int rc;
        int slot_id = 0;
        write_fd = fpga_dma_open_queue( FPGA_DMA_XDMA, slot_id,/*channel*/ 0, /*is_read*/ false);
        rc = fpga_dma_burst_write(write_fd, (uint8_t*)feature,feature_length,data_in_offset_addr);
        if(write_fd >= 0)
            close(write_fd);
	cout <<"write data finish"<<endl; 
}

void acc_ctrl::start_process(int mode)
{
	cout <<"process start"<<endl;
        int rc;
        uint32_t value;
        pci_bar_handle_t pci_bar_handle = PCI_BAR_HANDLE_INIT;
        rc = fpga_pci_attach(0,FPGA_APP_PF, 0, 0,  &pci_bar_handle);

        rc = fpga_pci_poke(pci_bar_handle, ctrl_addr+0x18, weight_offset_addr);   //weight
        rc = fpga_pci_poke(pci_bar_handle, ctrl_addr+0x10, para_offset_addr);   //para
        rc = fpga_pci_poke(pci_bar_handle, ctrl_addr+0x20, data_in_offset_addr);   //feature in

        rc = fpga_pci_poke(pci_bar_handle, ctrl_addr+0x28, mode);

        // cout << ctrl_addr<<endl;
        rc = fpga_pci_peek(pci_bar_handle, ctrl_addr, &value);
        cout << "value:"<<value<<endl;
        rc = fpga_pci_poke(pci_bar_handle, ctrl_addr, 0x00000001);
        while((value&0x00000002) != 0x00000002)
        {
           rc = fpga_pci_peek(pci_bar_handle, ctrl_addr, &value);
        }

        if (pci_bar_handle >= 0)
        {
            rc = fpga_pci_detach(pci_bar_handle);
            if (rc)
            {
                cout <<"Failure while detaching from the fpga."<<endl;
            }
        }
	cout <<"process finish"<<endl;
}

void acc_ctrl::read_data(short int feature[][32],int feature_length,int memId)
{
	cout << "read start"  << endl;
        int read_fd;
        int rc;
        int slot_id = 0;

        read_fd = fpga_dma_open_queue(FPGA_DMA_XDMA, slot_id, /*channel*/ 0, /*is_read*/ true);
        rc = fpga_dma_burst_read(read_fd, (uint8_t*)feature,feature_length, 0xC0000000);
        if (read_fd >= 0)
        {
            close(read_fd);
        }
	cout << "read finish" << endl;
}

