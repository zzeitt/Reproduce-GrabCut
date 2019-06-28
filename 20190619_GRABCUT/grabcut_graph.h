#pragma once
#include <opencv2/opencv.hpp>
#include "VladimirKolmogorov/graph.h"
#include "gaussian_mixture_model.h"

using namespace cv;

class GrabCutGraph {
 private:
  int i_vertex_count;                          // ͼ�Ķ������
  int i_edge_count;                            // ͼ�ı߸���
  double d_beta;                               // �߽��з���beta����
  Mat img_src_copy;                            // ԭͼ - ����
  Mat mask_alpha_copy;                         // ��Ĥ - ����
  Mat mat_penalty_r_fgd;                       // �����з���ǰ��
  Mat mat_penalty_r_bgd;                       // �����з�������
  Mat mat_penalty_b_left;                      // �߽��з�����
  Mat mat_penalty_b_up;                        // �߽��з�����
  Mat mat_penalty_b_upleft;                    // �߽��з�������
  Mat mat_penalty_b_upright;                   // �߽��з�������
  Graph<double, double, double> graph_to_cut;  // �����ͼ������double
                                               // Ϊ��������������������
  double d_energy;  // ͼ��󻨷ѵ�����
 public:
  GrabCutGraph(Mat img_src_arg, Mat mask_alpha_arg);
  void calcBeta();                                    // ����beta
  void calcWeight(GMM gmm_fgd_arg, GMM gmm_bgd_arg);  // �����ߵ�Ȩ��
  void assignWeight();                                // ����Ȩ�ص�ͼ��
  void doMinimumCut();                                // ������С��
  Mat getMaskAlpha();  // ������С��֮�����������
};
