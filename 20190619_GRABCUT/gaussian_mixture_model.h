#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

/*******************��˹����*******************/
class Gau {
 private:
  int i_dim;                   // ά��
  Mat mat_mean;                // ��ֵ����1��xn��
  Mat mat_cov;                 // Э�������
  double d_weight;             // Ȩ��
  vector<Vec3b> vec_pixs_gau;  // �洢ÿ����˹������Ӧ��������
  Mat mat_pixs_gau;            // �洢Ϊ������ʽ��n��x3��
  int i_pix_count;             // ÿ�����������ظ���
 public:
  Gau(int dim);
  void addPixel(Vec3b pix);                 // Ϊ��˹���������
  void doComputation(int i_pix_gmm_count);  // ������ӵ����ؼ���13������
  double calcProbability(Vec3b pix);  // ���뵥�����أ������Ӧ����
};

/*******************��˹���ģ��*******************/
class GMM {
 private:
  int i_comp_count;          // ��ָ���
  int i_comp_dim;            // ���ά��
  vector<Gau> vec_comp_gau;  // �������
  int i_pix_gmm_count;       // ģ����ϵ������ظ���
 public:
  GMM(int n, int d);
  void fitGMM(Mat mat_k, vector<Vec3b> vec_pixs);  // ���������������������ģ��
  int findMostLikelyGau(Vec3b pix);  // ���뵥�����أ��ҵ��������ĸ�˹��
  double sumProbability(Vec3b pix);  // �ۼӵ��������ڸ���˹�����ĸ���ֵ
};
