#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include <cmath>
#include <iostream>


using namespace std;
using namespace cv;

Mat tftd(Mat label_mat, int arr[], int Num_Cluster,int no_of_images){
	
	cout<<label_mat.rows<<endl;
			cout<<no_of_images<<endl;
			Mat tfcount(no_of_images,Num_Cluster,label_mat.type(),Scalar(0));
			
				int count_inr=0;
				int count =0;
				for(int i=0;i<no_of_images;i++){
					count = count+arr[i];
					//cout<<count_inr << " "<<count<<endl;
					//cout<<arr[i]<<endl;
						for(int j=count_inr;j<count;j++){
							int clno=label_mat.at<int>(j,0);
							//cout<<clno<<" ";
							tfcount.at<int>(i,clno) = tfcount.at<int>(i,clno)+1;
						}
						cout<<endl;
						count_inr=count;
				}
				//free(arr);
				return(tfcount);
}

Mat dft(Mat  tftdmatrix, int Num_Cluster,int no_of_images){

Mat dftmat(1,Num_Cluster,tftdmatrix.type(),Scalar(0));

		for(int i=0;i<Num_Cluster;i++){
			for(int j=0;j<no_of_images;j++){
				int digit = tftdmatrix.at<int>(j,i);
						if(digit>0){
							dftmat.at<int>(0,i)=dftmat.at<int>(0,i)+1;
						}
			}
		}
	return(dftmat);
}

Mat tf_id(Mat  tftd, Mat dftmat,int Num_Cluster,int no_of_images){
	
	Mat tf_idmat(no_of_images,Num_Cluster,CV_32F,Scalar(0));
	float ratio;
			for(int i=0;i<tf_idmat.rows;i++){
				for(int j=0;j<tf_idmat.cols;j++){
						if(dftmat.at<int>(0,j)!=0){
							ratio=log((double)no_of_images/dftmat.at<int>(0,j));
							//cout<<"Ratio="<<ratio<<" ";
						}
						else{
							ratio=0;	
						}
					tf_idmat.at<float>(i,j)=tftd.at<int>(i,j)*ratio;	
				}
			}
	return(tf_idmat);
}



