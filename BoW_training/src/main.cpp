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
//#include "sift.h"
#include "BoW_training/surf.h"
#include "BoW_training/hist.h"
#include "BoW_training/query.h"
#include "BoW_training/tf_td.h"

using namespace std;
using namespace cv;
//using namespace flann;

int no_top_image=10;


int getdir (string dir, vector<string> &files)
{
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}
	while ((dirp = readdir(dp)) != NULL) {
		if((string(dirp->d_name) !=".") && (string(dirp->d_name) !="..")){
			files.push_back(string(dirp->d_name));
		}
	}
	closedir(dp);
	return 0;
}

void startTraining(const char* trainingDirectory,vector<string> &files){
	int classCount = 0;
	int totalImages = 0;
	static int featureVectorLength = 0;
	DIR* dir;
	struct dirent *dirEntry;
	struct stat inode;
	char name[1000];
	dir = opendir(trainingDirectory);
	if (dir == 0) {
		cerr<<"Invalid Dir";
		exit(1);
	}
	while ((dirEntry=readdir(dir)) != 0) {   
		sprintf(name,"%s/%s",trainingDirectory,dirEntry->d_name); 
		lstat (name, &inode);




		//adding lines

		// test the type of file
		if (S_ISDIR(inode.st_mode)) {
			if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) continue;
			else {
				DIR *classDir;
				struct dirent *classDirEntry;
				struct stat classInode;
				classDir = opendir(name);
				if(classDir == 0) {
					// invalid dir
					cerr<<"Invalid dir";
				}
				// Start of per class directory contents
				string classPath(name);
				// reset image count
				int imgCnt = 0;
				while((classDirEntry = readdir(classDir)) != 0) {
					if(strcmp(classDirEntry->d_name, ".") == 0 || strcmp(classDirEntry->d_name, "..") == 0) continue;
					else {
						string dirName(name);
						string file(classDirEntry->d_name);
						string fileName = dirName + "/" + file;
						files.push_back(fileName);
						cout<<fileName<<endl;
					}
				}

				classCount++;
			}
		}				
	}
}


















int main(int argc, char *argv[])
{
	//string dir = string("/home/suryansh/Desktop/ComputerVision/Assignment3/BoW/ButterflyTrain/"); "/home/suryansh/Desktop/ComputerVision/Assignment3/assign-3-dataset/train"
	const char  *ptr ="dataset";

	ofstream trainingfile("temp/training.txt");
	ofstream frequencyfile("./temp/histogram600.txt");
	ofstream clustercenter("./temp/clustercenter600.txt");
	ofstream labelmat("./temp/labelmat600.txt");
	ofstream descriptorcount("./temp/descriptor600.txt");
	int Num_Cluster=600;




	vector<string> files = vector<string>();

	startTraining(ptr,files);



	for (int i = 0;i <files.size();i++) {
		//cout<<files[i]<<endl;
	}




	//Assignment of training images 
	Mat arr[files.size()];
	int  desarr[files.size()];		
	Mat training_descriptor;
	_surfdescriptor sd;

	//Storing image and descriptor for each training image
	for(int i = 0;i < files.size();i++){
		Mat image = imread(files[i],0);
		arr[i]=image;
		sd.give_surfdescriptor(image);
		desarr[i]=sd.descriptor_count;
		//cout<<desarr[i]<<" ";
		descriptorcount<<desarr[i]<<" ";
		training_descriptor.push_back(sd.descriptor);
	}
	cout<<endl;
	//cout<<training_descriptor.rows<<endl;
	//cout<<training_descriptor.cols<<endl;


	//Now making clusters 

	Mat label_mat(training_descriptor.rows,1,training_descriptor.type());
	Mat cluster_center(Num_Cluster,training_descriptor.cols,training_descriptor.type());

	double compactness=kmeans(training_descriptor,Num_Cluster,label_mat,TermCriteria(CV_TERMCRIT_ITER, 10, 0.0),0,KMEANS_PP_CENTERS, cluster_center);

	/**
	 * Storing the cluster center 
	 * */
	for(int i=0;i<cluster_center.rows;i++){
		for(int j=0;j<cluster_center.cols;j++){
			clustercenter<<cluster_center.at<float>(i,j)<<" ";
		}
		clustercenter<<"\n";
	}
	/**
	 * Storing the label matrix
	 * */						int count_inr=0;	
	for(int i=0;i<files.size();i++){
		int count = desarr[i]+count_inr;
		for(int j=count_inr;j<count;j++){
			labelmat<<label_mat.at<int>(j,0)<<" ";
		}
		labelmat<<"\n";
		count_inr=desarr[i];
	}



	/**
	 * Generating histogram matrix instead of tf-id
	 * */							
	//Now generating histogram for the each trained image
	int no_of_images = files.size();
	cout<<"No. of Training Sample"<<no_of_images<<endl;

	Mat histmat(no_of_images,Num_Cluster,CV_64F,Scalar(0));
	for(int im=0;im<no_of_images;im++){
		sd.give_surfdescriptor(arr[im]);
		_histcount hc;
		list<int> index = hc.give_histcount(cluster_center,sd.descriptor,Num_Cluster);
		list<int> :: iterator it = index.begin();

		int it_count =0;
		while(it!=index.end()){
			histmat.at<double>(im,*it)=histmat.at<double>(im,*it)+1;
			it_count++;
			it++;
		}
		//cout<<it_count<<endl;
		//Normalising the frequency
		histmat.row(im)=histmat.row(im).mul(1.0/it_count);
	}
	cout<<"Normalised Hist Mat"<<endl;
	cout<<histmat<<endl;

	for(int i=0;i<histmat.rows;i++){
		trainingfile<<files[i];
		trainingfile<<"\n";
		for(int j=0;j<histmat.cols;j++){
			//printf("%6.2lf",histmat.at<double>(i,j));
			frequencyfile<<histmat.at<double>(i,j);
			frequencyfile<<" ";
			//Writing to file to store the trained histogram
		}
		frequencyfile<<"\n";
		//cout<<endl;
	}


	/*for(int i=0;i<cluster_center.rows;i++){
	  for(int j=0;j<cluster_center.cols;j++){
	  cout<<cluster_center.at<double>(i,j)<<" ";
	  }
	  cout<<endl;
	  }*/		

	cout<<cluster_center<<endl;	
	//Now Trying for testing images


	_query qy;
	list<double> hist_prob;
	list<double> hist_probsort;
	Mat queryImage = imread(argv[1],0);

	Mat tempq=qy.give_queryhist(queryImage,Num_Cluster,cluster_center);

	cout<<endl;
	cout<<"Testing Image Histogram ####"<<endl;
	for(int i=0;i<tempq.rows;i++){
		for(int j=0;j<tempq.cols;j++){
			//printf("%6.2lf",tempq.at<double>(i,j));
		}
		//cout<<endl;
	}
	//Dot product
	//cout<<tempq;

	cout<<"Dot Product statistics with histogram method"<<endl; 
	for(int i=0;i<no_of_images;i++){
		double temp  = histmat.row(i).dot(tempq);
		hist_prob.push_back(temp);
		hist_probsort.push_back(temp);
		//cout<<temp<<endl;
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

		cout<<"Element"<<*its<<endl;		
		++its;
	}
	//Accessing top 10 images
	for(int i=0;i<no_top_image;i++){
		cout<<files[indexofimage.at<int>(i,0)]<<endl;
	}
	return 0;
}
