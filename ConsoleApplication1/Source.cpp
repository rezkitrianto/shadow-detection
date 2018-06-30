#include <opencv2\core\core.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <math.h>
#include <iostream>
#include <string>
#include <highgui.h>

using namespace cv;

int main() {
	string fileName = "input.mp4";
	VideoCapture theVideo(fileName); //1. read the video

	if (!theVideo.isOpened()){
		throw "file is missing or corrupt";
	}
	else{
		Mat frame, gaussianBlurImage, yCbCrImage, contourImg;
		std::vector<std::vector<Point>> contours;
		int counter = 1;
		double threshold = 10000;

		while (theVideo.read(frame)){
			contourImg.release();
			//1. Gaussian
			GaussianBlur(frame, gaussianBlurImage, Size(5,5), 2, 0);
			
			//2. convert YCbCr
			cvtColor(gaussianBlurImage, yCbCrImage, COLOR_BGR2YCrCb);

			//3. Extract the skin color
			inRange(yCbCrImage, cv::Scalar(0, 138, 98), cv::Scalar(255, 177, 122), yCbCrImage);

			//5. Use find contours function
			frame.copyTo(contourImg, yCbCrImage);
			findContours(yCbCrImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
			
			//6. parts of skin color
			vector<Rect> boundRect(contours.size());
			for (int i = 0; i < contours.size(); i++){
				double n = contourArea(contours[i]);
				//4. different threshold
				if (n > threshold){
					drawContours(contourImg, contours, i, Scalar(0, 0, 255)); //draw contours on img
					boundRect[i] = boundingRect(Mat(contours[i]));
					
					rectangle(contourImg, boundRect[i].tl(), boundRect[i].br(), Scalar(255, 0, 0), 1, 8, 0);
					string text = "pixels : " + std::to_string(n);
					putText(contourImg, text, boundRect[i].tl(), FONT_HERSHEY_COMPLEX_SMALL, 1, cvScalar(0, 0, 255), 1, CV_AA);
				}
			}

			//7. show the video
			imshow("The Video", contourImg);

			if (counter == 100 || counter == 200){
				string outputFileName = "resultFrame_" + std::to_string(counter) + ".jpg";
				imshow(outputFileName, contourImg);
				imwrite(outputFileName, contourImg);
			}
			counter++;
			waitKey(3);
		}
	}
	waitKey(3);
	return 0;
}