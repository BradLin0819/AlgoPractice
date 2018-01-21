#include<iostream>
using namespace std;
void swap(int& a,int& b);
void quicksort(int *a,int left,int right);
int main(){

	int a[6]={1,0,1,5,1,8};
	quicksort(a,0,5);
	for(int i=0;i<6;i++)
		cout<<a[i]<<' ';

return 0;
}
void swap(int& a,int& b){
	int temp=a;
	a=b;
	b=temp;
}
void quicksort(int *a,int left,int right){
	int i=left,j=right,mid=(left+right)/2;
	while(i<=j){
		while(a[i]<a[mid]) i++;
		while(a[j]>a[mid]) j--;
		if(i<=j){
			swap(a[i],a[j]);
			i++;
			j--;
		}
	}
	if(left<j)
		quicksort(a,left,j);
	if(i<right)
		quicksort(a,i,right);
}
