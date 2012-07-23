// VirtualTennis.cpp : Defines the entry point for the console application.
//


#include "stdafx.h";

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>



CvCapture* capture;

using namespace cv;



enum ColorsForThresh {            
	Blue,
	Red,
	Hand
};

IplImage* GetThresholdedImage(IplImage* img, int colorForThresh)
{
	IplImage* imgHSV=cvCreateImage(cvGetSize(img),8,3);
	cvCvtColor(img,imgHSV,CV_BGR2HSV);

	IplImage* imgThreshed=cvCreateImage(cvGetSize(img),8,1);
	IplImage* imgThreshed1=cvCreateImage(cvGetSize(img),8,1);
	
	if(colorForThresh==Blue) {
		cvInRangeS(imgHSV,cvScalar(90,100,100),cvScalar(115,290,260),imgThreshed);
	}
	else if(colorForThresh==Red) {			
		cvInRangeS(imgHSV,cvScalar(35,100,0),cvScalar(50,155,255),imgThreshed);
		
	}
	else if(colorForThresh==Hand) {
		cvInRangeS(imgHSV,cvScalar(160,40,140),cvScalar(200,85,240),imgThreshed);
		cvInRangeS(imgHSV,cvScalar(0,40,140),cvScalar(10,85,240),imgThreshed1);
		cvAdd(imgThreshed,imgThreshed1,imgThreshed);
	}
	
	cvReleaseImage(&imgHSV);	

	return imgThreshed;
}


float getDistance(int x1,int y1,int x2,int y2) {
	double dx=(x1-x2);
	double dy=(y1-y2);
	dx*=dx;
	dy*=dy;
	return (sqrt(dx+dy));
}

void printBool(bool a) {
	if(a==true) printf(" true ");
	else printf(" false ");
}

void drawBall(IplImage* img,int x,int y,int radius) {
	cvCircle(img,cvPoint(x,y),radius,cvScalar(0,255,0),-1);
}

void drawCircle(IplImage* img,int x, int y, CvScalar color) {
	cvCircle(img,cvPoint(x,y),3,color,3);
}

int _tmain(int argc, _TCHAR* argv[]) {
	capture=cvCaptureFromCAM(0);
	
	int c;
	if(!capture)
    {
        printf("Could not initialize capturing...\n");
        return -1;
    }

	IplImage* frame=NULL;
	
	 IplImage *hand, *blue, *red;
	 int depth;
	 cv::Size size;
	 CvScalar s;

	 int posXBall=320,posYBall=240;
	 int posXGrip,posYGrip;

 	 bool testHold;
	 bool testGrip;
	 int GRIP_THRESHOLD=80;
	  int radius=30;

	frame=cvQueryFrame(capture);
	frame=cvQueryFrame(capture);


	size = cvGetSize(frame);
    depth = frame->depth;

	blue = cvCreateImage(size, depth, 1);
    cvZero(blue);  
	red = cvCreateImage(size, depth, 1);
    cvZero(red);  

	if(!frame) return -1;
    bool hold=false,hold1=false,hold2=false,hold3=false,hold4=false,hold5=false;
    bool grip=false,grip1=false,grip2=false,grip3=false,grip4=false,grip5=false;
	bool gripa=false;


	double moment10Blue,moment01Blue,moment10Red,moment01Red;
	double areaBlue,areaRed;
	CvMoments *momentsBlue;
	CvMoments *momentsRed;
	while(1) {
		hold5=hold4;
		hold4=hold3;
		hold3=hold2;
		hold2=hold1;
		hold1=hold;

		grip5=grip4; grip4=grip3; grip3=grip2; grip2=grip1; grip1=grip;

		frame=cvQueryFrame(capture);
		
		
		cvCvtColor(frame,frame,CV_BGR2HSV);
		s=cvGet2D(frame,240,320);
		cvCvtColor(frame,frame,CV_HSV2BGR);
		//hand=GetThresholdedImage(frame,Hand);
		blue=GetThresholdedImage(frame,Blue);
		cvSmooth(blue,blue,CV_MEDIAN,9);
	
		
		red=GetThresholdedImage(frame,Red);
		cvSmooth(red,red,CV_MEDIAN,9);
		

		momentsBlue = (CvMoments*)malloc(sizeof(CvMoments));
		momentsRed = (CvMoments*)malloc(sizeof(CvMoments));

        cvMoments(blue, momentsBlue, 1);
		cvMoments(red, momentsRed, 1);

		//BLUE CALCULATION
		moment10Blue = cvGetSpatialMoment(momentsBlue, 1, 0);
        moment01Blue = cvGetSpatialMoment(momentsBlue, 0, 1);



        areaBlue = cvGetCentralMoment(momentsBlue, 0, 0);
		

		static int posXBlue = 0;
        static int posYBlue = 0;
 
        int lastXBlue = posXBlue;
        int lastYBlue = posYBlue;
 
        posXBlue = moment10Blue/areaBlue;
        posYBlue = moment01Blue/areaBlue;

		posXBlue=(0.9*posXBlue)+(0.1*lastXBlue);
		posYBlue=(0.9*posYBlue)+(0.1*lastYBlue);

		 



		 //Red CALCULATION
		
		double moment10Red = cvGetSpatialMoment(momentsRed, 1, 0);
        double moment01Red = cvGetSpatialMoment(momentsRed, 0, 1);
        double areaRed = cvGetCentralMoment(momentsRed, 0, 0);

		static int posXRed = 0;
        static int posYRed = 0;

 
        int lastXRed = posXRed;
        int lastYRed = posYRed;
 
        posXRed = moment10Red/areaRed;
        posYRed = moment01Red/areaRed;

		posXRed=(0.9*posXRed)+(0.1*lastXRed);
		posYRed=(0.9*posYRed)+(0.1*lastYRed);
		
	  
		 
		 posXGrip=(posXRed+posXBlue)/2;
		 posYGrip=(posYRed+posYBlue)/2;

		 drawBall(frame,posXBall,posYBall,radius);
			
		 float distance=getDistance(posXRed,posYRed,posXBlue,posYBlue);
		
		 hold=(distance<=GRIP_THRESHOLD);
		//hold = if ur fingers are together
		 

		testHold=(hold || hold1 || hold2 || hold3);
		
		float gripDistance=getDistance(posXGrip,posYGrip,posXBall,posYBall);
		
		grip=(gripDistance<radius);

		testGrip=(grip || grip1 || grip2 || grip3);
		

		if(gripa && testGrip && hold) {
			 drawCircle(frame,posXGrip,posYGrip,cvScalar(0,0,255,0));
			 gripa=true;
		}
		 else if(testGrip && hold1==false && hold==true)
		 {	
			gripa=true;
			drawCircle(frame,posXGrip,posYGrip,cvScalar(0,0,255,0));
		}
		 else {
			gripa=false;
			printBool((gripDistance<radius));
			if(hold) drawCircle(frame,posXGrip,posYGrip,cvScalar(255,0,0,0));
		 }

		 if(gripa) {
			posXBall=posXGrip;
			posYBall=posYGrip;
		 }

		


		  //if(lastXRed>0 && lastYRed>0 && posXRed>0 && posYRed>0) cvCircle(frame,cvPoint(posXRed,posYRed),10,cvScalar(0,0,255),4,8,0);			
           
        
		 //if(lastXBlue>0 && lastYBlue>0 && posXBlue>0 && posYBlue>0) cvCircle(frame,cvPoint(posXBlue,posYBlue),10,cvScalar(255,0,0),4,8,0);
           
        
		 

		 



		//cvFlip(blue,blue,1);
		//cvFlip(red,red,1);	
		
		
		if(distance>GRIP_THRESHOLD)	printf("\nFar");
		else if(distance<=GRIP_THRESHOLD) printf("\nCLOSE");
	
		cvShowImage("videoBlue",blue);
		//cvShowImage("videoRed",red);
		

		//cvLine(frame,cvPoint(310,240),cvPoint(330,240),cvScalar(255,255,255));
		//printf("%f %f %f\n",s.val[0],s.val[1],s.val[2]);
		
		cvFlip(frame,frame,1);
		//cvShowImage("frame",frame);
        
		

		
		c=cvWaitKey(5);		
		
		if((char)c==27) break;
	}

	cvReleaseCapture(&capture);
	return 0;
}


