#include "grabcut_client.h"

GrabCutClient::GrabCutClient(String file_path)
    : win_src{"Image SRC"}, win_dst{"Image DST"}, gc_method{5, 3, 5, 3} {
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
    showDstImage();
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
  (mask_alpha(rect_fgd)).setTo(Scalar(GC_PR_FGD));

  /*����дgrabcut����ִ�еĴ���*/
  //......
  gc_method.initPixelsVec(img_src, mask_alpha);
  gc_method.clusterPixels();
  gc_method.fitTwoGMMs();
  gc_method.updateTwoIndexMat();
  gc_method.fitTwoGMMs();
}

void GrabCutClient::showDstImage() {
  Mat mask_fgd;  // ��ǰ�������Ĥ
  compare(mask_alpha, GC_PR_FGD, mask_fgd,
          CMP_EQ);                    // ����ʾ�ж�ΪPR_FGD������
  img_src.copyTo(img_dst, mask_fgd);  // ����mask_alphaָʾ����copy
  namedWindow(win_dst, WINDOW_AUTOSIZE);
  imshow(win_dst, img_dst);
}
