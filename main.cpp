#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <vector>

const double pi = 3.14159265359;

struct coordinate {
    int x, y;
};

struct vector2d {
    double x, y, w;
};

struct rectangle {
    vector2d p1, p2, p3, p4;
};

rectangle rect = {};
std::vector<coordinate> coordinates = {};
coordinate lastClickedCoordinate = { x: 200, y: 200 }; // Default value if no point is clicked in the images

cv::Mat image(500, 500, CV_8UC3, cv::Scalar(0, 0, 0));

std::vector<std::vector<double>> createIdentityMatrix(int size) { 
    std::vector<std::vector<double>> matrix(size, std::vector<double>(size, 0));
    for (int i = 0; i < size; i++) {
        matrix[i][i] = 1;
    }

    return matrix; 
}

vector2d multiplicaVetor(vector2d v, std::vector<std::vector<double>> matriz) {
    return { 
        x: v.x * matriz[0][0] + v.y * matriz[0][1] + v.w * matriz[0][2], 
        y: v.x * matriz[1][0] + v.y * matriz[1][1] + v.w * matriz[1][2], 
        w: v.x * matriz[2][0] + v.y * matriz[2][1] + v.w * matriz[2][2]
    };
}

std::vector<coordinate> bresenhamLine(int x, int y, int x2, int y2) {
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

    std::vector<coordinate> coordinates = {};
    int numerator = longest >> 1;
    for (int i = 0; i <= longest; i++) {
        coordinates.push_back({ x, y });
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

    return coordinates;
}

void setTranslate(std::vector<std::vector<double>> &matriz, double a, double b) {
    matriz[0][0] = 1;
    matriz[0][1] = 0;
    matriz[0][2] = a;
    
    matriz[1][0] = 0;
    matriz[1][1] = 1;
    matriz[1][2] = b;
    
    matriz[2][0] = 0;
    matriz[2][1] = 0;
    matriz[2][2] = 1;
}

void rotateRectangle(std::vector<std::vector<double>> &matriz, float degree, coordinate pivot) {
    double rad = (degree * (pi / 180));
    matriz[0][0] = cos(rad);
    matriz[0][1] = -sin(rad);
    matriz[0][2] = (pivot.x * (1 - cos(rad)) + pivot.y * sin(rad));
    
    matriz[1][0] = sin(rad);
    matriz[1][1] = cos(rad);
    matriz[1][2] = (pivot.y * (1 - cos(rad)) - pivot.x * sin(rad));
    
    matriz[2][0] = 0;
    matriz[2][1] = 0;
    matriz[2][2] = 1;
}

void paintCoordinates(cv::Mat &image, std::vector<coordinate> coordinates, int r = 255, int g = 255, int b = 255) {
    for (coordinate c : coordinates) {
        image.at<cv::Vec3b>(c.y, c.x) = cv::Vec3b(r,g,b);
    }
}

void mouseHandler(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        std::cout << "Clicked (" << x << ", " << y << ")" << std::endl;

        lastClickedCoordinate = { x, y };
        coordinate coord = { x, y };
        coordinates.push_back(coord);

        int numCoordinates = coordinates.size();
        if (numCoordinates % 2 == 0) {
            std::cout << "Gonna create line between" << std::endl;
            std::cout << "x: " << (coordinates[numCoordinates -1].x) << " y: " << coordinates[numCoordinates -1].y << std::endl;
            std::cout << "x: " << (coordinates[numCoordinates -2].x) << " y: " << coordinates[numCoordinates -2].y << std::endl;
            cv::Point p1(coordinates[numCoordinates -1].x, coordinates[numCoordinates -1].y);
            cv::Point p2(coordinates[numCoordinates -2].x, coordinates[numCoordinates -2].y);
            paintCoordinates(image, bresenhamLine(p1.x, p1.y, p2.x, p2.y));
            cv::imshow("My Window", image);
        }
    }
}

void drawRectangle(rectangle rect) {
    image = (500, 500, CV_8UC3, cv::Scalar(0, 0, 0)); // Clear image i.e. sets to black again.
    paintCoordinates(image, bresenhamLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y));
    paintCoordinates(image, bresenhamLine(rect.p2.x, rect.p2.y, rect.p3.x, rect.p3.y));
    paintCoordinates(image, bresenhamLine(rect.p3.x, rect.p3.y, rect.p4.x, rect.p4.y));
    paintCoordinates(image, bresenhamLine(rect.p4.x, rect.p4.y, rect.p1.x, rect.p1.y));
    cv::imshow("My Window", image);
}

int waitAndExecutePressedKeyAction() {
    char pressedKey = (char) cv::waitKey(0);
    std::cout << "Key pressed: " << pressedKey << std::endl;
    std::vector<std::vector<double>> identity = createIdentityMatrix(3);
    if (pressedKey == 'c') {
        return 1;
    }

    if (pressedKey == 'Q') { // left
        setTranslate(identity, -10, 0);
    }

    if (pressedKey == 'R') { // up
        setTranslate(identity, 0, -10);
    }

    if (pressedKey == 'S') { // right
        setTranslate(identity, 10, 0);
    }

    if (pressedKey == 'T') { // down
        setTranslate(identity, 0, 10);
    }

    if (pressedKey == 'e') {
        rotateRectangle(identity, 15, lastClickedCoordinate);
    }

    if (pressedKey == 'q') {
        rotateRectangle(identity, -15, lastClickedCoordinate);
    }

    rect.p1 = multiplicaVetor(rect.p1, identity);
    rect.p2 = multiplicaVetor(rect.p2, identity);
    rect.p3 = multiplicaVetor(rect.p3, identity);
    rect.p4 = multiplicaVetor(rect.p4, identity);

    drawRectangle(rect);

    return waitAndExecutePressedKeyAction();
}

int main() {
    rect = {
        { 200, 200, 1 },
        { 300, 200, 1 },
        { 300, 300, 1 },
        { 200, 300, 1 }
    };

    cv::namedWindow("My Window");
    cv::setMouseCallback("My Window", mouseHandler);

    drawRectangle(rect);

    waitAndExecutePressedKeyAction();
  
    return 0;
}