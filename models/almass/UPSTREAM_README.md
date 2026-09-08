This repository contains the source code for the ALMaSS methodology paper, Landscape, Population Manager, and Subpopulation Manager. To build and run it, you need to install cmake and a compiler (GCC or Microsoft Visual Studio). Here are the instructions for building and running the examples on a Linux machine:

After downloading the source code, open a terminal and navigate to the almass_methodology folder. Then, execute the following commands:

1. mkdir build
2. cd build
3. cmake ../source_code/
4. make
5. cp ./almass_cmd ../running_folder/
6. cd ../running_folder
7. OMP_NUM_THREADS=1 ./almass_cmd

This will perform an ALMaSS simulation without any species for 5 years using one thread. OMP_NUM_THREADS is used to set the number of thread uses. Please use one thread for the Theoretical1 and Skylark example. For other examples, multithread is supported. 

To run the examples provided in the Population Manager and Subpopulation Manager paper, open the BatchALMaSS.ini file in the running_folder directory. Modify the last line to select the desired simulation scenario:

-1: Only landscape, no species;
0: Theoretical1;
1: Theoretical2;
2: Subpopulation;
3: Skylark

The number in the second-last line determines the duration of the simulation.

The code documentation of the Landscape model can be found in [Landscape.pdf](https://almass.gitlab.io/almass_methodology/Landscape.pdf).

The code documentation of the Population Manager can be found in [PopulationManager.pdf](https://almass.gitlab.io/almass_methodology/PopulationManager.pdf).

The code documentation of the Subpopulation model can be found in [Subpopulation.pdf](https://almass.gitlab.io/almass_methodology/Subpopulation.pdf).