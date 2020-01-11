#include <opencv2/opencv.hpp>

#pragma comment(lib, "opencv_world341.lib")

using namespace std;
using namespace cv;

#define FILTER_FRAME 3

int main() {

	int current_frame = 0;

	// 入力映像
	VideoCapture src("input.avi");
	int src_fps = src.get(CV_CAP_PROP_FPS);
	Size src_size(src.get(CV_CAP_PROP_FRAME_WIDTH), src.get(CV_CAP_PROP_FRAME_HEIGHT));

	// 出力映像
	VideoWriter dst("output.avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), src_fps, src_size, true);

	// キュー
	vector<Mat> src_queue;
	vector<Mat> src_queue_gray; // グレースケール版

	// 時間差分画像を取得してキューに格納
	for (int i = 0; i < FILTER_FRAME; i++) {
		Mat temp, temp_gray;
		src >> temp;
		cvtColor(temp, temp_gray, COLOR_BGR2GRAY);
		src_queue.push_back(temp);
		src_queue_gray.push_back(temp_gray);
	}

	// 動画の生成
	while (true) {
		// フレームの読み込み
		Mat current;
		src >> current;
		current_frame++;

		// 終了条件
		if (current.empty()) {
			break;
		}

		{
			// キューに入れる
			Mat temp_gray;
			cvtColor(current, temp_gray, COLOR_BGR2GRAY);
			src_queue.push_back(current);
			src_queue_gray.push_back(temp_gray);

			// 先頭要素は削除
			src_queue.erase(src_queue.begin());
			src_queue_gray.erase(src_queue_gray.begin());
		}

		{
			Mat new_frame(src_size, CV_8UC3);

			// 全ての画素に対して
			for (int y = 0; y < src_size.height; y++) {
				for (int x = 0; x < src_size.width; x++) {

					// 最も暗いもののインデックスを見つけ出す
					int index_most_black = 0;
					for (int i = 1; i < FILTER_FRAME; i++) {
						if (src_queue_gray[i].at<unsigned char>(y, x) < src_queue_gray[index_most_black].at<unsigned char>(y, x)) {
							index_most_black = i;
						}
					}

					// ピクセル置き換え
					for (int i = 0; i < 3; i++) {
						new_frame.at<Vec3b>(y, x)[i] = src_queue[index_most_black].at<Vec3b>(y, x)[i];
					}
					
				}
			}

			dst << new_frame;
			cout << current_frame << ", ";
		}

	}

	// 出力動画の再生
	{
		VideoCapture result("output.avi");
		int fps = src.get(CV_CAP_PROP_FPS);
		Mat current;

		while (true) {
			// フレームの読み込み
			result >> current;

			// 終了条件
			if (current.empty()) {
				break;
			}

			// 表示
			imshow("出力", current);
			waitKey(1000 / fps);
		}
	}

	waitKey();

	return 0;
}