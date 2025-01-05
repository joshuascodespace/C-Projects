# C-Projects
This repository contains projects completed by me that illustrates my C experience.

I did not write set-up code myself; it was provided to me. I did write the body of main.c functions that called matrix.c functions, as well as the matrix.c functions itself. For any other file, aside from the ".h" files, I coded about half of the functions present myself.

### Basic-Matrix-Operations
This program performs operations on one matrix at a time, which include reading in from files, summing all elements, finding the max of all elements, inserting an element, creating a matrix, and more.

### Multiprocessing
Creates multiple processes to compute the maximum and summation across all elements in a matrix for faster computation. The code also allows user commands to be read in from a file in addition to the terminal.

### Networking
Reads in matrices from information on different computer servers using UDP networking and TCP networking.

### Multithreading
Program spawns multiple threads and uses it to sum through all elements and find the maximum of elements in a matrix. Two techniques are employed in this project; one uses threads spawned at the beginning of the program and maintains all threads throughout (The worker thread program), and the other uses threads being spawned solely for the tasks they are created for.
