#This is the README file for the NN model generation.

Follow the steps below to generate the CNN accelerator:
1. Extract the parameters of an input CNN in paramExtractor/ folder. Copy the net_config_params.txt to dse/ folder.
2. Run design space exploration in dse/ folder and get the acc_ins_param.txt.
3. Mv acc_ins_param.txt to netGen folder to generate the accelerators.
