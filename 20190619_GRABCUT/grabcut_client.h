#pragma once
#pragma warning(disable : 4996)
#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "grabcut_method.h"

using namespace cv;
using namespace std;

class GrabCutClient {
 private:
  Mat img_src;              // ԭͼƬ
  Mat img_src_2;            // �����û�������ԭͼ����
  Rect rect_fgd;            // ǰ�����ο�
  Mat mask_alpha;           // ����������Ĥ
  Mat mask_fgd;             // ǰ��ר����Ĥ
  Mat img_dst;              // ����ͼƬ
  bool b_opencv;            // �Ƿ�ִ��OpenCV�����㷨
  int i_iterate;            // ��������
  int i_comp;               // GMM�и�˹��ָ���
  const String win_src;     // ԭͼ����
  const String win_dst;     // ����ͼ����
  GrabCutMethod gc_method;  // �㷨ִ����
 public:
  GrabCutClient(String file_path, bool b_opencv_arg, int i_comp_arg,
                int i_iterate_arg);
  void redrawSrcImage();  // �������
  void onMouseMember(int event, int x, int y, int flags, void* userdata);
  static void onMouse(int event, int x, int y, int flags, void* userdata);
  void iterateLabelMask();  // ǰ����Ĥ����
  void showDstImage();      // �����ʾ
  void saveTwoImages(string s_date, string s_time_elapse);  // ����ͼƬ
};
