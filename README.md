This is a leightweight Python wrapper around https://github.com/numactl/numactl. Currently supported only for Frontier supercomputer at OLCF.

## Requirements for compiling 
1.  Installed copy of `numactl`
3.  Setting the environment variable `NUMA_LIB` to the location where `numactl` libraries are installed

If a pre-installed copy of `numactl` is not available, run `git submodule init && git submodule update` to checkout a copy of `numactl` as a submodule. Run `make local_numactl`. This will install `numactl` and set `NUMA_LIB` appropriately.

Run `make` from `numa_api` home directory. 

## Supported functionality
1.  `get_cpu_affinity_to_pid` - Given a process id (PID), get the CPU the PID is running on. This also prints the CPU affinity to PID.
2.  `get_gpu_affinity_to_pid_Frontier` - Given a PID, get the proposed GPU for optimal CPU-GPU affinity on Frontier node architecure. This also prints the proposed GPU for optimal affinity to CPU based on the PID.

## Use in Python script
The below code snippet highlights the important aspects of working with `numa_api`. We use `ctypes` to call functions in the `numa_api` library. Paste this snippet of code towards the top of your driver script.
```python
import ctypes
import os

# Load the shared library
numa_api_lib = ctypes.CDLL('<path to libnuma_api.so>')

# Set the argument and return types of the function
numa_api_lib.get_gpu_affinity_to_pid_Frontier.argtypes = [ctypes.c_int, 
                                                          ctypes.POINTER(ctypes.c_int), 
                                                          ctypes.POINTER(ctypes.c_int)]
numa_api_lib.get_gpu_affinity_to_pid_Frontier.restype = None

# interfacing with numa_api to get the cpu/gpu affinity
def get_gpu_affinity(pid):
    gpu_list = (ctypes.c_int * 8)()  # 8 here corresponds to the number of GPUs on Frontier
    gpu_count = ctypes.c_int()

    numa_api_lib.get_gpu_affinity_to_pid_Frontier(pid, gpu_list, ctypes.byref(gpu_count))

    # Collect results
    return gpu_list, gpu_count.value
```
Call the `get_gpu_affinity` function close to where the device ids are being set using `torch.cuda.set_device`. Compare the device ids being set against the output of `get_gpu_affinity`!
```
pid = os.getpid()
gpu_list, gpu_count = get_gpu_affinity(pid)
```

