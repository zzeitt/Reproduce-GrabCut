#include "grabcut_client.h"
#include <chrono>

using namespace std::chrono;

GrabCutClient::GrabCutClient(String file_path, int i_iterate_arg)
    : win_src{"Image SRC"},
      win_dst{"Image DST"},
      gc_method{5, 3, 5, 3},
      i_iterate{i_iterate_arg},
      i_time_total{0} {
  /*******************��ʼ��*******************/
  try {
    img_src = imread(file_path);
    if (img_src.empty()) {
      throw "Empty image!";
    }
  } catch (char* str) {
    cout << "��Error��: " << str << endl;
    Mat img_exception(200, 300, CV_8UC3, Scalar(127, 127, 127));
    img_src = img_exception;
  } catch (...) {
    cout << "��Error��: Image load failed!" << endl;
    Mat img_exception(200, 300, CV_8UC3, Scalar(127, 127, 127));
    img_src = img_exception;
  }
  mask_alpha = Mat::zeros(img_src.size(), CV_8UC1);  // ��ʼ��ΪȫGC_BGD=0
  Mat img_dst_temp(img_src.size(), CV_8UC3, Scalar(96, 96, 96));
  img_dst = img_dst_temp;  // ��ʼ�����ͼ
  /*******************�û���ѡ*******************/
  namedWindow(win_src, WINDOW_AUTOSIZE);
  imshow(win_src, img_src);
  setMouseCallback(win_src, onMouse, this);
  /*******************������Ӧ*******************/
  char c = waitKey(0);
  if (c == 'g') {
    iterateLabelMask();
    // showDstImage();
  }
  waitKey(0);
  destroyAllWindows();
}

void GrabCutClient::redrawSrcImage() {
  img_src.copyTo(img_src_2);
  rectangle(img_src_2, rect_fgd, Scalar(0, 0, 255), 2);
  imshow(win_src, img_src_2);
}

void GrabCutClient::onMouse(int event, int x, int y, int flags,
                            void* userdata) {
  GrabCutClient* temp = reinterpret_cast<GrabCutClient*>(userdata);
  temp->onMouseMember(event, x, y, flags, userdata);
}

void GrabCutClient::onMouseMember(int event, int x, int y, int flags,
                                  void* userdata) {
  switch (event) {
    case EVENT_LBUTTONDOWN:
      rect_fgd.x = x;
      rect_fgd.y = y;
      rect_fgd.width = 1;
      rect_fgd.height = 1;
      break;
    case EVENT_MOUSEMOVE:
      if (flags && EVENT_FLAG_LBUTTON) {
        rect_fgd = Rect(Point(rect_fgd.x, rect_fgd.y), Point(x, y));
        redrawSrcImage();
      }
      break;
    case EVENT_LBUTTONUP:
      if (rect_fgd.width > 1 && rect_fgd.height > 1) {
        redrawSrcImage();
      }
      break;
    default:
      break;
  }
}

void GrabCutClient::iterateLabelMask() {
  // �û���ѡ�ķ�Χ��ʼ��ΪGC_PR_FGD�����ź����������㣬
  // ���ܻ��в���������ΪGC_PR_BGD
  (mask_alpha(rect_fgd)).setTo(Scalar(GC_PR_FGD));

  /*����дgrabcut����ִ�еĴ���*/
  // ��ʼ��
  gc_method.initPixelsVec(img_src, mask_alpha);
  gc_method.clusterPixels();
  cout << endl << " ===================>>>����ʼ����ɡ�" << endl;
  // ������......
  for (int i = 0; i < i_iterate; i++) {
    cout << endl
         << " ===================>>>���ڣ�" << (i + 1) << "���ε�����ʼ��"
         << endl;
    auto start = high_resolution_clock::now();  // ��ʱ��ʼ
    /////////// ��ʱ���� ///////////
    gc_method.fitTwoGMMs();
    gc_method.updateMaskAlpha();
    mask_alpha = gc_method.getMaskAlpha();
    ///////////////////////////////
    auto stop = high_resolution_clock::now();  //��ʱ����
    auto duration = duration_cast<microseconds>(stop - start);
    int i_time_du = (int)duration.count() / 1000000;
    i_time_total += i_time_du;
    cout << " ===================>>>���ڣ�" << (i + 1) << "���ε�����ʱ "
         << i_time_du << " �롿" << endl
         << endl;
    showDstImage();
  }
  cout << endl
       << " ===================>>>��������ʱ" << i_time_total << "�롿" << endl;
}

void GrabCutClient::showDstImage() {
  Mat mask_fgd;
  // ����ʾ�ж�ΪGC_PR_FGD������
  compare(mask_alpha, GC_PR_FGD, mask_fgd, CMP_EQ);
  // ����mask_alphaָʾ����copy
  img_src.copyTo(img_dst, mask_fgd);
  namedWindow(win_dst, WINDOW_AUTOSIZE);
  waitKey(1);
  imshow(win_dst, img_dst);
}
