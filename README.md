    Overview:
This program runs Conway's Game of Life using a number of threads on a square board.
The program takes as input:
    m - The size the board is m*m
    n - The number of threads is n*n and m % n = 0
    max - The number of generations to be simulated by the program
The program expects the arguments from command line input in that order.
Any additional arguments are ignored.
Incorrect input is rejected.

    Execution:
To run the program in Ubuntu, make sure ncurses is installed and then navigate to the directory containing the program and execute:
    make
    ./main <m> <n> <max>
Then just follow the program's instructions.