# **PC-Lint GUI**

## About

A Qt based GUI application written in C++ to use with the C/C++ static analysis tool PC-Lint or PC-Lint Plus

![alt text](Screens/000.png)

## Features

* Supports old PC-Lint and newer PC-Lint Plus versions
* Multithread support for PC-Lint (PC-Lint does not have the `-max_threads=n` option)
* Basic code editor with syntax highlighting
* Tree view of lint output to navigate through
* Lint various project solutions (Atmel Studio, Microchip Studio, Visual Studio etc)

Newer features to be implemented soon in no particular order

* Complete syntax highlighting
* Edit lint (.lnt) file; surpress/show lint messages etc
* Lint message help on code numbers

## Building

Build the main application in the folder `PC-Lint GUI` 

## Tests

Build the main application in the folder `PC-Lint GUI Test`

## Bugs

See `bugs.txt` for the list of bugs/features

## Release history

N/A 