#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include "gaussian_mixture_model.h"
#include "grabcut_graph.h"

using namespace cv;
using namespace std;

class GrabCutMethod {
 private:
  int i_fgd_comp;              // ǰ����˹��ָ���
  int i_bgd_comp;              // ������ָ���
  Mat img_src;                 // ԭͼ
  Mat mask_alpha;              // ��ǩ����
  vector<Vec3b> vec_pixs_fgd;  // ǰ�����أ�������
  vector<Vec3b> vec_pixs_bgd;  // ������������
  Mat mat_pixs_fgd;            // ǰ�����أ����󣩣�n��x3��
  Mat mat_pixs_bgd;            // ����������
  Mat mat_pixs_fgd_k;          // ǰ�����ظ�˹��������n��x1��
  Mat mat_pixs_bgd_k;          // ��������
  GMM gmm_fgd;                 // ǰ����˹ģ��
  GMM gmm_bgd;                 // ����ģ��
 public:
  GrabCutMethod(int i_fgd_comp_arg, int i_fgd_comp_dim_arg, int i_bgd_comp_arg,
                int i_bgd_comp_dim_arg);
  void initPixelsVec(Mat img, Mat mask);  // ��ʼ�����ص�������
  void clusterPixels();                   // ���ؾ���
  void fitTwoGMMs();                      // ǰ/����GMM���
  void updateTwoIndexMat();               // ���¸�˹������
  GrabCutGraph initGraph();               // ��ʼ����ͼ
  void updateMaskAlpha(GrabCutGraph &gc_graph_arg);  // ����mask_alpha
  Mat getMaskAlpha();  // ����mask_alpha��gc_client
};
