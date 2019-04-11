# Cloud-Dnn

## Introduction

Cloud-Dnn is an open-source framework that maps DNN models trained by Caffe to FPGAs in the cloud for inference acceleration. It generates C++ network description and generates the final hardware accelerator IPs with the input *.prototxt DNN description. The structural optimization for FPGA implementation is provided during the synthesizable code generation. The purpose of Cloud-Dnn is to take advantage of HLS design methodology and providing more flexible and understandable DNN acceleration on cloud-FPGAs (e.g., AWS F1).


## Hardware requirement
- Local cluster
-UltraScale+ VU118 board with PCIe connection
- AWS cluster
-AWS F1.2Xlarge instance

## Software requirement
- Python 3.5
- GCC
- Xilinx vivado_hls 2018.2 (2018.2.op for AWS)
- Xilinx vivado 2018.2 (2018.2.op for AWS)
- Caffe and the required libraries (including Pycaffe)
- Drivers
-Local: XDMA driver for UltraScale+ VU118 board
-AWS: AWS shell IP support, EDMA(XDMA for the latest version of AWS shell) driver


<details>
<summary><big><strong>GitHub Repository Structure</strong></big></summary>

```sh
Open-Dnn/
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
|   `-- sys_gen
|-- acc_runtime
|   |-- local_acc
|   `-- aws_acc
`-- fpga_cnn
    |-- src
    `-- testbench
    
```
</details>


## Brief manual

<details>
<summary><strong>Steps briefing for accelerator system generation</strong></summary>

<p align="center">
  <img width="700" height="295" src="https://github.com/microideax/Open-Dnn/tree/master/docs/flow.png">
</p>

Building an accelerator system for either local cluster or AWS cluster both requires:

1. DNN description analysis

1. C++ accelerator description generation

1. Accelerator IP generation with vivado_hls

1. Accelerator System configuration

1. Host function construction and compilation

The generation process are the same before step 4. The differences in the rest of the steps are explained with the detailed operations below.

</details>

<details>
<summary><strong>Build your own accelerator system</strong></summary>

Please follow the steps with a given alex.prototxt file and trained alex.caffemodel to build your accelerator system. make sure your environment is well set before starting this mannual.

1. Generating C++ accelerator description. After the repo is downloaded
   - execute
   		```sh
		cd Open-Dnn/netGenerator
        ./run_generator.sh -i alex.prototxt
        ```
run_generator.sh will automatically extract, analyze and generate the C++ code with the given alex.prototxt file. Since the alex.prototxt is given as the repo file, please only download the alex.caffemodel before executing the runtime software.

   >**:pushpin: TIPS:**
   > - The parameter extract script is sensitive to the format of the name\type in the prototxt, please use the "" for them.


2. Generating accelerator IPs. After the run_generator.sh script is executed successfully, the generated project is named as gen_proj and located at Open-Dnn/gen_proj. cd to the gen_proj/hls_proj/ folder.
   - execute
   		```sh
        ./syn.sh
        ```
syn.sh will generate the 3 sub-net IPs with the C++ code and scripts generated from previous step. One could also hack the accelerator　configurations in the acc_instance.h and call the testbench classes to verify the correctness of your change.

3. Accelerator system construction. The system construction scripts are provided within the generated project folder gen_proj/impl_proj. Before constructing the accelerator system, make sure the environment is well set and the sub-net IPs are generated and located properly.
   - Local Cluster
   		```sh
        cd Open-Dnn/gen_proj/impl_proj/local_impl/
        vivado build_system_local.tcl
        ```
   - AWS F1
   		```sh
        cd Open-Dnn/gen_proj/impl_proj/aws_impl/
        (follow the IPI design flow and move the folder to the correct path. Currently, 
        mkdir ~/aws-fpga/hdk/cl/examples/aws_ipi)
        vivado build_system_aws.tcl
        ```

4. Runtime software compilation.
   - Local Cluster
   		```sh
        After the bitstream of the accelerator system is generated and downloaded to the UltraScale+ VU118 board. Copy the acc_runtime/local_acc/ folder to your prefered execution path. Copy the config.h file from the gen_proj/hls_proj/src/ to the local_acc/ folder. Run compilation to get the executable file.
        ```
   - AWS F1
   		```sh
        After the AGFI (follow the instructions for AWS F1 AGFI generation) of the accelerator system is generated and downloaded to the AWS F1 instance. Copy the acc_runtime/aws_acc/ folder to your prefered execution path. Copy the config.h file from the gen_proj/hls_proj/src/ to the aws_acc/ folder. Run compilation to get the executable file.
        ```

</details>

<details>
<summary><strong>Playing with given demos on local cluster</strong></summary>

Please follow the steps to play with a given demo with bitstream and runtime software.


</details>

<details>
<summary><strong>Playing with given demos on AWS F1</strong></summary>

Please follow the steps to play with a given demo with AGFI and runtime software.


</details>


Notes:
For co-sim, need to manually comment the iteration in the hls_script.tcl.
Current hls_script.tcl is only for IP generation.

For more details, please refer to the paper below.

```sh
@inproceedings{Chen2019fpga,
 author = {Chen, Yao and He, Jiong and Zhang, Xiaofan and Hao, Cong and Chen, Deming},
 title = {Cloud-DNN: An Open Framework for Mapping DNN Models to Cloud FPGAs},
 booktitle = {Proceedings of the 2019 ACM/SIGDA International Symposium on Field-Programmable Gate Arrays},
 series = {FPGA '19},
 year = {2019},
 isbn = {978-1-4503-6137-8},
 location = {Seaside, CA, USA},
 pages = {73--82},
 numpages = {10},
 url = {http://doi.acm.org/10.1145/3289602.3293915},
 doi = {10.1145/3289602.3293915},
 acmid = {3293915},
 publisher = {ACM},
 address = {New York, NY, USA},
 keywords = {cloud computing, dnn accelerator, fpga, high-level synthesis, neural network, reconfiguration}
}
```
