#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "gaussian_mixture_model.h"

using namespace cv;
using namespace std;

class GrabCutMethod {
 private:
  int i_fgd_comp;             // ǰ����˹��ָ���
  int i_bgd_comp;             // ������ָ���
  Mat img_src;                // ԭͼ
  Mat mask_alpha;             // ��ǩ����
  vector<Vec3b> vec_pixs_fgd; // ǰ����������
  vector<Vec3b> vec_pixs_bgd; // ��������
  Mat mat_pixs_fgd;           // ǰ�����ؾ���
  Mat mat_pixs_bgd;           // ��������
  Mat mat_pixs_fgd_k;         // ǰ������ģ������
  Mat mat_pixs_bgd_k;         // ��������
  GMM gmm_fgd;                // ǰ����˹ģ��
  GMM gmm_bgd;                // ����ģ��

 public:
  GrabCutMethod(int i_fgd_comp_arg, int i_fgd_comp_dim_arg, int i_bgd_comp_arg,
                int i_bgd_comp_dim_arg);
  void initPixelsVec(Mat img, Mat mask);
  void clusterPixels();
  void fitGMMs();
};
