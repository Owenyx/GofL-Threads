#include <iostream>
#include <vector>
#include <ncurses.h>
#include <utility>
#include <string>
#include <thread>
#include <unistd.h>
#include <cstdlib>

using namespace std;

using VEC = vector<int>;
using MAT = vector<VEC>;
using POINT = pair<int, int>; // (row, column)
using REC = pair<POINT, POINT>;

bool is_all_digits(const string& str);
MAT initialize_board(int rows, int cols);
void print_board(const MAT& b);
void compute_section(MAT& current, MAT& next, REC area);

int main(int argc, char *argv[]) {

    double wait_time = 0.2; // Seconds between generations
    wait_time *= 1000000; //becuase usleep is used - takes microseconds

    // Acquire command-line input
    if (argc < 4) {
        cerr << "3 arguments expected, terminating...\n";
        return 1;
    }

    // Check that all the argv values are non-negative ints
    // 1 - 3 includes the first 3 command-line inputs, which is how many we expect
    for (int i = 1; i < 4; i++)
        if (!is_all_digits(string(argv[i]))) {
            cerr << "Arguments must be positive integers, terminating...\n";
            return 1;
        }

    // Retrieve input
    int m = atoi(argv[1]), n = atoi(argv[2]), max = atoi(argv[3]);

    if (m <= 0 || n <= 0 || max <= 0) {
        cerr << "Arguments must be positive integers, terminating...\n";
        return 1;
    }
    if (m % n != 0) {
        cerr << "Argument 1 must be evenly divisible by argument 2, terminating...\n";
        return 1;
    }

    // Display instructions
    cout << "Welcome to Game of Life" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "Click anywhere on the setup screen to spawn a cell, click it again to kill it" << endl;
    cout << "Press any key to finalize the setup and begin the simulations" << endl;
    cout << "Enter any input to continue to the setup screen : ";
    
    MAT current = initialize_board(m, m);
    print_board(current);
    usleep(wait_time);
    MAT next = current;

    // Divide board into sections
    vector<REC> sections; // will have n^2 sections
    for (int i = 0; i < m; i += n)
        for (int j = 0; j < m; j += n) 
            sections.emplace_back(REC(POINT(i, j), POINT(i + n-1, j + n-1)));

    for (int gen = 0; gen < max; gen++) {
        // Assign a thread to each section
        vector<thread> threads;
        for (REC area : sections) 
            threads.emplace_back(thread(compute_section, ref(current), ref(next), area));

        // Wait for threads to finish
        for (auto& t : threads) 
            if (t.joinable())
                t.join();

        // Update generation and print
        current = next;
        system("clear"); // clear screen
        print_board(current);
        usleep(wait_time);
    }
    return 0;
}


bool is_all_digits(const string& str) {
    for (char ch : str) {
        if (!isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }
    return true;
}

// Returns a GofL configuration set by the user using curses
MAT initialize_board(int rows, int cols) {
    // Make initial blank square board
    MAT board(rows, vector<int>(cols, 0));
    
    // Initialize input using curses
    MEVENT event;
    
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    mouseinterval(3);
    mousemask(ALL_MOUSE_EVENTS, NULL);
    
    while(1)
	{	
	    int c = getch();
		switch(c)
		{	
		    case KEY_MOUSE:
			    if(getmouse(&event) == OK)
			    
				    if(event.bstate & BUTTON1_PRESSED)
				    {	
					    if (event.y >= rows || event.x >= cols)  //if outside board bounds
					        continue;
					    
					    //flip cell state
					    ++board[event.y][event.x] %= 2;
					    mvprintw(event.y, event.x, board[event.y][event.x]?"X":" ");
					    
					    refresh(); 
				    }
		     break;
		     default : goto end; //pressing q finalizes input
		}
	}		
end:
    endwin ();

    return board;
}

// Expects a non-empty matrix, prints nothing if a cell is 0 and X otherwise
void print_board(const MAT& b) {
    int rows = b.size();
    int columns = b[0].size();
    for (auto r=0; r<rows; r++) {
        for (auto c=0; c<columns; c++)
            cout << (b[r][c]?"X":" ");
        cout << "\n";
    }
    return;
}

// Params: Boards of current and next generations and the designated area to compute
// The bounds given by area are inclusive
void compute_section(MAT& current, MAT& next, REC area) {
    // Assign section perimeter variables
    int row1 = area.first.first;
    int col1 = area.first.second;
    int row2 = area.second.first;
    int col2 = area.second.second;

    int surrounding;

    for (int r = row1; r <= row2; r++) {
        for (int c = col1; c <= col2; c++) {
            // For each cell, check surrounding 8 cells
            surrounding = 0;
            for (int r_check = r-1; r_check <= r+1; r_check++) {
                for (int c_check = c-1; c_check <= c+1; c_check++) {
                    // Validate checked cell's validity - within board bounds and not checking current cell
                    if 
                    (
                       r_check < 0 
                    || r_check > (int)current.size()-1
                    || c_check < 0
                    || c_check > (int)current[0].size()-1
                    || (r_check == r && c_check == c)
                    )
                        continue;

                    // Check the cell
                    if (current[r_check][c_check]) // If cell is live
                        surrounding++;
                }
            }
            // Done checking surrounding
            // Now assign cell to next generation board
            if (current[r][c] == 1) { // If cell is live
                // AND
                // Cell has less than 2 or greater than 3 neighbours, it dies
                if (surrounding < 2 || surrounding > 3)
                    next[r][c] = 0;
                // Otherwise cell lives
                else
                    next[r][c] = 1;
            }
            else { // Cell is dead
                // AND
                // Cell has exactly 3 neighbours, it lives
                if (surrounding == 3)
                    next[r][c] = 1;
                // Otherwise cell dies
                else
                    next[r][c] = 0;
            }
        }
    }
    return;
}