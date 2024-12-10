# installation

## quick start
First, clone this repo to your computer:
```
git clone https://github.com/this-is-pandora/gb-vm.git
```
Then, run these commands:
```
cmake -S . -B build
cmake --build
./build/gbvm
```
This requires SDL2, cmake, and g++ be installed on your computer.

## windows-specific installation
UNDER CONSTRUCTION

## running with docker
Make sure the docker daemon is up and running, then run these commands after cloning this repo to your computer:
```
docker build -t app .
docker run app
```