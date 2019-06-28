#pragma once
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
  Mat img_dst;              // ����ͼƬ
  const String win_src;     // ԭͼ����
  const String win_dst;     // ����ͼ����
  GrabCutMethod gc_method;  // �㷨ִ����
  int i_iterate;            // ��������
  int i_time_total;         // ������ʱ��
 public:
  GrabCutClient(String file_path, int i_iterate_arg);
  void redrawSrcImage();  // �������
  void onMouseMember(int event, int x, int y, int flags, void* userdata);
  static void onMouse(int event, int x, int y, int flags, void* userdata);
  void iterateLabelMask();  // ǰ����Ĥ����
  void showDstImage();      // �����ʾ
};
