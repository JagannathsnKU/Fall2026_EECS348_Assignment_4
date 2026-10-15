/**
 * Program Name: EECS 348 Assignment 4: Sudoku Solver
 * Brief Description: A recursive object-oriented C++ program that solves 
 *                    partially completed 9x9 Sudoku puzzles using depth-first 
 *                    search with backtracking, printing all solutions or reporting 
 *                    if no solution exists.
 * Inputs: Text files containing 9x9 Sudoku grids (puzzle1.txt through puzzle5.txt), 
 *         where blank cells are represented by underscores ('_'), periods ('.'), or '0'.
 * Outputs: Console display of the puzzle file name, initial puzzle grid layout, 
 *          and all valid solved grids (or "No solution found").
 * Author: Jagannath Sritinath Nair
 * Creation Date: October 15, 2026
 * Revision Date: October 15, 2026
 * Revisions:       Refined file parsing logic for cross-platform blank formats, 
 *                  implemented modular object-oriented architecture across 
 *                  SudokuGrid, SudokuSolver, and PuzzleRunner classes, and 
 *                  added comprehensive prologue and in-line block comments.
 * Collaborators: None
 * External Sources : Claude 3.5 Sonnet (https://claude.ai)

 **/

#include <array>    // Include library for std::array data structures
#include <fstream>  // Include library for file input and output stream operations
#include <iostream> // Include library for standard console input and output streams
#include <string>   // Include library for std::string container manipulation
#include <vector>   // Include library for dynamic std::vector array containers
#include <utility>  // Include library for utility functions like std::move

/**
 * Source: Claude 3.5 Sonnet
 * Other Collaborators: None
 **/
// SudokuGrid class: encapsulates board data, file loading, validation rules, and grid rendering.
class SudokuGrid { // Begin declaration of the SudokuGrid class
public: // Access specifier allowing public member functions
    static constexpr int SIZE = 9; // Define constant static integer for grid size (9x9)
    static constexpr int BLOCK = 3; // Define constant static integer for sub-grid block size (3x3)

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    SudokuGrid() { // Constructor definition for initializing the grid state
        // Iterate through each row in the 2D array and fill with 0
        for (auto& row : cells_) { // Range-based for loop traversing each row array by reference
            row.fill(0); // Set every element in the current row array to integer 0
        } // Close the for loop body
    } // Close the default constructor body

    /**
     * Source: Claude 3.5 Sonnet (modified by author for robust token extraction)
     * Other Collaborators: None
     **/
    bool loadFromFile(const std::string& filename) { // Function definition to load grid from file, returning boolean success
        // Open file stream for reading
        std::ifstream in(filename); // Declare input file stream object linked to given filename
        // Return false if file cannot be opened
        if (!in) return false; // Conditional check evaluating if stream opened successfully

        int count = 0; // Initialize cell counter integer variable to zero
        std::string token; // Declare string token for reading space-separated or compact entries
        // Read token by token, automatically skipping whitespace and newlines
        while (in >> token) { // While loop extracting tokens from input stream
            // Process each character within the extracted token string
            for (char ch : token) { // Range-based for loop iterating through characters in token
                // Validate that grid does not exceed 81 cells
                if (count >= SIZE * SIZE) return false; // Check for overflow of total allowable cells
                int value; // Declare integer variable to store parsed cell value
                // Convert character digits '1'-'9' to integer values 1-9
                if (ch >= '1' && ch <= '9') { // Check if character is a valid digit between '1' and '9'
                    value = ch - '0'; // Convert character digit to corresponding integer value
                } // Close if block for digit parsing
                // Map blank cell indicators ('_', '.', '0') to integer 0
                else if (ch == '_' || ch == '.' || ch == '0') { // Check if character represents a blank cell placeholder
                    value = 0; // Assign integer 0 to represent an empty cell
                } // Close else-if block for blank cell parsing
                // Return false if an unexpected character is encountered
                else { // Catch-all block for any unrecognized characters in file
                    return false; // Return false indicating invalid file format
                } // Close else block
                // Store parsed value into the appropriate grid position
                cells_[count / SIZE][count % SIZE] = value; // Calculate 2D row and column indices and store parsed value
                ++count; // Increment total parsed cell counter by one
            } // Close inner character loop
        } // Close while loop for reading file tokens
        // Ensure exactly 81 valid cells were successfully read
        return count == SIZE * SIZE; // Return true if exactly 81 cells were loaded, false otherwise
    } // Close loadFromFile function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    int get(int r, int c) const { // Inline getter function returning const integer value at specified coordinates
        return cells_[r][c]; // Return the cell value at row r and column c
    } // Close get function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    void set(int r, int c, int v) { // Inline setter function assigning value v to specified coordinates
        cells_[r][c] = v; // Assign integer v to grid cell at row r and column c
    } // Close set function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    bool findEmptyCell(int& row, int& col) const { // Function finding first empty cell, returning references to row and col
        // Iterate through each row
        for (int r = 0; r < SIZE; ++r) { // Outer loop iterating through rows 0 to 8
            // Iterate through each column in the current row
            for (int c = 0; c < SIZE; ++c) { // Inner loop iterating through columns 0 to 8
                // If cell is 0, it is empty; record coordinates and return true
                if (cells_[r][c] == 0) { // Check if the current cell is unassigned (0)
                    row = r; // Assign current row index to output parameter reference
                    col = c; // Assign current column index to output parameter reference
                    return true; // Return true indicating an empty cell was found
                } // Close if statement checking for empty cell
            } // Close inner for loop
        } // Close outer for loop
        // Return false if no empty cells remain (grid is fully solved)
        return false; // Return false indicating no empty cells remain
    } // Close findEmptyCell function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    bool isValidPlacement(int r, int c, int v) const { // Function checking legality of placement v at (r, c)
        // Check row and column constraints
        for (int i = 0; i < SIZE; ++i) { // Loop iterating through indices 0 to 8 for row and column checks
            // Check if value already exists in the same row (excluding current cell)
            if (i != c && cells_[r][i] == v) return false; // Return false if value v appears elsewhere in row r
            // Check if value already exists in the same column (excluding current cell)
            if (i != r && cells_[i][c] == v) return false; // Return false if value v appears elsewhere in column c
        } // Close for loop for row and column validation
        // Determine top-left corner coordinates of the 3x3 sub-grid block
        const int br = (r / BLOCK) * BLOCK; // Calculate top-left row index for the 3x3 block
        const int bc = (c / BLOCK) * BLOCK; // Calculate top-left column index for the 3x3 block
        // Check 3x3 block constraints
        for (int i = br; i < br + BLOCK; ++i) { // Outer loop iterating through rows of the 3x3 block
            for (int j = bc; j < bc + BLOCK; ++j) { // Inner loop iterating through columns of the 3x3 block
                // Check if value exists within the 3x3 block (excluding current cell)
                if ((i != r || j != c) && cells_[i][j] == v) return false; // Return false if value v exists within the 3x3 block
            } // Close inner block for loop
        } // Close outer block for loop
        // Return true if placement is valid across row, column, and block
        return true; // Return true indicating placement is completely valid
    } // Close isValidPlacement function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    std::vector<int> candidates(int r, int c) const { // Function returning vector of valid integer candidates for cell (r, c)
        std::vector<int> result; // Initialize dynamic vector container to hold valid candidate numbers
        // Test numbers 1 through 9
        for (int v = 1; v <= 9; ++v) { // Loop iterating through possible digits 1 through 9
            // If placement is valid, add to candidate vector
            if (isValidPlacement(r, c, v)) { // Check if digit v is a legal placement
                result.push_back(v); // Append valid candidate digit to result vector
            } // Close if statement for valid placement check
        } // Close for loop testing digits 1 to 9
        return result; // Return populated vector of candidate numbers
    } // Close candidates function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    bool givensAreConsistent() const { // Function validating that pre-filled clues do not violate Sudoku rules
        // Iterate through all cells on the board
        for (int r = 0; r < SIZE; ++r) { // Loop traversing rows 0 to 8
            for (int c = 0; c < SIZE; ++c) { // Loop traversing columns 0 to 8
                // If cell contains a given clue, verify its validity against the board
                if (cells_[r][c] != 0 && !isValidPlacement(r, c, cells_[r][c])) { // Check if non-zero cell conflicts with board state
                    return false; // Return false if inconsistency is detected among initial givens
                } // Close if statement checking clue consistency
            } // Close inner for loop
        } // Close outer for loop
        return true; // Return true if all initial clues are consistent
    } // Close givensAreConsistent function body

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    void print(std::ostream& os) const { // Function printing grid representation to output stream os
        // Iterate through rows
        for (int r = 0; r < SIZE; ++r) { // Loop iterating through each row 0 to 8
            // Print horizontal separator line between 3x3 block rows
            if (r > 0 && r % BLOCK == 0) { // Check if row index is a multiple of 3 (excluding row 0)
                os << "------+-------+------\n"; // Print horizontal divider string to output stream
            } // Close if statement for horizontal divider
            // Iterate through columns
            for (int c = 0; c < SIZE; ++c) { // Loop iterating through each column 0 to 8
                // Print vertical separator line between 3x3 block columns
                if (c > 0 && c % BLOCK == 0) { // Check if column index is a multiple of 3 (excluding column 0)
                    os << "| "; // Print vertical pipe separator to output stream
                } // Close if statement for vertical divider
                // Print underscore for blanks (0) or the actual digit character
                os << (cells_[r][c] == 0 ? '_' : static_cast<char>('0' + cells_[r][c])); // Print underscore for 0 or character digit
                // Add space formatting between cells
                os << (c < SIZE - 1 ? " " : ""); // Print space between cells unless it is the last cell in row
            } // Close inner for loop for columns
            os << '\n'; // Print newline character at the end of each row
        } // Close outer for loop for rows
    } // Close print function body

private: // Access specifier for private internal member variables
    // 2D std::array representing the 9x9 Sudoku board state
    std::array<std::array<int, SIZE>, SIZE> cells_; // 2D fixed-size array storing integer grid values
}; // Close class SudokuGrid definition

/**
 * Source: Claude 3.5 Sonnet
 * Other Collaborators: None
 **/
// SudokuSolver class: implements the recursive depth-first search backtracking algorithm.
class SudokuSolver { // Begin declaration of the SudokuSolver class
public: // Access specifier allowing public member functions
    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    explicit SudokuSolver(const SudokuGrid& puzzle) : working_(puzzle) {} // Constructor initializer list copying puzzle into working grid

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    const std::vector<SudokuGrid>& solveAll() { // Public method initiating solver and returning const reference to solutions vector
        solutions_.clear(); // Clear any previous solutions from vector
        // Only run search if initial puzzle givens are consistent
        if (working_.givensAreConsistent()) { // Check if initial puzzle givens pass consistency validation
            search(); // Invoke recursive search algorithm
        } // Close if statement for consistency check
        return solutions_; // Return reference to collected solutions vector
    } // Close solveAll function body

private: // Access specifier for private internal helper methods and member data
    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    void search() { // Private recursive helper function executing depth-first search
        int r, c; // Declare row and column coordinate variables
        // Check if an empty cell exists; if none exist, all 81 cells are filled
        if (!working_.findEmptyCell(r, c)) { // Check if board has no empty cells remaining
            // Store completed valid board configuration into solutions container
            solutions_.push_back(working_); // Push completed SudokuGrid object onto solutions vector
            return; // Return from current recursive branch
        } // Close if statement for completed board check
        // Iterate through all legal candidate numbers for the empty cell
        for (int v : working_.candidates(r, c)) { // Range-based for loop iterating over valid candidate numbers
            // Try placing candidate v into cell (r, c)
            working_.set(r, c, v); // Assign candidate digit v to cell (r, c)
            // Recursively attempt to solve the next filled state
            search(); // Recursive call to search for subsequent cell solutions
            // Backtrack: reset cell back to 0 to explore alternative paths
            working_.set(r, c, 0); // Reset cell (r, c) back to 0 (backtracking step)
        } // Close for loop iterating over candidate numbers
    } // Close search function body

    // Internal working grid state for recursive traversal
    SudokuGrid working_; // SudokuGrid instance used for recursive traversal and state changes
    // Container storing all discovered valid solutions
    std::vector<SudokuGrid> solutions_; // Vector container holding all successfully discovered SudokuGrid solutions
}; // Close class SudokuSolver definition

/**
 * Source: Claude 3.5 Sonnet
 * Other Collaborators: None
 **/
// PuzzleRunner class: manages file execution, output stream formatting, and solution presentation.
class PuzzleRunner { // Begin declaration of the PuzzleRunner class
public: // Access specifier allowing public member functions
    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    explicit PuzzleRunner(std::string filename) : filename_(std::move(filename)) {} // Constructor initializing filename member via move semantics

    /**
     * Source: Claude 3.5 Sonnet
     * Other Collaborators: None
     **/
    void run(std::ostream& os) const { // Public function executing puzzle execution and reporting to stream os
        os << "==============================\n"; // Print decorative header separator line to stream
        os << "Puzzle file: " << filename_ << "\n\n"; // Print current puzzle filename label and value to stream

        SudokuGrid puzzle; // Declare SudokuGrid instance for loading file data
        // Attempt to load puzzle from file; handle load errors gracefully
        if (!puzzle.loadFromFile(filename_)) { // Attempt to parse puzzle from file; check for failure
            os << "Error: could not read a valid 9x9 puzzle from this file.\n\n"; // Print error message if loading fails
            return; // Return early from execution pipeline
        } // Close if statement checking load failure

        os << "Puzzle:\n"; // Print puzzle heading label to output stream
        puzzle.print(os); // Print initial puzzle layout to output stream
        os << '\n'; // Print trailing newline for spacing

        // Instantiate solver and execute search algorithm
        SudokuSolver solver(puzzle); // Instantiate SudokuSolver object with loaded puzzle instance
        const auto& solutions = solver.solveAll(); // Retrieve constant reference to vector of solved grids

        // If no solutions were found, output required failure message
        if (solutions.empty()) { // Check if solutions vector is empty
            os << "No solution found\n\n"; // Print required failure output string
            return; // Return early from execution pipeline
        } // Close if statement checking empty solutions

        // Output solution count and each solved grid configuration
        os << solutions.size() << (solutions.size() == 1 ? " solution" : " solutions") // Print number of solutions found with correct pluralization
           << " found:\n\n"; // Print conclusion header string
        for (std::size_t i = 0; i < solutions.size(); ++i) { // Loop iterating through each discovered solution
            if (solutions.size() > 1) { // Check if there are multiple solutions
                os << "Solution " << (i + 1) << ":\n"; // Print indexed solution label if multiple exist
            } // Close if statement for multiple solutions label
            solutions[i].print(os); // Print solved Sudoku grid layout to output stream
            os << '\n'; // Print trailing newline between solutions
        } // Close for loop iterating through solutions
    } // Close run function body

private: // Access specifier for private member data
    // Filename associated with the current puzzle instance
    std::string filename_; // String storing the target file path/name
}; // Close class PuzzleRunner definition

/**
 * Source: Claude 3.5 Sonnet
 * Other Collaborators: None
 **/
// Main function: entry point managing command-line inputs and coordinating the sequential puzzle runner workflow.
int main(int argc, char* argv[]) { // Main function entry point accepting command-line argument count and array
    std::vector<std::string> files; // Initialize string vector container to store puzzle filenames
    // If files are provided via command line arguments, use them
    if (argc > 1) { // Check if user provided custom command-line arguments
        for (int i = 1; i < argc; ++i) { // Loop iterating through provided command-line arguments
            files.emplace_back(argv[i]); // Add each argument filename to files vector
        } // Close for loop adding command-line files
    } // Close if statement handling custom arguments
    // Otherwise, default to processing all five required puzzle files
    else { // Fallback branch when no command-line arguments are provided
        for (int i = 1; i <= 5; ++i) { // Loop iterating from 1 to 5 for default puzzle files
            files.push_back("puzzle" + std::to_string(i) + ".txt"); // Construct default filename string and push to vector
        } // Close for loop adding default puzzle filenames
    } // Close else block

    // Iterate through each puzzle file and run solver pipeline
    for (const auto& f : files) { // Range-based for loop iterating over each filename in files vector
        PuzzleRunner(f).run(std::cout); // Instantiate PuzzleRunner for file f and execute run pipeline
    } // Close for loop iterating over puzzle files

    return 0; // Return integer 0 to operating system indicating successful program termination
} // Close main function body
