# DESProgram
-------------

Author          : Yu-Ho, Cheng

Created         : October 1 , 2018

Last Modified   : September 24, 2018

Affiliation          : Georgia Institute of Technology


## Description
-------------

This is the program simulate discrete event simulation. More details can be found in documentation folder. Basically, we built two parts of program, one part is the simulation program which contains generator (create customer), queue station (where customer get served), fork (a channel to distribute customers to different next stations) and exit (where customer leaves). The other is configuration program which read and write the config file to perform simulation

## Folder Structure
-------------


├── document  # Documentation files 

├── txt       # Contains config.txt, stats.txt and log.txt

├── src       # Source files 
    
    ├── Configuration # Program to read, validate, write config, stats and log 
    ├── Sim # Libaray to simulate DES program

├── Makefile

└── README.md    

## Compilation & Experiment
-------------

Please run make under DESProgram folder.
```
make
```
Then you will find the folder will have several .o files, do not worry about that, the make clean command later will clean them up. 

Then please run exe file to simulate the experiment with required parameters and file path.

```
./cppsim experiment_time config_file_path stats_file_path log_file_path
```

Example:
```
./cpssim 100.0 txt/config.txt ./txt/stats.txt ./txt/log.txt
```

After running the above example command, you should be able to see the output **stats.txt** and **log.txt** files in ./txt folder. 

## Debug & Experiment Results
-------------

The terminal will output debug info for invalid parameters. The **log.txt** file will store the invalid config file information. The **stats.txt** will store the experiment results.

Config validation test contains:

Valid component
1. The number of components should be aligned with given number. The type should be known, and the ID should be unique.
2. Parameters in component should be either positive integer or double. 
3. The correct number of parameter of corresponding  different components, also the type of them. 

Valid connection
1. G should not be any predecessor component of anyone
2. G should not directly connect to X

