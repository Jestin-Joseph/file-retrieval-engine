# Install the packages
**Download**
```
wget https://registrationcenter-download.intel.com/akdlm/IRC_NAS/cca951e1-31e7-485e-b300-fe7627cb8c08/intel-oneapi-base-toolkit-2025.1.0.651_offline.sh
```
**Install**

*non interactive mode*
```
sudo sh ./intel-oneapi-base-toolkit-2025.1.0.651_offline.sh -a --silent --eula accept
```

**Set Env Variables**
```
source /opt/intel/oneapi/setvars.sh 
```

### For Nvidia GPUS

You will have to install the CUDA toolkit and drivers

[Read more here](https://developer.codeplay.com/products/oneapi/nvidia/2025.1.1/guides/get-started-guide-nvidia.html)

**Toolkit Installation**
```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.1-1_all.deb
sudo dpkg -i cuda-keyring_1.1-1_all.deb
sudo apt-get update
sudo apt-get -y install cuda-toolkit-12-9
```


**Driver Installation**
```
sudo apt-get install -y nvidia-open
```

**Install oneAPI for Nvidia GPU**
```
curl -LOJ "https://developer.codeplay.com/api/v1/products/download?product=oneapi&variant=nvidia&filters[]=linux"
```
