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
#include "BoW_testing/surf.h"
#include "BoW_testing/querysurf.h"
#include "BoW_testing/tf_td.h"




int main(int argc, char* argv[])//BoW_surf(Mat image){
{	
	int Num_Cluster=600;
	int surfcols = 64;
	int no_of_images=632;
	int no_top_image=10;

	//int no_of_images=10;
		_tfid t;
		string s=".jpg";
		string s1;



	
	//Trying to open the file	
	ifstream infilecenter;
	ifstream infilehistogram;
	ifstream pathfile;
	ifstream labelmatfile;
	ifstream descfile;
	ofstream result("./result/outputsurf.txt");
	//ofstream tf_idresult("../result/tf_idmatsurf.txt");
	ofstream hist_matresult("./result/hist_matsurf.txt");
	vector<string> files = vector<string>();
	string line;
	
	pathfile.open("./trained/training.txt");
	infilecenter.open("./trained/clustercenter600.txt");
	infilehistogram.open("./trained/histogram600.txt");
	labelmatfile.open("./trained/labelmat600.txt");
	descfile.open("./trained/descriptor600.txt");
	
	
	//Reading the taining data image path 
				if(pathfile.is_open()){
						while(pathfile.good()){
							getline(pathfile,line);
							files.push_back(line);
						}
						pathfile.close();
				}
				
				
	
	
	Mat cluster_center(Num_Cluster,surfcols,CV_64F);
	Mat histmat(no_of_images,Num_Cluster,CV_64F,Scalar(0));
	
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
		
	//Reading the cluster center file			
	char center[256];
	int rows=0;
	int cols =0;
				while(!infilecenter.eof()){
					infilecenter>>center;
					double c = atof(center);
					cluster_center.at<double>(rows,cols)=c;
					cols++;
						if(cols==surfcols){
							rows++;
							cols=0;
						}
				}
		
	//Reading the histogram file	
	char histo[256];
	rows=0;
	cols=0;
				while(!infilehistogram.eof()){
					infilehistogram>>histo;
					histmat.at<double>(rows,cols)=atof(histo);
					cols++;
							if(cols==Num_Cluster){
								rows++;
								cols=0;
							}
				}
		hist_matresult<<histmat<<endl;
	
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
	//cout<<total_descriptor<<"\n";
	
	
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
	
	// Testing image 	
		_querysurf qy;
		list<double> hist_prob;
	    list<double> hist_probsort;
	    
		Mat queryImage = imread(argv[1],0);
		//Mat queryImage = image; 
		
		Mat tempq=qy.give_queryhist(queryImage,Num_Cluster,cluster_center);
		
		cout<<endl;
		
		//Dot product
		
		//cout<<"Dot Product statistics"<<endl; 		
		for(int i=0;i<no_of_images;i++){
			double sum_temp=0;
				for(int j=0;j<histmat.cols;j++){
					sum_temp=sum_temp +  histmat.at<double>(i,j)*tempq.at<double>(0,j);
				}
				hist_prob.push_back(sum_temp);
				hist_probsort.push_back(sum_temp);
		}
		
			
	
		hist_probsort.sort();
		hist_probsort.reverse();	
		list<double> :: iterator it;
		list<double> :: iterator its = hist_probsort.begin();
		Mat indexofimage;
		for(int i=0;i<no_top_image;i++){
			int id=0;
				for(it=hist_prob.begin();it!=hist_prob.end();++it){
					id++;
					if(*its==*it){
						indexofimage.push_back(id-1);
						break;
					}
				}
				
		     //cout<<"Element"<<*its<<endl;		
			++its;
			}
	    //Accessing top 10 images
				for(int i=0;i<no_top_image;i++){
					//cout<<files[indexofimage.at<int>(i,0)]<<endl;
					result<<files[indexofimage.at<int>(i,0)]<<endl;
					//Mat image = imread(files[indexofimage.at<int>(i,0)],1);
					//std::stringstream ss1;
					//ss1<<i;
					//s1=ss1.str();
					//string s_new1 ="./Output/"+s1+s;
					//imwrite(s_new1,image);
				}
			
		indexofimage.release();
		histmat.release();
		cluster_center.release();	
		
		/**
		 * Using TF_ID matrix
		 * */
		 
		/*Mat tf_idmat(no_of_images,Num_Cluster,CV_32F,Scalar(0)); 
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
		tf_idresult<<tf_idmat<<endl;
		
		
		
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
	    //cout<<"Using TFid "<<endl;
				for(int i=0;i<no_top_image;i++){
					//cout<<files[indexofimage1.at<int>(i,0)]<<endl;
				}	*/		
	
	
	
	return 0;
}
