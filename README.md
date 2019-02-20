# Cloud-Dnn

## Introduction

Cloud-Dnn is an open-source framework that maps DNN models trained by Caffe to FPGAs in the cloud for inference acceleration. It generates new C++ network description based on our pre-designed synthesizable function template library. The structural optimization for FPGA implementation is provided during the synthesizable code generation. The purpose of Cloud-Dnn is to take advantage of HLS design methodology and providing more flexible and understandable DNN acceleration on cloud FPGAs.

## Hardware requirement
- Local cluster
-UltraScale+ VU118 board with PCIe connection
- AWS cluster
-AWS F1.2Xlarge instance

## Software requirement
- Python
- GCC
- Xilinx vivado_hls 2018.1
- Xilinx vivado 2018.1
- Caffe and the required libraries (including Pycaffe)
- Drivers
-Local: UltraScale+ VU118 board XDMA driver
-AWS: AWS shell IP support, EDMA driver

## Brief mannual

<details>
<summary><big><strong>GitHub Repository Structure</strong></big></summary>
```sh
CHaiDNN/
|
|-- LICENSE
|-- README.md
|-- netGenerator
|   |-- paramExtractor
|   |-- dse
|   `-- netGen
|-- scripts
|   |-- compile
|   |-- hls_impl
|   `-- mem_gen
|-- software
|   |-- local_acc
|   `-- cloud_acc
`-- fpga_cnn
    |-- src
    `-- testbench
    
```
</details>

