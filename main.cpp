#include <opencv2/opencv.hpp>
#include <iostream> // Wir nehmen kurz iostream statt print, das ist stabiler
#include <cstdlib>  // Für _putenv

int main() {

    cv::Mat img = cv::Mat::zeros(800, 800, CV_8UC3);

    cv::putText(img, "OpenCV laeuft!", 
                cv::Point(50, 400),
                cv::FONT_HERSHEY_DUPLEX,
                2.0,
                cv::Scalar(0, 255, 0),
                3);

    cv::namedWindow("Erfolg!", cv::WINDOW_AUTOSIZE);
    cv::imshow("Erfolg!", img);

    cv::waitKey(0);

    return 0;
}