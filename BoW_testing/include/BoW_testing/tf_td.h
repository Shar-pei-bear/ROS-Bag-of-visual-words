#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include <cmath>
#include <iostream>


using namespace std;
using namespace cv;

class _tfid{
 public : Mat tftd(Mat label_mat, int arr[], int Num_Cluster,int no_of_images){
			//cout<<label_mat.rows<<endl;
			//cout<<no_of_images<<endl;
			Mat tfcount(no_of_images,Num_Cluster,label_mat.type(),Scalar(0));
			
				int count_inr=0;
				int count =0;
				for(int i=0;i<no_of_images;i++){
					count = count+arr[i];
					//cout<<count_inr << " "<<count<<endl;
					//cout<<arr[i]<<endl;
						for(int j=count_inr;j<count;j++){
							int clno=label_mat.at<uchar>(j,0);
							//cout<<clno<<" ";
							tfcount.at<uchar>(i,clno) = tfcount.at<uchar>(i,clno)+1;
						}
						//cout<<endl;
						count_inr=count;
				}
				//free(arr);
				return(tfcount);
}

public : Mat dft(Mat dftmat,Mat  tftdmatrix, int Num_Cluster,int no_of_images){

//Mat dftmat(1,Num_Cluster,CV_8U);

//cout<<tftdmatrix.rows<<endl;
//cout<<tftdmatrix.cols<<endl;
		for(int i=0;i<Num_Cluster;i++){
			for(int j=0;j<no_of_images;j++){
				int digit = tftdmatrix.at<uchar>(j,i);
				//cout<<digit<<" ";
						if(digit>0){
							dftmat.at<uchar>(0,i)=dftmat.at<uchar>(0,i)+1;
						}
			}
		}
		//cout<<endl;
	return(dftmat);
}

public : Mat tf_id(Mat tf_idmat,Mat  tftd, Mat dftmat,int Num_Cluster,int no_of_images){
	
	//Mat tf_idmat(no_of_images,Num_Cluster,CV_32F,Scalar(0));
	//Mat tf_idmat;
	float ratio;
			for(int i=0;i<tf_idmat.rows;i++){
				for(int j=0;j<tf_idmat.cols;j++){
						if(dftmat.at<uchar>(0,j)!=0){
							ratio=log((double)no_of_images/dftmat.at<uchar>(0,j));
							//cout<<"Ratio="<<ratio<<" ";
						}
						else{
							ratio=0;	
						}
					tf_idmat.at<float>(i,j)=tftd.at<uchar>(i,j)*ratio;	
				}
			}
	return(tf_idmat);
}
};


