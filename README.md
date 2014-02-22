# ISS - Incredible/Inferior Sokoban Solver readme

A solver program for the puzzle game [Sokoban](http://en.wikipedia.org/wiki/Sokoban). Sokoban is NP-hard (PSPACE-complete) so the solver is not guaranteed to find a solution, but it solves the most common maps. The solver uses a bidirectional best-first search to find a valid solution as fast as possible, but this solution may not be the solution with the fewest moves.

Most code was created during an epic 48-hour hackathon fueled by cookies and red bull. As a result code quality has suffered a bit.

## Build and Run

Build the program by running make (g++ is required)

When the program is built, a binary named 'iss' will be located in the bin/ directory.

Usage:
    cat input | ./iss

A lot of test input is included in the tests folder

For example:
    cat tests/1.in | bin/iss
