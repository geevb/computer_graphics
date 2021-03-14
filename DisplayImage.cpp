#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <vector>

struct coordinate {
    int x;
    int y;
};
std::vector<coordinate> coordinates = {};

cv::Mat image(500, 500, CV_8UC3, cv::Scalar(0, 0, 0));

void bresenhamLine(int x, int y, int x2, int y2) {
    int w = x2 - x;
    int h = y2 - y;
    int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

    if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
    if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
    if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;

    int longest = abs(w);
    int shortest = abs(h);
    if (longest <= shortest) {
        longest = abs(h);
        shortest = abs(w);
        if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
        dx2 = 0;
    }

    int numerator = longest >> 1 ;
    for (int i = 0; i <= longest; i++) {
        image.at<cv::Vec3b>(y,x) = cv::Vec3b(255,255,255);
        numerator += shortest;
        if (numerator >= longest) {
            numerator -= longest;
            x += dx1;
            y += dy1;
        } else {
            x += dx2;
            y += dy2;
        }
    }

    cv::imshow("My Window", image);
}

void mouseHandler(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Clicked (" << x << ", " << y << ")" << std::endl;

        coordinate coord = { x, y };
        coordinates.push_back(coord);

        int numCoordinates = coordinates.size();
        if (numCoordinates % 2 == 0) {
            std::cout << "Gonna create line between" << std::endl;
            std::cout << "x: " << (coordinates[numCoordinates -1].x) << " y: " << coordinates[numCoordinates -1].y << std::endl;
            std::cout << "x: " << (coordinates[numCoordinates -2].x) << " y: " << coordinates[numCoordinates -2].y << std::endl;
            cv::Point p1(coordinates[numCoordinates -1].x, coordinates[numCoordinates -1].y);
            cv::Point p2(coordinates[numCoordinates -2].x, coordinates[numCoordinates -2].y);
            bresenhamLine(p1.x, p1.y, p2.x, p2.y);
        }
    }
}

int main() {
    cv::namedWindow("My Window");
    cv::setMouseCallback("My Window", mouseHandler);
    cv::imshow("My Window", image);
    cv::waitKey(0);
  
    return 0;
}