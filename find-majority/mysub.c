#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// This function handles the situation that there is no result_4 group or size n < 100
// It takes the size n as parameter and returns the index of majority or 0 if the array has equal number of elements
int subtle(int n){ 
    // We set the first element in the array as groupA
    int GroupA = 1, GroupB = 0;
    int sizeA  = 1, sizeB = 0;
    int Q[4];
    Q[0]=1, Q[1]=n-2, Q[2]=n-1, Q[3]=n;
    
    // Record the QCOUNT result for the first and last three elements
    int last_result = QCOUNT(1,Q); 
    int curr_result;

    // Now keep using the last three elements and move the first element from 2 to n-3
    // This one by one comparsion indicates if two adjancent elements are the same and we assign them to different groups
    for (int i = 2; i!= n-2; ++i){
        Q[0]= i;
        curr_result = QCOUNT(1,Q);
        if (curr_result == last_result)
            ++sizeA;
        else {
            if (!GroupB)
                GroupB = i;
            ++sizeB;   
        }
    }
    if (sizeB == 0)
        return GroupA;

    Q[0]=1, Q[1]=2, Q[2]=3, Q[3]= 4;
    last_result = QCOUNT(1,Q);
    
    // In this step we are trying to figure out the components of the last three elemenets in the array
    for (int i = n-2; i != n+1; ++i){
        Q[0] = i;
        curr_result = QCOUNT(1,Q);
        if (last_result == curr_result)
            sizeA ++;
        else 
            sizeB ++;
    }

    if (sizeA == sizeB)
        return 0;
    if (sizeA > sizeB)
        return GroupA;
    else 
        return GroupB;
}

// This function takes in size n as parameter and returns the index of the majority in the array or 0 if no majority
int mysub(int n){
    // Check invalid inputs
    if (n < 10 || n > 10000)
        return -1;

    int GroupA, GroupB = 0;
    int sizeA = 0, sizeB = 0;
    int Q[4], result_4[2500],result_2[2500],result_2_2[2500];
    int count_4 = 0, count_2 = 0, count_2_2 = 0, count_0 = 0;
    int result;
    int leftover = n % 4;
    n = n - leftover;

    // Call QCOUNT on every four elements in the array and store the results into different groups. Ignore the leftover for now
    for (int i=1; i < n; i=i+4){
        for (int x=0; x<4;++x)
            Q[x] = i+x;
        result = QCOUNT(1, Q);
        if (result == 4)
            result_4[count_4++] = i;
        else if (result == 2)
            result_2[count_2++] = i;
        else 
            count_0 += 2;
    }
    
    // Since our efficent algorithm only handles the large size n, so for small size n, we will call subtle function to handle
    if (count_4 == 0 || n < 100)
        return subtle(n+leftover);
    
    int base = result_4[0];
    Q[0] = base, Q[1] = base+1;
    GroupA = base;
    sizeA += 4;
    
    // Handle the data in result_4 and put them into groupA and groupB
    int i = 1;
    for (; i < count_4-1; i = i+2){
        Q[2] = result_4[i];
        Q[3] = result_4[i+1];
        result = QCOUNT(1, Q);
        if (result == 4)
            sizeA += 8;
        else if (result == 2){
            sizeA +=4;
            sizeB +=4;  
        }
        else if (result == 0){
            if (!GroupB)
                GroupB = result_4[i];
            sizeB += 8;
        }
    }
    if (i == count_4-1 ){
        Q[2] = result_4[i];
        Q[3] = result_4[i]+1;
        result = QCOUNT(1, Q);
        if (result == 0){
            if (!GroupB)
                GroupB = result_4[i];
            sizeB +=4;
        }
        else if (result == 4)
            sizeA += 4;
    }
    
    // Compare the first two element in result_2 with base, and further divide the group into result_2_2
    for (int x=0; x < count_2; ++x){
        Q[2] = result_2[x];
        Q[3] = result_2[x]+1;
        result = QCOUNT(1, Q);
        if (result == 4){
            sizeA += 3;
            sizeB += 1;
        }
        else if (result == 0){
            if (!GroupB)
                GroupB = result_2[x];
            sizeB += 3;
            sizeA += 1;
        }
        else
            result_2_2[count_2_2++] = result_2[x]+2;
    }

    // Handle the data in result_2_2 and put them into groupA and groupB
    int x = 0;
    for (; x<count_2_2-1; x=x+2){
        Q[2] = result_2_2[x];
        Q[3] = result_2_2[x+1];
        result = QCOUNT(1, Q);
        if (result == 4){
            sizeA += 6;
            sizeB += 2;
        }
        else if (result == 0){
            if (!GroupB)
                GroupB = result_2_2[x];
            sizeB += 6;
            sizeA += 2;  
        }
        else if (result == 2){
            sizeB+=4;
            sizeA+=4;
        }
    }
    if (x < count_2_2){
        Q[2] = result_2_2[x];
        Q[3] = result_2_2[x]+1;
        result = QCOUNT(1, Q);
        if (result == 0){
            sizeB += 3;
            sizeA += 1;
        }
        else if (result == 4){
            sizeA += 3;
            sizeB += 1;
        }
    }
    sizeA+=count_0, sizeB+=count_0;
    
    // Handle any possible leftover by compare them with base one by one
    Q[2] = Q[1]+1;
    for (int i = 0; i != leftover ;++i){
        Q[3]= n+i+1;
        result = QCOUNT(1,Q);
        if (result == 2)
            ++sizeB;
        else if (result == 4)
            ++sizeA;
    }

    if (sizeA == sizeB)
        return 0;
    else if (sizeA > sizeB)
        return GroupA;
    else
        return GroupB;
    return 0;
}
