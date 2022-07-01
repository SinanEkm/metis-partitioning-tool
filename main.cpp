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
// g++ metis.cpp -lmetis
int main(){
    
    idx_t nWeights  = 1;
    idx_t nParts    = 4;
    idx_t *nVertices;
    idx_t objval; 
    idx_t numOfRow, numOfCol = 0, numOfVal = 0;
    nVertices = &numOfRow;
    
    idx_t *I, *J;
    double *val;
    idx_t *csr_col, *csr_row;
    double *csr_val;
    
    std::ifstream file("1138_bus.mtx");

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
    
    for(idx_t i = 0; i < numOfVal; i++){
        csr_row[I[i] + 1]++;
   }
    for(idx_t i = 0; i < numOfRow; i++){
        csr_row[i+1] += csr_row[i];
    }
    std::cout<<std::endl;
    
    for(idx_t i = 0; i < numOfVal; i++){
        csr_col[csr_row[I[i] + 1]] = J[i];
        csr_val[csr_row[I[i]+1]++] = val[i];
    }
    
    idx_t part[*nVertices];

    //Adding options
    idx_t options[METIS_NOPTIONS];
    options[METIS_OPTION_OBJTYPE] = METIS_OBJTYPE_CUT;
    
    int ret = METIS_PartGraphKway(nVertices, &nWeights, csr_row, csr_col,
				       NULL, NULL, NULL, &nParts, NULL,
				       NULL, NULL, &objval, part);
    
    std::cout << ret << std::endl;
    
    for(unsigned part_i = 0; part_i < numOfRow; part_i++){
	    std::cout << part_i << " " << part[part_i] << std::endl;
    }
     
    return 0;
}
