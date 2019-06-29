#pragma once
#include <opencv2/opencv.hpp>
#include "VladimirKolmogorov/graph.h"
#include "gaussian_mixture_model.h"

using namespace cv;

class GrabCutGraph {
 private:
  int i_vertex_count;                          // ͼ�Ķ������
  int i_edge_count;                            // ͼ�ı߸���
  double d_beta;                               // �ڱ��з���beta����
  double d_gamma;                              // �ڱ��з���gamma����
  Mat img_src_copy;                            // ԭͼ - ����
  Mat mask_alpha_copy;                         // ��Ĥ - ����
  Graph<double, double, double> graph_to_cut;  // �����ͼ������double
                                               // Ϊ��������������������
  double d_energy;  // ͼ��󻨷ѵ�����
 public:
  GrabCutGraph(Mat img_src_arg, Mat mask_alpha_arg);
  void initBetaAndNode();                          // ����beta
  double calcBoundrayPenalty(Vec3b pix_diff_arg);  //�����ڱ�Ȩ��
  void assignBoundaryWeight();                     //�����ڱ�Ȩ��
  void assignRegionalWeight(GMM gmm_fgd_arg,
                            GMM gmm_bgd_arg);  // ���������Ȩ��
  void doMinimumCut();                         // ������С��
  Mat getMaskAlpha();  // ������С��֮�����������
};
