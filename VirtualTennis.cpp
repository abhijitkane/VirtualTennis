// VirtualTennis.cpp : Defines the entry point for the console application.
//


#include "stdafx.h";

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>
#include <stdlib.h>



using namespace cv;
/*
Capture* capture;

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



void drawBall(IplImage* img,int x,int y,int radius) {
	cvCircle(img,cvPoint(x,y),radius,cvScalar(0,255,0),-1);
}


int _tmain(int argc, _TCHAR* argv[]) {
	capture=cvCaptureFromCAM(0);
	CvRect cr,cb;
	int directionX=1,directionY=1;
	int speed=20;	
	int posXBall=320,posYBall=240;
	int scoreB=0,scoreR=0;
	CvPoint redRect1,redRect2,blueRect1,blueRect2;

    int ratio=2;
	int rectW=60;
	int rectH=ratio*rectW;
	
	int areaLimit=3000;

	int radius=13;


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
		
	 
	
	 

	frame=cvQueryFrame(capture);
	frame=cvQueryFrame(capture);


	size = cvGetSize(frame);
    depth = frame->depth;

	blue = cvCreateImage(size, depth, 1);
    cvZero(blue);  
	red = cvCreateImage(size, depth, 1);
    cvZero(red);  
   int onScreenBlue=0;
	   int onScreenRed=0;
	if(!frame) return -1;
   
	char buffer [50];
	while(1==1) {
		

		frame=cvQueryFrame(capture);
		
		
		//cvCvtColor(frame,frame,CV_BGR2HSV);
		//s=cvGet2D(frame,240,320);
		//cvCvtColor(frame,frame,CV_HSV2BGR);
		//hand=GetThresholdedImage(frame,Hand);

		blue=GetThresholdedImage(frame,Blue);

	
		
		red=GetThresholdedImage(frame,Red);
		
		

		CvMoments *momentsBlue = (CvMoments*)malloc(sizeof(CvMoments));
		CvMoments *momentsRed = (CvMoments*)malloc(sizeof(CvMoments));

        cvMoments(blue, momentsBlue, 1);
		cvMoments(red, momentsRed, 1);

		//BLUE CALCULATION
		double moment10Blue = cvGetSpatialMoment(momentsBlue, 1, 0);
        double moment01Blue = cvGetSpatialMoment(momentsBlue, 0, 1);
        double areaBlue = cvGetCentralMoment(momentsBlue, 0, 0);
		

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
		



		redRect1=cvPoint(posXRed-(rectW/2),posYRed-(rectH/2));
		redRect2=cvPoint(posXRed+(rectW/2),posYRed+(rectH/2));
		blueRect1=cvPoint(posXBlue-(rectW/2),posYBlue-(rectH/2));
		blueRect2=cvPoint(posXBlue+(rectW/2),posYBlue+(rectH/2));

   
		//AREA
		//printf("Area blue: %f, green: %f\n",areaBlue,areaRed);
		if(areaBlue>areaLimit && (posXBlue+(rectW/2) < 320)) {
			onScreenBlue=1;
			//cvRectangle(frame,blueRect1,blueRect2,cvScalar(255,0,0,0),2);
			cvLine(frame,cvPoint((posXBlue+rectW/2),posYBlue-(rectH/2)),cvPoint((posXBlue+rectW/2),posYBlue+(rectH/2)),cvScalar(255,0,0,0),2);
			
		}
		else onScreenBlue=0;

		if(areaRed>areaLimit && (posXRed-(rectW/2)> 320)) {
			onScreenRed=1;
			//cvRectangle(frame,redRect1,redRect2,cvScalar(0,0,255,0),2);
			cvLine(frame,cvPoint((posXRed-rectW/2),posYRed-(rectH/2)),cvPoint((posXRed-rectW/2),posYRed+(rectH/2)),cvScalar(0,0,255,0),2);
			
		}
		else onScreenRed=0;
		

		if((onScreenRed==1) && (   abs(posXBall - ((posXRed-(rectW/2))-radius)) < 20     ) && (posYBall >= (posYRed-(rectH/2))-radius) && (posYBall <= posYRed+(rectH/2)+radius)) {
			directionX=-1;
			printf("GREEN %d\n",posXBall);
		}
		
		
		else if((onScreenBlue==1) && (abs (posXBall - (posXBlue+(rectW/2)+radius)) < 20 ) && (posYBall >= (posYBlue-(rectH/2))-radius) && (posYBall <= posYBlue+(rectH/2)+radius)) {
			directionX=1;
		}

		else if(posXBall <= 30) {
			directionX=1;
			scoreR++;
		}

		else if(posXBall >= 610) {
			directionX=-1;
			scoreB++;
		}
		

		if(posYBall <=30 ) directionY=1;
		if(posYBall >= 450) directionY=-1;
		
		
		
		
		 posXBall+=(speed*directionX);
		 posYBall+=(speed*directionY);
		 drawBall(frame,posXBall,posYBall,radius);

		 sprintf(buffer,"GREEN: %d  |  BLUE: %d",scoreR,scoreB);

	    cvFlip(frame,frame,1);
		//cvShowImage("blue",blue);
		cvShowImage("green",red);
		CvFont font;
		cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.5, 1.5, 0, 2, CV_AA);
		cvPutText(frame, buffer, cvPoint(210, 20), &font, cvScalar(0,0,0, 0));

		cvShowImage("frame",frame);
 
		

		
		c=cvWaitKey(1);		
		
		if((char)c==27) break;
	}

	cvReleaseCapture(&capture);
	return 0;
}
*/
