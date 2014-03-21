#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "BoW_testing/sift.h"
#include "BoW_testing/hist.h"
#include "BoW_testing/query.h"
#include "BoW_testing/tf_td.h"


int main(int argc, char *argv[]){
	
	int Num_Cluster=50;
	int siftcols = 128;
	//int no_of_images=558;
	int no_top_image=10;

	int no_of_images=10;
	
	_tfid t;
	
	
	ifstream infilecenter;
	ifstream infilehistogram;
	ifstream pathfile;
	ifstream labelmatfile;
	ifstream descfile;
	vector<string> files = vector<string>();
	string line;
	
	
	pathfile.open("training.txt");
	infilecenter.open("clustercenter10.txt");
	infilehistogram.open("histogram10.txt");
	labelmatfile.open("labelmat10.txt");
	descfile.open("descriptor10.txt");
	
				if(!infilecenter){
					cerr << "Can't open the file"<<endl;
					exit(1);
				}
				if(!infilehistogram){
					cerr << "Can't open the file"<<endl;
					exit(1);
				}
				if(!labelmatfile){
					cerr << "Can't open the file"<<endl;
					exit(1);
				}
	Mat cluster_center(Num_Cluster,siftcols,CV_64F);			
	char center[256];
	int rows=0;
	int cols =0;
				while(!infilecenter.eof()){
					infilecenter>>center;
					double c = atof(center);
					cluster_center.at<double>(rows,cols)=c;
					cols++;
						if(cols==siftcols){
							rows++;
							cols=0;
						}
				}
	
	
	if(pathfile.is_open()){
						while(pathfile.good()){
							getline(pathfile,line);
							files.push_back(line);
						}
						pathfile.close();
				}
	
				

//Reading the descriptor_count
	char des[256];
	int  desarr[no_of_images];
    rows =0;
	int total_descriptor=0;
				while(!descfile.eof()){
					descfile>>des;
					desarr[rows]=atoi(des);
					total_descriptor = total_descriptor+desarr[rows];
					//cout<<desarr[rows]<<" ";
					rows++;
				}
	cout<<endl;
	total_descriptor=total_descriptor-desarr[rows-1];
	
	
	//Reading the label mat file
	char label[256];
	Mat label_mat(total_descriptor,1,CV_8U);
	rows=0;
				while(!labelmatfile.eof()){
					labelmatfile>>label;
					//cout<<label<<" ";
					label_mat.at<int>(rows,0)=atof(label);
					rows++;
				}
				cout<<endl;
	//cout<<label_mat<<endl;
	
		_query qy;
		list<double> hist_prob;
	    list<double> hist_probsort;
	    
		Mat queryImage = imread(argv[1],0);
		
		Mat tempq=qy.give_queryhist(queryImage,Num_Cluster,cluster_center);
		
		cout<<endl;
		
		
		Mat tf_idmat(no_of_images,Num_Cluster,CV_32F,Scalar(0)); 
		Mat dftmat(1,Num_Cluster,CV_8U,Scalar(0));
		
		Mat tftdmatrix = t.tftd(label_mat,desarr, Num_Cluster,no_of_images); 
		//cout<<tftdmatrix<<endl;
		Mat dftmatrix = t.dft(dftmat,tftdmatrix,Num_Cluster,no_of_images);
		//cout<<dftmatrix<<endl;
		
		Mat tfidmat =  t.tf_id(tf_idmat,tftdmatrix,dftmatrix,Num_Cluster,no_of_images);
		
		//cout<<tfidmat<<endl;
		
		
		 for(int i=0;i<tf_idmat.rows;i++){
			float sum=0;
			for(int j=0;j<tf_idmat.cols;j++){
				sum = sum + tf_idmat.at<float>(i,j);
			}
			if(sum!=0){
				tf_idmat.row(i) = tf_idmat.row(i).mul(1.0/sum);}
		}
		//cout<<"tf_idnormalised "<<tf_idmat<<endl;
		
		
		
		list<double> tfid_prob;
	    list<double> tfid_probsort;
		 for(int i=0;i<no_of_images;i++){
			double sum_temp1=0;
				for(int j=0;j<tf_idmat.cols;j++){
					sum_temp1=sum_temp1 +  tf_idmat.at<float>(i,j)*tempq.at<double>(0,j);
				}
				tfid_prob.push_back(sum_temp1);
				tfid_probsort.push_back(sum_temp1);
		}
		
			
	
		tfid_probsort.sort();
		tfid_probsort.reverse();	
		list<double> :: iterator it1;
		list<double> :: iterator its1 = tfid_probsort.begin();
		Mat indexofimage1;
		for(int i=0;i<no_top_image;i++){
			int id1=0;
				for(it1=tfid_prob.begin();it1!=tfid_prob.end();++it1){
					id1++;
					if(*its1==*it1){
						indexofimage1.push_back(id1-1);
						break;
					}
				}
				
		     cout<<"Element tfid "<<*its1<<endl;		
			++its1;
			}
	    //Accessing top 10 images
	    cout<<"Using TFid "<<endl;
				for(int i=0;i<no_top_image;i++){
					cout<<files[indexofimage1.at<int>(i,0)]<<endl;
				}			
	
return 0;
}
	
