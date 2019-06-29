#include "grabcut_graph.h"

GrabCutGraph::GrabCutGraph(Mat img_src_arg, Mat mask_alpha_arg)
    : i_vertex_count{img_src_arg.rows * img_src_arg.cols},
      i_edge_count{2 * (4 * img_src_arg.cols * img_src_arg.rows -
                        3 * (img_src_arg.cols + img_src_arg.rows) + 2)},
      d_beta{0.0},
      d_gamma{50.0},
      graph_to_cut{i_vertex_count, i_edge_count},
      d_energy{-1.0} {
  // ����ԭͼ����Ĥ
  img_src_copy = img_src_arg;
  mask_alpha_copy = mask_alpha_arg;
  // ����beta����ӽڵ�
  initBetaAndNode();
  // �����ڱ�Ȩ��
  assignBoundaryWeight();
}

void GrabCutGraph::initBetaAndNode() {
  // �ĸ�����˵����
  //
  //   upleft   up  upright
  //          \ | /
  //   left ���� NODE

  double d_square_sum = 0.0;
  for (int r = 0; r < img_src_copy.rows; r++) {
    for (int c = 0; c < img_src_copy.cols; c++) {
      // ����beta......
      Vec3b pix = img_src_copy.at<Vec3b>(r, c);
      if (c >= 1) {  // left
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r, c - 1);
        d_square_sum += pix_diff.dot(pix_diff);
      }
      if (r >= 1 && c >= 1) {  // upleft
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c - 1);
        d_square_sum += pix_diff.dot(pix_diff);
      }
      if (r >= 1) {  // up
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c);
        d_square_sum += pix_diff.dot(pix_diff);
      }
      if (r >= 1 && c <= img_src_copy.cols - 2) {  // upright
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c + 1);
        d_square_sum += pix_diff.dot(pix_diff);
      }
      // ͼ��ӽڵ�
      graph_to_cut.add_node();
    }
  }
  d_square_sum /= (double)i_edge_count;
  d_beta = (double)1 / (2 * d_square_sum);
}

double GrabCutGraph::calcBoundrayPenalty(Vec3b pix_diff_arg) {
  return d_gamma * exp(-d_beta * pix_diff_arg.dot(pix_diff_arg));
}

void GrabCutGraph::assignBoundaryWeight() {
  for (int r = 0; r < img_src_copy.rows; r++) {
    for (int c = 0; c < img_src_copy.cols; c++) {
      Vec3b pix = img_src_copy.at<Vec3b>(r, c);
      int i_vertex_id = r * img_src_copy.cols + c;
      double d_weight_edge;

      if (c >= 1) {  // left
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r, c - 1);
        d_weight_edge = calcBoundrayPenalty(pix_diff);
        graph_to_cut.add_edge(i_vertex_id, i_vertex_id - 1, d_weight_edge,
                              d_weight_edge);
      }
      if (r >= 1 && c >= 1) {  // upleft
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c - 1);
        d_weight_edge = calcBoundrayPenalty(pix_diff);
        graph_to_cut.add_edge(i_vertex_id, i_vertex_id - 1, d_weight_edge,
                              d_weight_edge);
      }
      if (r >= 1) {  // up
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c);
        d_weight_edge = calcBoundrayPenalty(pix_diff);
        graph_to_cut.add_edge(i_vertex_id, i_vertex_id - img_src_copy.cols,
                              d_weight_edge, d_weight_edge);
      }
      if (r >= 1 && c <= img_src_copy.cols - 2) {  // upright
        Vec3b pix_diff = pix - img_src_copy.at<Vec3b>(r - 1, c + 1);
        d_weight_edge = calcBoundrayPenalty(pix_diff);
        graph_to_cut.add_edge(i_vertex_id, i_vertex_id - img_src_copy.cols + 1,
                              d_weight_edge, d_weight_edge);
      }
    }
  }
}

void GrabCutGraph::assignRegionalWeight(GMM gmm_fgd_arg, GMM gmm_bgd_arg) {
  graph_to_cut.setFlow(0.0);      // flow����
  double d_lambda = 9 * d_gamma;  // ȷ����ǰ/���������з�ֵ
  for (int r = 0; r < img_src_copy.rows; r++) {
    for (int c = 0; c < img_src_copy.cols; c++) {
      Vec3b pix = img_src_copy.at<Vec3b>(r, c);
      int i_vertex_id = r * img_src_copy.cols + c;
      double d_weight_source, d_weight_sink;
      // ��ʼ��Դ��Ȩ��
      graph_to_cut.set_trcap(i_vertex_id, 0.0);
      // ���㲢����Դ��Ȩ��
      if (mask_alpha_copy.at<uchar>(r, c) == GC_PR_BGD ||
          mask_alpha_copy.at<uchar>(r, c) == GC_PR_FGD) {
        d_weight_source = -log(gmm_bgd_arg.sumProbability(pix));
        // ����ǰ���ĸ���Խ�ߣ��ۺ����÷ֱ�����ԽС��������ǰ/�����Ե�
        d_weight_sink = -log(gmm_fgd_arg.sumProbability(pix));
      } else if (mask_alpha_copy.at<uchar>(r, c) == GC_BGD) {
        d_weight_source = 0.0;
        d_weight_sink = d_lambda;
      } else if (mask_alpha_copy.at<uchar>(r, c) == GC_FGD) {
        d_weight_source = d_lambda;
        d_weight_sink = 0.0;
      }
      graph_to_cut.add_tweights(i_vertex_id, d_weight_source, d_weight_sink);
    }
  }
}

void GrabCutGraph::doMinimumCut() {
  // ͨ�������ʵ����С��
  d_energy = graph_to_cut.maxflow();
  // ע�⣬Ҫ��ʼ����mask_alpha��
  for (int r = 0; r < img_src_copy.rows; r++) {
    for (int c = 0; c < img_src_copy.cols; c++) {
      int i_id_temp = r * img_src_copy.cols + c;
      if (mask_alpha_copy.at<uchar>(r, c) == GC_PR_BGD ||
          mask_alpha_copy.at<uchar>(r, c) == GC_PR_FGD) {
        if (graph_to_cut.what_segment(i_id_temp) ==
            Graph<double, double, double>::SOURCE) {
          mask_alpha_copy.at<uchar>(r, c) = GC_PR_FGD;
        } else {
          mask_alpha_copy.at<uchar>(r, c) = GC_PR_BGD;
        }
      }
    }
  }
  cout << " ** energy **: " << d_energy << endl;
}

Mat GrabCutGraph::getMaskAlpha() { return mask_alpha_copy; }
