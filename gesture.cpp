
 
#include &amp;lt;iostream&amp;gt;
#include &amp;lt;wiringPi.h&amp;gt;
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace std;
using namespace cv;
void DetectGesture(cv::Mat frame);
 
int controlPin1 = 28; // Pin 38 in Rpi3
int controlPin2 =29; // Pin 40 in Rpi3
 
int main()
{
 
if(wiringPiSetup() == -1)
{
std::cout&amp;lt;&amp;lt;"Unable to setup GPIOs access \n";
return 1;
}
pinMode(controlPin1, OUTPUT);
pinMode(controlPin2, OUTPUT);
 
cv::Mat img;
 
cv::VideoCapture capture(0);
{
while(capture.isOpened())
{
capture &amp;gt;&amp;gt; img;
 
if(!img.empty())
{
DetectGesture(img);
cv::waitKey(1);
}
 
if(i == 27)
break;
 
}
}
return 0;
}
void DetectGesture(cv::Mat frame)
{
cv::Mat img, binary;
cvtColor(frame, img, CV_BGR2GRAY);
 
//thresholding
cv::threshold(img, binary, 35, 255, cv::THRESH_BINARY_INV);
 
//Contour storage
vector&amp;lt;vector&amp;lt;Point&amp;gt;&amp;gt; Contours;
vector&amp;lt;Vec4i&amp;gt; hierarchy;
 
//convex Hull storage
vector&amp;lt;vector&amp;lt;Point&amp;gt;&amp;gt; cnvxHull(Contours.size());
vector&amp;lt;vector&amp;lt;int&amp;gt;&amp;gt; cnvxHullsI(Contours.size());
vector&amp;lt;vector&amp;lt;Vec4i&amp;gt;&amp;gt; defects(Contours.size());
 
//morphological transformations
erode(binary, binary, getStructuringElement(MORPH_RECT, Size(3, 3)));
erode(binary, binary, getStructuringElement(MORPH_RECT, Size(3, 3)));
 
dilate(binary, binary, getStructuringElement(MORPH_RECT, Size(8, 8)));
dilate(binary, binary, getStructuringElement(MORPH_RECT, Size(8, 8)));
 
//finding the contours required
findContours(binary, Contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE, Point(0, 0));
 
//finding the contour of largest area and storing its index
int biggestContour = 0;
int maxarea = 0;
 
for (int i = 0; i &amp;lt; Contours.size(); i++)
{
double currentArea = contourArea(Contours[i]);
if ( currentArea &amp;gt; maxarea)
{
maxarea = currentArea;
biggestContour = i;
}
}
for (int i = 0; i &amp;lt; Contours.size(); i++)
{
convexhull(Contours[i], cnvxHull[i], false);
convexhull(Contours[i], cnvxHullsI[i], false);
if(cnvxHullsI[i].size() &amp;gt; 3 )
{
convexityDefects(Contours[i], cnvxHullsI[i], defects[i]);
}
}
 
std::vector&amp;lt;cv::Point&amp;gt; peakCount;
 
if (maxarea &amp;gt; 100){
 
/// Draw convexity Defects
for(int j=0; j&amp;lt;defects[biggestContour].size(); ++j)
{
const Vec4i&amp;amp; v = defects[biggestContour][j];
float depth = v[3] / 256;
if (depth &amp;gt; 10) //
{
int startidx = v[0];
Point ptStart(Contours[biggestContour][startidx]);
peakCount.push_back(ptStart);
 
int endidx = v[1];
Point ptEnd(Contours[biggestContour][endidx]);
 
int faridx = v[2];
Point ptFar(Contours[biggestContour][faridx]);
 
line(frame, ptStart, ptEnd, Scalar(0, 255, 0), 5);
line(frame, ptStart, ptFar, Scalar(0, 255, 0), 5);
line(frame, ptEnd, ptFar, Scalar(0, 255, 0), 5);
circle(frame, ptFar, 4, Scalar(255, 0, 0), 5);
circle(frame, ptStart, 6, Scalar(0, 0, 255), 5);
 
}
}
}
 
if(peakCount.size() &amp;gt; 4)
{
digitalWrite(controlPin1, 1);
 
}
else if(peakCount.size() == 4)
{
digitalWrite(controlPin2, 1);
digitalWrite(controlPin1, 0);
}
else
{
digitalWrite(controlPin1, 0);
digitalWrite(controlPin2, 0);
}
 
cv::imshow("Live", frame);
 
}