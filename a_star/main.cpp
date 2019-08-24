#include <iostream>
#include "a_star.h"


int main(int argc, char *argv[])
{
    if(argc != 3) 
        std::puts("Usage : ./a_star <input_file> <output_file>");


    a_star::AStar a_star;

    if (a_star.loadFile(argv[1])) 
        return -EOFILE;
    
    a_star.calculate();
    a_star.saveResult(argv[2]);

    return 0;
}
