# DESProgram
-------------

Author          : Yu-Ho, Cheng

Created         : October 1 , 2018

Last Modified   : September 24, 2018

Affiliation          : Georgia Institute of Technology


## Description
-------------

This is the program simulate discrete simulation program. More details can be found in documentation folder. Basically, we are building two parts of program, one part is the simulation program which contains generator (create customer), queue station (where customer get served), fork (a channel to distribute customers to different next stations) and exit (where cusomter leaves).

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