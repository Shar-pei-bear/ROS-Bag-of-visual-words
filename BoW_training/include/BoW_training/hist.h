#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include <list>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;


class _histcount{
	
	public : list<int> give_histcount(Mat cluster_center,Mat imagedescriptor,int Num_Cluster){
			
			 list<double> d_list;
			 list<double> d_listsort;
			 list<double> :: iterator it;
			 list<int >idlist;
			
			 for(int k=0;k<imagedescriptor.rows;k++){ 
				int id=0;
			 	for(int y=0;y<cluster_center.rows;y++){
									float sum=0;
									for(int x=0;x<cluster_center.cols;x++){
									float valc = cluster_center.at<float>(y,x);
									float vald = imagedescriptor.at<float>(k,x);
									sum = sum + (valc-vald)*(valc-vald);
									//cout<<valc<<" ";
									//cout<<vald<<"\n";
									}
									sum = sqrt(sum); 
									d_list.push_back(sum);
									d_listsort.push_back(sum);
									//cout<<endl; 
									//cout<<"Distance from Cluster"<<y<<"="<<sum<<endl;
			 	}
					d_listsort.sort();
					//cout<<d_listsort.front()<<endl;
						for(it=d_list.begin();it!=d_list.end();++it){
							if(*it==d_listsort.front()){
								break;
							}
							else{
							id++;
							}
						}
			 idlist.push_back(id);	
			 d_list.clear();
			 d_listsort.clear();
		 	}
			return(idlist);	 
		 }
};
