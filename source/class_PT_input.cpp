//
// Created by david on 9/28/16.
//

#include "class_PT_input.h"

bool input::folder_existence_check(){
    struct stat info;
    if( info.st_mode & S_IFDIR ) {
        printf("%Found directory: %s ", pathname);
        return true;
    }
    else {
        printf("Folder does not exist: %s\n", pathname);
        printf( "Error: Provided settings argument %d but can't find folder: %s\n", job_id, pathname );
        exit(1);
    }
}


bool input::file_existence_check() {
    struct stat buffer;
    std::string filename = pathname + std::to_string(job_id);
    return (stat (pathname.c_str(), &buffer) == 0);
}

bool input::file_existence_check(const std::string filename) {
    struct stat buffer;
    return (stat (filename.c_str(), &buffer) == 0);
}


int input::file_counter(){
    int returnedCount = 0;
    std::string filename = pathname + std::to_string(returnedCount) + ".dat";
    while(file_existence_check(filename)){
        returnedCount++;
        filename = pathname + std::to_string(returnedCount) + ".dat";
    }
    return returnedCount;

}

void input::load_settings_from_file() {
    std::ifstream infile;
    std::vector<std::string> lines;
    std::string line;
    std::string number;
    infile.open(filename);
    if (!infile.is_open()) {
        std::cout << "Could not open file with name: " << filename << std::endl;
        exit(5);
    }

}



ArrayXXd indata::read_file(string filename) {
    ifstream infile;
    vector<string> lines;
    string line;
    string number;
    infile.open(filename);

    if (!infile.is_open()) {
        cout << "Could not open file with name: " << filename << endl;
        MPI_Finalize();
        exit(5);
    }
    unsigned long int rows = 0, cols = 0;
    //Load all the lines first and count rows.
    while (getline(infile,line)) {
        lines.push_back(line);
        rows++;
    }
    //Now count the number of elements on the  first line
    stringstream stream(lines[0]);
    while (!stream.eof()) {
        stream >> number;
        cols++;
    }
    //Now make your matrix and fill it with the contents of line[].
    ArrayXXd result(rows, cols);
    int j;
    for (unsigned long int i = 0; i < rows; i++) {
        stringstream new_stream;
        new_stream << lines[i];
        j = 0;
        while (!new_stream.eof()) {
            new_stream >> number;
            result(i, j++) = std::stod(number);
        }
    }
    return result;
}