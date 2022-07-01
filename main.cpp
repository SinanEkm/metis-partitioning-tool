#include <cmath>
#include <cstddef> /* NULL */
#include <cstdlib>
#include <fstream>
#include <malloc/_malloc.h>
#include <metis.h>
#include <iostream>

// Install metis from:
// http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz

// Build with
// g++ metis.cc -lmetis
int main(){
    
    idx_t nVertices = 6;
    idx_t nEdges    = 7;
    idx_t nWeights  = 1;
    idx_t nParts    = 4;

    idx_t objval;
    
    
    idx_t numOfRow = 0, numOfCol = 0, numOfVal = 0;
    idx_t *I, *J;
    double *val;
    idx_t *csr_col, *csr_row;
    double *csr_val;
    
    std::ifstream file("1138_bus.mtx");

    while(file.peek() == '%') file.ignore(2048, '\n');

    file >> numOfRow >> numOfCol >> numOfVal;
    idx_t part[numOfRow]; 
    I = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    J = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    val = (double *)malloc(numOfVal * sizeof(double));

    for (idx_t i = 0; i < numOfVal; i++) {
        file>> I[i] >> J[i] >> val[i];
        I[i]--;
        J[i]--;
    }
    file.close();
    /* 
    for(int i = 0; i < 100; i++)
    {
        std::cout<<I[i]<< " "<< J[i] <<" "<<val[i]<<std::endl;
    }
    */
      
    csr_col = (idx_t *)malloc(numOfVal * sizeof(idx_t));
    csr_val = (double *)malloc(numOfVal * sizeof(double));
    csr_row = (idx_t *)malloc((numOfRow + 2) * sizeof(idx_t));
    
    for(idx_t i = 0; i < numOfVal; i++){
        csr_row[I[i] + 1]++;
        //csr_val[i] = val[i];
        //csr_col[i] = J[i];
    }
    for(idx_t i = 0; i < numOfRow; i++){
        csr_row[i+1] += csr_row[i];
    }
    //for(idx_t i = 0; i < numOfRow; i++){
    //    std::cout<<csr_row[i]<<" ";
    //}
    std::cout<<std::endl;
    
    for(idx_t i = 0; i < numOfVal; i++){
        csr_col[csr_row[I[i] + 1]] = J[i];
        csr_val[csr_row[I[i]+1]++] = val[i];
    }
    
    // Indexes of starting points in adjacent array

    // Weights of vertices
    // if all weights are equal then can be set to NULL
    //idx_t vwgt[nVertices * nWeights];
    
   /* 
     int ret = METIS_PartGraphRecursive(&nVertices,& nWeights, xadj, adjncy,
     				       NULL, NULL, NULL, &nParts, NULL,
     				       NULL, NULL, &objval, part);
    */
    std::cout<<numOfRow<<std::endl;
    //numOfRow+=10;
    idx_t options[METIS_NOPTIONS];
    options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
    idx_t asfas = 6;
    int ret = METIS_PartGraphKway(&numOfRow, &nWeights, csr_row, csr_col,
				       NULL, NULL, NULL, &nParts, NULL,
				       NULL, NULL, &objval, part);
    
    std::cout << ret << std::endl;
    
    for(unsigned part_i = 0; part_i < numOfRow; part_i++){
	std::cout << part_i << " " << part[part_i] << std::endl;
    }
    
   
    return 0;
}
