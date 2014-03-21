#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include <iostream>



class _query{
	public : Mat give_queryhist(Mat image,int Num_Cluster,Mat cluster_center){
		_siftdescriptor sd;
		sd.give_siftdescriptor(image);
		//cout<<"Desc"<<sd.descriptor<<endl;
		Mat histmatnew(1,Num_Cluster,CV_64F,Scalar(0));
		_histcount hc;
		int im=0;
							list<int> index=hc.give_histcount(cluster_center,sd.descriptor,Num_Cluster);
							list<int> :: iterator it = index.begin();
							//cout<<"DisplayingIndex"<<endl;
									
							float it_count=0;
								while(it!=index.end()){
									histmatnew.at<double>(im,*it)=histmatnew.at<double>(im,*it)+1;
									it++;
									it_count++;
								}
								histmatnew.row(im)=histmatnew.row(im).mul(1.0/it_count);
								//cout<<"Hist Mat"<<histmatnew<<endl;
	return(histmatnew);
	}
};
