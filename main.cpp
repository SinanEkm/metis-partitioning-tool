#include <chrono>
#include <cmath>
#include <cstddef> /* NULL */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <malloc/_malloc.h>
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
// ./run -i path/to/MMfile.mtx -k <PART NUMBER>

std::string returnFileName(std::string filePath);

int main(int argc, char *argv[]){
   
    //putting input arguments into a vector to make use easier
    std::vector<std::string> arguments(argv, argv + argc); 

    std::string filePath = "";
    idx_t nParts = 2; 

    //Reading the input arguments, specifically matrix name and part number.
    if(argc < 2){
        fprintf(stderr, "Usage: ./run -i [MatrixMarket File path] -k [Partition Number]\nExample: ./run -i matrices/Stanford/Stanford.mtx -k 4\n");
        exit(1);
    }else{
     
        for(int j = 0; j < arguments.size(); j++){
            if(arguments[j] == "-i"){
                filePath += arguments[j+1];
             }
            if(arguments[j] == "-k"){
                nParts = (idx_t)std::stoi(arguments[j+1]);
            }
        }
    }
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //Metis and CSR variables
    idx_t nWeights  = 1;
    idx_t *nVertices;
    idx_t objval; 
    idx_t numOfRow, numOfCol = 0, numOfVal = 0;

    nVertices = &numOfRow;
     
    idx_t *I, *J;
    double *val;
    idx_t *csr_col, *csr_row;
    double *csr_val;
    
    //reading the matrix market file; I, J and, val pointers used to store values. 
    std::ifstream file(filePath);

    while(file.peek() == '%') file.ignore(2048, '\n');

    file >> numOfRow >> numOfCol >> numOfVal;
     
    I = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    J = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    val = (double *)malloc(numOfVal * sizeof(double));

    for (idx_t i = 0; i < numOfVal; i++) {
        file>> I[i] >> J[i] >> val[i];
        I[i]--;
        J[i]--;
    }
    file.close();
    
    csr_col = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    csr_val = (double *)malloc(numOfVal * sizeof(double));
    csr_row = (idx_t *)malloc((numOfRow + 2) * sizeof(idx_t));
    
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
    free(I);
    free(J);
    free(val);


    idx_t part[*nVertices];

    //Adding options
    idx_t options[METIS_NOPTIONS];
    METIS_SetDefaultOptions(options);
    options[METIS_OPTION_SEED] = 0;

    int ret = METIS_PartGraphKway(nVertices, &nWeights, csr_row, csr_col,
				       NULL, NULL, NULL, &nParts, NULL,
				       NULL, options, &objval, part);
    free(csr_col);
    free(csr_row);
    free(csr_val);

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    
    //extracting the file name from the given path, and adjusting the output file.
    std::string fileName = returnFileName(filePath);
    std::string partVectorName = fileName + "_part" + std::to_string(nParts) + ".txt";

    std::fstream partVectorFile;
    partVectorFile.open(partVectorName,std::ios::out);

    std::cout<<"Elapsed time for partitioning "<< fileName<<" is:"<< elapsedTime << "ms." << std::endl;
    
    //Putting partitioned vector into a .txt file which is named matrixName_part[PARTNUMBER].TXT
    for(unsigned part_i = 0; part_i < numOfRow; part_i++){
        partVectorFile<<part[part_i]<<std::endl;
    }
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
