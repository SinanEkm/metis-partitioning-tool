#include <chrono>
#include <cmath>
#include <cstddef> /* NULL */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <metis.h>
#include <iostream>
#include <string>
#include <vector>

// Install metis from:
// http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz

// Build with
// g++ metis.cpp -lmetis -o run -std=c++11
//
// Run with
// ./run -i path/to/MMfile.mtx -k <PART NUMBER> -o <OBJ_TYPE>
// OBJ_TYPE -> edge-cut ->> edge cut minimization
//          -> volume ->> Total communication volume minimization

std::string returnFileName(std::string filePath);

int main(int argc, char *argv[]){
   
    //putting input arguments into a vector to make use easier
    std::vector<std::string> arguments(argv, argv + argc); 

    std::string filePath = "";
    idx_t nParts = 2; 
    int objective = 1;
    std::string objective_name = "";


    //Reading the input arguments, specifically matrix name and part number.
    if(argc < 2){
        fprintf(stderr, "Usage: ./run -i [MatrixMarket File path] -k [Partition Number]\nExample: ./run -i matrices/Stanford/Stanford.mtx -k 4\n");
        exit(1);
    }else{
     
        for(int j = 0; j < arguments.size(); j++){
            if(arguments[j] == "-i"){
                filePath += arguments[j+1];
                continue;
             }
            if(arguments[j] == "-k"){
                nParts = (idx_t)std::stoi(arguments[j+1]);
                continue;
            }
            if(arguments[j] == "-o"){
                if(arguments[j+1] == "edge-cut"){
                    objective = 1;
                    objective_name += "edge-cut";
                }else if(arguments[j+1] == "volume"){
                    objective = 2;
                    objective_name += "volume";
                }
            }
        }
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //Metis and CSR variables
    idx_t nWeights  = 1;
    idx_t *nVertices;
    idx_t objval; 
    idx_t numOfRow = 0, numOfCol = 0, numOfVal = 0;

    nVertices = &numOfRow;


    //reading the matrix market file; I, J and, val pointers used to store values. 
    std::ifstream file(filePath);

    while(file.peek() == '%') file.ignore(2048, '\n');

    file >> numOfRow >> numOfCol >> numOfVal;

    idx_t* I = new idx_t[numOfVal]();
    idx_t* J = new idx_t[numOfVal]();
    double* val = new double[numOfVal]();
    
    for (idx_t i = 0; i < numOfVal; i++) {
        file>> I[i] >> J[i] >> val[i];
        I[i]--;
        J[i]--;
    }
    file.close();

    idx_t* csr_col = new idx_t[numOfVal]();
    idx_t* csr_val = new idx_t[numOfVal]();
    idx_t* csr_row = new idx_t[numOfRow + 2]();

    //Converting Coordinate format to CSR
    for(idx_t i = 0; i < numOfVal; i++){
        csr_row[I[i] + 1]++;
    }
    for(idx_t i = 0; i < numOfRow; i++){
        csr_row[i+1] += csr_row[i];
    }
    
    for(idx_t i = 0; i < numOfVal; i++){
        csr_col[csr_row[I[i] + 1]] = J[i];
        csr_val[csr_row[I[i]+1]++] = val[i];
    }

    delete [] I;
    delete [] J;
    delete [] val;


    idx_t* part = new idx_t[*nVertices]();
    
    //Adding options
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);
    options[METIS_OPTION_SEED] = 0;

    if(objective == 1){
        options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
    }else if(objective == 2){
        options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_VOL;
    }

    int ret = METIS_PartGraphKway(nVertices, &nWeights, csr_row, csr_col,
				       NULL, NULL, NULL, &nParts, NULL,
				       NULL, options, &objval, part);

    delete [] csr_val;
    delete [] csr_row;
    delete [] csr_col;

    //Measuring the execution time.
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    
    //extracting the file name from the given path, and adjusting the output file.
    std::string fileName = returnFileName(filePath);
    std::string partVectorName = fileName +"_metis_"+objective_name+ "_part" + std::to_string(nParts) + ".txt";

    std::fstream partVectorFile;
    partVectorFile.open(partVectorName,std::ios::out);

    std::cout<<"Elapsed time for partitioning "<< fileName<<" is:"<< elapsedTime << "ms." << std::endl;
    
    //Putting partitioned vector into a .txt file
    for(unsigned part_i = 0; part_i < numOfRow; part_i++){
        partVectorFile<<part[part_i]<<std::endl;
    }
    
    delete [] part;
    partVectorFile.close();
    
    return 0;
}

//extracts the file name from path
std::string returnFileName(std::string filePath){
    
    std::string delimiter1 = "/";
    std::string delimiter2 = ".";
    std::string result;
    size_t pos = 0;
    std::string token;
    
    while((pos = filePath.find(delimiter1)) != std::string::npos){
        filePath.erase(0,pos + delimiter1.length());
    }

    pos = filePath.find(delimiter2);
    result = filePath.substr(0,pos);
    return result;
}
