#ifndef PARAMTERS_USE_UI
#define PARAMTERS_USE_UI
#endif

#include "Parameters.hpp"
#include "Types.hpp"
#include <vector>
#include <string>
#include <type_traits>

using namespace Parameters;

#ifdef OPENCV_FOUND
#include <opencv2/core.hpp>

/*static TypedParameter<cv::Point> point_("instance");
static TypedParameter<cv::Point2d> point2d_("instance");
static TypedParameter<cv::Point2f> point2f_("instance");
//static TypedParameter<cv::Vec2b> vec2b_("instance");
static TypedParameter<cv::Vec2d> vec2d_("instance");
static TypedParameter<cv::Vec2f> vec2f_("instance");
static TypedParameter<cv::Vec2i> vec2i_("instance");
//static TypedParameter<cv::Vec3b> vec3b_("instance");
static TypedParameter<cv::Vec3d> vec3d_("instance");
static TypedParameter<cv::Vec3f> vec3f_("instance");
static TypedParameter<cv::Vec3i> vec3i_("instance");*/

static TypedParameter<std::vector<cv::Point>> vecpoint_("instance");
static TypedParameter<std::vector<cv::Point2d>> vecpoint2d_("instance");
static TypedParameter<std::vector<cv::Point2f>> vecpoint2f_("instance");
//static TypedParameter<std::vector<cv::Vec2b>> vecvec2b_("instance");
static TypedParameter<std::vector<cv::Vec2d>> vecvec2d_("instance");
static TypedParameter<std::vector<cv::Vec2f>> vecvec2f_("instance");
static TypedParameter<std::vector<cv::Vec2i>> vecvec2i_("instance");
static TypedParameter<std::vector<cv::Vec3b>> vecvec3b_("instance");
static TypedParameter<std::vector<cv::Vec3d>> vecvec3d_("instance");
static TypedParameter<std::vector<cv::Vec3f>> vecvec3f_("instance");
static TypedParameter<std::vector<cv::Vec3i>> vecvec3i_("instance");

#endif