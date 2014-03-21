#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
#include <iostream>

using namespace cv;
using namespace std;


class _siftdescriptor{
	public : Mat descriptor;
	public : Mat descriptor_resized;
	public : int descriptor_count;

	public : void give_siftdescriptor(Mat image){
			 vector<KeyPoint> keypoint;
			 SiftFeatureDetector sift(0.03,10.0);
			 SiftDescriptorExtractor siftDesc;
			 sift.detect(image,keypoint);
			 siftDesc.compute(image,keypoint,descriptor);
			 descriptor_count = descriptor.rows;
			 Mat descriptor_new(keypoint.size(),130,CV_32F,Scalar(0));
								for(int i=0;i<descriptor.rows;i++){
									for(int j=0;j<descriptor.cols;j++){
										descriptor_new.at<float>(i,j)=descriptor.at<float>(i,j);
									}
								}
			 vector<KeyPoint> :: iterator it = keypoint.begin();
			 int count =0;
					while(it!=keypoint.end()){
						float x = (*it).pt.x;
						float y = (*it).pt.y;
							descriptor_new.at<float>(count,128)=x;
							descriptor_new.at<float>(count,129)=y;
							count++;
							it++;
							//cout<<"[x,y] = "<<(*it).pt.x<<" "<<(*it).pt.y<<endl;
					}
			descriptor_new.copyTo(descriptor_resized);
			 //drawKeypoints(image,keypoint,image,Scalar(255,255,255),DrawMatchesFlags :: DRAW_RICH_KEYPOINTS);
		 }
};
