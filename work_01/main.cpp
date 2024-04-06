#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "line.cpp"

int main() {

    int file_line_count = 0;
    int number_of_retas = 0;
    
    std::vector< Line* > retas;
    std::vector< std::string > input_lines;
    
    std::ifstream input_file ("./input.txt");

    for( std::string line; getline( input_file, line ); )
    {
        input_lines.push_back(line);
    }

    number_of_retas = std::stoi(input_lines[0]);

    int current_line_number = 1;
    while ( current_line_number != number_of_retas + 1 ) {
        
        std::string current_line = input_lines[current_line_number];

        int x1 = int(current_line[0] - 48);
        int y1 = int(current_line[2] - 48);
        int x2 = int(current_line[4] - 48);
        int y2 = int(current_line[6] - 48);

        Line* newLine = new Line( x1, y1, x2, y2 );

        newLine->id = current_line_number;
        newLine->angular_coeficient = float( y2 - y1 ) / float( x2 - x1 );
        newLine->linear_coeficient = y1 - float(newLine->angular_coeficient * x1);

        retas.push_back( newLine );
        current_line_number++;
    }

    return 0;
}