# distributed_ray_tracer
Utilizes OpenMPI to distribute the workload of computing a ray traced image over SSH. SDL is used to render the image.
## Example render
![spheres](/img/spheres.bmp?raw=true)
## Speedup graph
Example of speedup achieved when workload was distributed to three Ubuntu machines on the same network.
![speedup](/img/speedup.PNG?raw=true)
## Installation
This project has only been tested on Ubuntu 18.04.
Install the required packages with APT before compiling.
### Required packages
```
libsdl1.2-dev
libsdl-image1.2
libsdl-image1.2-dev
libsdl-mixer1.2
libsdl-mixer1.2-dev
libsdl-net1.2
libsdl-net1.2-dev
lam4-dev
openmpi-bin
mesa-common-dev
libglu1-mesa-dev
libopenmpi-dev
libopenmpi2
```
## Usage
```bash
make
mpirun -np <number of processes> -hostfile <filename> build/app/ray_tracer
```
The hostfile contains an example list of computers in a local network and how many processes they can handle.
This file needs to be changed to list the IP addresses of computers in your network that will contribute to the computation.
Short example of a hostfile below.
```bash
localhost slots=8 max_slots=8
192.168.1.82 slots=4 max_slots=4
```
In this case, you are specifying that the localhost machine can handle 8 processes, and that the machine with IP address 192.168.1.82 can handle 4 processes.
the [set_up_ssh_keys.sh](/set_up_ssh_keys.sh) script can help with setting up the SSH keys required to distribute the workload over SSH.
