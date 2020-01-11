#include <opencv2/opencv.hpp>

#pragma comment(lib, "opencv_world341.lib")

using namespace std;
using namespace cv;

#define FILTER_FRAME 3

int main() {

	int current_frame = 0;

	// ���͉f��
	VideoCapture src("input.avi");
	int src_fps = src.get(CV_CAP_PROP_FPS);
	Size src_size(src.get(CV_CAP_PROP_FRAME_WIDTH), src.get(CV_CAP_PROP_FRAME_HEIGHT));

	// �o�͉f��
	VideoWriter dst("output.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), src_fps, src_size, true);

	// �L���[
	vector<Mat> src_queue;
	vector<Mat> src_queue_gray; // �O���[�X�P�[����

	// ���ԍ����摜���擾���ăL���[�Ɋi�[
	for (int i = 0; i < FILTER_FRAME; i++) {
		Mat temp, temp_gray;
		src >> temp;
		cvtColor(temp, temp_gray, COLOR_BGR2GRAY);
		src_queue.push_back(temp);
		src_queue_gray.push_back(temp_gray);
	}

	// ����̐���
	while (true) {
		// �t���[���̓ǂݍ���
		Mat current;
		src >> current;
		current_frame++;

		// �I������
		if (current.empty()) {
			break;
		}

		{
			// �L���[�ɓ����
			Mat temp_gray;
			cvtColor(current, temp_gray, COLOR_BGR2GRAY);
			src_queue.push_back(current);
			src_queue_gray.push_back(temp_gray);

			// �擪�v�f�͍폜
			src_queue.erase(src_queue.begin());
			src_queue_gray.erase(src_queue_gray.begin());
		}

		{
			Mat new_frame(src_size, CV_8UC3);

			// �S�Ẳ�f�ɑ΂���
			for (int y = 0; y < src_size.height; y++) {
				for (int x = 0; x < src_size.width; x++) {

					// �ł��Â����̂̃C���f�b�N�X�������o��
					int index_most_black = 0;
					for (int i = 1; i < FILTER_FRAME; i++) {
						if (src_queue_gray[i].at<unsigned char>(y, x) < src_queue_gray[index_most_black].at<unsigned char>(y, x)) {
							index_most_black = i;
						}
					}

					// �s�N�Z���u������
					for (int i = 0; i < 3; i++) {
						new_frame.at<Vec3b>(y, x)[i] = src_queue[index_most_black].at<Vec3b>(y, x)[i];
					}
					
				}
			}

			dst << new_frame;
			cout << current_frame << ", ";
		}

	}

	// �o�͓���̍Đ�
	{
		VideoCapture result("output.avi");
		int fps = src.get(CV_CAP_PROP_FPS);
		Mat current;

		while (true) {
			// �t���[���̓ǂݍ���
			result >> current;

			// �I������
			if (current.empty()) {
				break;
			}

			// �\��
			imshow("�o��", current);
			waitKey(1000 / fps);
		}
	}

	waitKey();

	return 0;
}