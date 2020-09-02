# csv_parser

This C++ application
    1. to read and process 3 csv files, as shown in the below 3 tables
    2. to calculate a total spending of a certain scope, 
   an application in C++
    1. to read and process 3 csv files, as shown in the below 3 tables
    2. to calculate a total spending of a certain scope, 
        e.g. the total spending of the residents on Amsterdam Aalbersestraat is: Amsterdam Aalbersestraat 34 and Amsterdam Aalbersestraat 98
                 Amsterdam Aalbersestraat 34 = (1 * 100 + 1 * 200 + 2 * 300) * 1.8
                 Amsterdam Aalbersestraat 98 = (2 * 400) * 1.8
 

 * Use this command from a shell to build the program
   g++ -o my_progam -std=c++1z -O2 -Wall -pedantic csvparser.cpp -lstdc++fs
   
* Design considerations 

 * - the program takes 3 arguments, 1 mandatory[city] 2 options [street, houseNo_lists],
    ./my_program Amsterdam
    ./my_program Amsterdam Aalbersestraat
    ./my_program Amsterdam Aalbersestraat 34,98
    
 * - the program assumes 3 csv input files exists in current working directory ["scopes.csv", "weights.csv", "age.csv"]
 * - the program assumes the first line of the 3 input files  is a header line.
 * - the program assumes the input files are well structured
                
