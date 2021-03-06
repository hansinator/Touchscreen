/*
 * projector_calibrator.h
 *
 *  Created on: May 9, 2012
 *      Author: Nikolas Engelhard
 */

#ifndef PROJECTOR_CALIBRATOR_H_
#define PROJECTOR_CALIBRATOR_H_

#include "type_definitions.h"
#include "calibration.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <pcl/common/transform.h>

typedef cv::Rect_<float> cv_RectF;


class Projector_Calibrator {

 // number of inner corners of the board
 cv::Size C_checkboard_size;

 cv::Size C_proj_size; // size of the projector image in pixels



 // trafo cloud s.t. checkerboard is z=0,  middle of board at x=y=0
 // the first trafo is stored and used for all following frames
 Eigen::Affine3f kinect_trafo;
 bool kinect_trafo_valid;

 cv::Mat input_image; // rgb image of kinect
 cv::Mat gray; // gray version of kinect image

 // image on kinect image showing the area where the first checkerboard was found in white
 // its 8UC1, with board: 255, rest: 0
 cv::Mat mask;



 // point cloud from kinect (still in kinect frame)
 Cloud input_cloud;


 // tilt of kinect (rotation around optical axis)
 float kinect_tilt_angle_deg;
 bool kinect_orientation_valid;

 // pixel coordinates of the detected checkerboard corners
 std::vector<cv::Point2f> corners;

 // remove all point from the input cloud where mask!=255
 void applyMaskOnInputCloud(Cloud& out);

 // fit a plane into the pointcloud
 float fitPlaneToCloud(const Cloud& cloud, Eigen::Vector4f& model);


 // draw a checkerboard with given number of internal corners on the image and store the corners
 void drawCheckerboard(cv::Mat& img, const cv::Size size, std::vector<cv::Point2f>& corners_2d);


 // Position of internal checkerboard corners
 std:: vector<cv::Point2f> projector_corners;


 std::string hom_cv_filename, hom_svd_filename, proj_matrix_filename, kinect_trafo_filename;

 bool saveMat(const std::string name, const std::string filename, const cv::Mat& mat);
 bool loadMat(const std::string name, const std::string filename, cv::Mat& mat);



public:
 // The projection matrix of the projector and the homographies computed via OpenCV and SVD
 cv::Mat  hom_CV, hom_SVD;



 // list of 3d-observations (in the wall-frame) of the checkerboard corners
 // length is a multiple of the number of checkerboardcorners
 Cloud observations_3d;

 void getCheckerboardArea(std::vector<cv::Point2i>& pts);


 void publish3DPoints();


 // should be private
 // point cloud in wall-frame
 Cloud cloud_moved;
 cv::Mat proj_Matrix;
 // image to be shown by the projector
 cv::Mat projector_image;



 // a simple image for debugging
 cv::Mat test_img;



 // apply on image
 cv::Mat warp_matrix;

 bool findOptimalProjectionArea(float ratio, cv_RectF& rect);


 bool projMatorHomSet(){return projMatrixSet() ||   homOpenCVSet() || homSVDSet();}


 Cloud* getTransformedCloud(){return &cloud_moved;}

 void initFromFile();

 bool projMatrixSet(){ return proj_Matrix.cols > 0;}
 bool homOpenCVSet(){ return hom_CV.cols > 0;}
 bool homSVDSet(){ return hom_SVD.cols > 0;}
 bool warpMatrixSet(){ return warp_matrix.cols > 0;}
 //   if (calibrator.imageProjectionSet()){
 //#ifdef SHOW_TEST_IMAGE
 //    calibrator.showUnWarpedImage(calibrator.test_img);
 //#else
 //    system("xwd -root | convert - /tmp/screenshot.jpg");
 //    cv::Mat screen = cv::imread("/tmp/screenshot.jpg");
 //    cv::Mat primary_screen = screen(cv::Range(0,mainScreenSize.height), cv::Range(0,mainScreenSize.width));
 //    calibrator.showUnWarpedImage(primary_screen);
 //
 //#endif
 //   }

 bool setupImageProjection(const cv_RectF& wall_area, const cv::Size& img_size);

 bool setupImageProjection(float width_m, float height_m, float off_x_m, float off_y_m, const cv::Size& img_size);
 bool setupImageProjection(float width_m, float off_x_m, float off_y_m, const cv::Size& img_size);

 bool imageProjectionSet() { return warp_matrix.cols > 0; }

 // save 3d positions (in wall-frame) of the last checkerboard detection
 bool storeCurrent3DObservations();

 bool isKinectOrientationSet(){return kinect_orientation_valid;}
 void setKinectOrientation(float angle_deg){kinect_tilt_angle_deg = angle_deg;kinect_orientation_valid = true;}

 // compute the transformation that transforms the point cloud from the kinect-frame to the wall frame
 void computeKinectTransformation();

 bool isKinectTrafoSet(){return kinect_trafo_valid;}

 bool mask_valid() {return mask.cols > 0;}

 // calls the openCV checkerboard detector and stores the result internally
 bool findCheckerboardCorners();

 // create the mask on the kinect image that shows the region where the checkerboard was detected
 void createMaskFromDetections();

 void setInputImage(cv::Mat& image){input_image = image; corners.clear();}
 void setInputCloud(Cloud& cloud);



 void showUnWarpedImage(const cv::Mat& img);

 void showUnWarpedImage(){
  assert(test_img.data);
  showUnWarpedImage(test_img);
 }


 cv::Mat* getTestImg(){return &test_img;}



 // pixel coordinates of checkerboard corners
 void computeProjectionMatrix();
 void computeHomography_OPENCV();
 void computeHomography_SVD();

 void showFullscreenCheckerboard();


 Cloud visualizePointCloud();


 Projector_Calibrator(){
  kinect_orientation_valid = false;
  kinect_trafo_valid = false;
  C_checkboard_size = cv::Size(10,6);

  C_proj_size = cv::Size(1024,768);


  projector_image = cv::Mat(C_proj_size, CV_8UC3);

  hom_cv_filename = "homography_opencv";
  hom_svd_filename = "homography_svd";
  proj_matrix_filename = "projection_matrix";
  kinect_trafo_filename = "kinect_trafo";

  test_img = cv::imread("/usr/gast/engelhan/ros/Touchscreen/imgs/Testbild.png");

  if (!test_img.data){
   ROS_ERROR("Could not open test image!");
  }

  // creating fullscreen image (old syntax)
  cvNamedWindow("fullscreen_ipl",0);
  cvMoveWindow("fullscreen_ipl", 2000, 100);
  cvSetWindowProperty("fullscreen_ipl", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

  showFullscreenCheckerboard();

 }



};




#endif /* PROJECTOR_CALIBRATOR_H_ */
