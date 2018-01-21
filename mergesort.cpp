#include<iostream>
using namespace std;
void merge(int* a,int left,int mid,int right);
void mergesort(int* a,int left,int right);
int main(){

	int a[10]={1,8,1,5,1,1};
	mergesort(a,0,9);
	for(int i=0;i<10;i++)
		cout<<a[i]<<' ';


return 0;
}
void merge(int* a,int left,int mid,int right){
	int i=left,j=mid+1,k=0;
	int temp[right-left+1];
	while((i<=mid) && (j<=right)){
		if(a[i]<a[j])
			temp[k]=a[i++];
		else
			temp[k]=a[j++];
		k++;
	}
	while(i<=mid)
		temp[k++]=a[i++];
	
	while(j<=right)
		temp[k++]=a[j++];
			
	for(int i=0;i<=right-left;i++)
		a[i+left]=temp[i]; 
}
void mergesort(int* a,int left,int right){
	if(left<right){
		int mid=(left+right)/2;
		mergesort(a,left,mid);
		mergesort(a,mid+1,right);
		merge(a,left,mid,right);
	} 
}
