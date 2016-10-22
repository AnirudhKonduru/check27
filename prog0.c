#include <stdio.h>
#include <stdint.h>


unsigned long long no_inversions=0;
void mergeSort(unsigned long long array[],unsigned long long low,unsigned long long high);
void merge(unsigned long long array[],unsigned long long low,unsigned long long mid,unsigned long long high);

int main(){
    unsigned long long size;
    scanf("%llu",&size);
    unsigned long long arr[size];
    for(unsigned long long i=0;i<size;i++)
        scanf("%llu",&arr[i]);
        mergeSort(arr, 0, size-1);
        
        for(unsigned long long i=0; i<size ; i++)
            printf("%llu\n", arr[i]);
        
        printf("No of Inversions: %llu",no_inversions);
    
}

void mergeSort(unsigned long long array[],unsigned long long low,unsigned long long high){
    
    if(low<high){
     unsigned long long mid = low + (high - low)/2;
    
    
    mergeSort(array, low, mid);
    mergeSort(array, mid+1, high);
    
    merge(array, low, mid, high);
    }
}

void merge(unsigned long long array[],unsigned long long low,unsigned long long mid,unsigned long long high){
        
        unsigned long long size1 = mid-low+1;
        unsigned long long copy1[size1];
        unsigned long long size2 = high-mid;
        unsigned long long copy2[size2];
        
        for(unsigned long long i=0; i<size1;i++)
            copy1[i] = array[low+i];
        for(unsigned long long i=0;i<size2;i++)
        copy2[i] = array[mid+1+i];
    
        unsigned long long i=0,j=0,k=low;
        
        while(i<size1 && j<size2){
            
            if(copy1[i]<copy2[j]){
                array[k++] = copy1[i++];
            
            }
            else if(copy1[i]>= copy2[j]){
                    array[k++] = copy2[j++];
                    no_inversions += size1 - i;
                
            }
        }
        
        while(i<size1){
            array[k++] = copy1[i++];
        }
        while(j<size2){
            array[k++] = copy2[j++];
            no_inversions += size1 - i;
                
        }
        
}