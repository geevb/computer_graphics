#include <iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <vector>

const char* WINDOW_NAME = "ComputacaoGrafica";
const int IMAGE_HEIGHT = 1000;
const int IMAGE_WIDTH = 1000;
const double pi = 3.14159265359;

cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));

struct vector2d { double x, y, w; };
struct rectangle {
    vector2d p1, p2, p3, p4;
};
rectangle rect = {};

struct vector3d { double x, y, z, w; };

struct coordinate { int x, y; };
coordinate lastClickedCoordinate = { x: 200, y: 200 }; // Default value if no point is clicked in the images
std::vector<coordinate> coordinates = {};

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
        if (x < 0 || x >= IMAGE_WIDTH || y < 0 || y >= IMAGE_HEIGHT) {
			continue;
		}

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

void paintCoordinates(cv::Mat &image, std::vector<coordinate> coordinates, int r = 255, int g = 255, int b = 255) {
    for (coordinate c : coordinates) {
        image.at<cv::Vec3b>(c.y, c.x) = cv::Vec3b(r,g,b);
    }
}

std::vector<std::vector<double>> createIdentityMatrix(int size) { 
    std::vector<std::vector<double>> matrix(size, std::vector<double>(size, 0));
    for (int i = 0; i < size; i++) {
        matrix[i][i] = 1;
    }

    return matrix; 
}

struct Matrix3D {
	std::vector<std::vector<double>> matrix = createIdentityMatrix(4);
	void setIdentity() {
		matrix[0][0] = 1;
		matrix[0][1] = 0;
		matrix[0][2] = 0;
		matrix[0][3] = 0;
		
		matrix[1][0] = 0;
		matrix[1][1] = 1;
		matrix[1][2] = 0;
		matrix[1][3] = 0;
		
		matrix[2][0] = 0;
		matrix[2][1] = 0;
		matrix[2][2] = 1;
		matrix[2][3] = 0;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
	}
	
	void setTranslate(double a, double b, double c) {
		matrix[0][0] = 1;
		matrix[0][1] = 0;
		matrix[0][2] = 0;
		matrix[0][3] = a;
		
		matrix[1][0] = 0;
		matrix[1][1] = 1;
		matrix[1][2] = 0;
		matrix[1][3] = b;
		
		matrix[2][0] = 0;
		matrix[2][1] = 0;
		matrix[2][2] = 1;
		matrix[2][3] = c;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
	}
	
	void setRotateX(double degree) {
        double rad = (degree * (pi / 180));
        double sin = std::sin(rad);
        double cos = std::cos(rad);
		
		matrix[0][0] = 1;
		matrix[0][1] = 0;
		matrix[0][2] = 0;
		matrix[0][3] = 0;
		
		matrix[1][0] = 0;
		matrix[1][1] = cos;
		matrix[1][2] = -sin;
		matrix[1][3] = 0;
		
		matrix[2][0] = 0;
		matrix[2][1] = sin;
		matrix[2][2] = cos;
		matrix[2][3] = 0;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
	}

    void setRotateY(double degree) {
        double rad = (degree * (pi / 180));
        double sin = std::sin(rad);
        double cos = std::cos(rad);

        matrix[0][0] = cos;
		matrix[0][1] = 0;
		matrix[0][2] = -sin;
		matrix[0][3] = 0;
		
		matrix[1][0] = 0;
		matrix[1][1] = 1;
		matrix[1][2] = 0;
		matrix[1][3] = 0;
		
		matrix[2][0] = sin;
		matrix[2][1] = 0;
		matrix[2][2] = cos;
		matrix[2][3] = 0;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
    }

    void setRotateZ(double degree) {
        double rad = (degree * (pi / 180));
        double sin = std::sin(rad);
        double cos = std::cos(rad);

        matrix[0][0] = cos;
		matrix[0][1] = -sin;
		matrix[0][2] = 0;
		matrix[0][3] = 0;
		
		matrix[1][0] = sin;
		matrix[1][1] = cos;
		matrix[1][2] = 0;
		matrix[1][3] = 0;
		
		matrix[2][0] = 0;
		matrix[2][1] = 0;
		matrix[2][2] = 1;
		matrix[2][3] = 0;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 0;
		matrix[3][3] = 1;
    }
	
    vector3d multiplyVector3d(vector3d v) {
        return {
            x: v.x * matrix[0][0] + v.y * matrix[0][1] + v.z * matrix[0][2] + v.w * matrix[0][3],
            y: v.x * matrix[1][0] + v.y * matrix[1][1] + v.z * matrix[1][2] + v.w * matrix[1][3],
            z: v.x * matrix[2][0] + v.y * matrix[2][1] + v.z * matrix[2][2] + v.w * matrix[2][3],
            w: v.x * matrix[3][0] + v.y * matrix[3][1] + v.z * matrix[3][2] + v.w * matrix[3][3]
        };
    };
};

struct Triangle3D {
	vector3d p1;
	vector3d p2;
	vector3d p3;
	
	void draw(Matrix3D m) {
		p1 = m.multiplyVector3d(p1);
		p2 = m.multiplyVector3d(p2);
		p3 = m.multiplyVector3d(p3);
		
        paintCoordinates(image, bresenhamLine(p1.x, p1.y, p2.x, p2.y));
        paintCoordinates(image, bresenhamLine(p2.x, p2.y, p3.x, p3.y));
        paintCoordinates(image, bresenhamLine(p3.x, p3.y, p1.x, p1.y));
	};
};
Triangle3D t1;
Triangle3D t2;
Triangle3D t3;
Triangle3D t4;

vector2d multiplyVector2d(vector2d v, std::vector<std::vector<double>> matriz) {
    return {
        x: v.x * matriz[0][0] + v.y * matriz[0][1] + v.w * matriz[0][2], 
        y: v.x * matriz[1][0] + v.y * matriz[1][1] + v.w * matriz[1][2], 
        w: v.x * matriz[2][0] + v.y * matriz[2][1] + v.w * matriz[2][2]
    };
}

void setTranslate2d(std::vector<std::vector<double>> &matriz, double a, double b) {
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
            cv::imshow(WINDOW_NAME, image);
        }
    }
}

void clearImage() {
    image = (IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0)); // Clear image i.e. sets to black again.
}

void draw3DTriangle(Matrix3D mat3d) {
    clearImage();

    t1.draw(mat3d);
    t2.draw(mat3d);
    t3.draw(mat3d);
    t4.draw(mat3d);

    cv::imshow(WINDOW_NAME, image);
}

void drawRectangle(rectangle rect) {
    clearImage();
    paintCoordinates(image, bresenhamLine(rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y));
    paintCoordinates(image, bresenhamLine(rect.p2.x, rect.p2.y, rect.p3.x, rect.p3.y));
    paintCoordinates(image, bresenhamLine(rect.p3.x, rect.p3.y, rect.p4.x, rect.p4.y));
    paintCoordinates(image, bresenhamLine(rect.p4.x, rect.p4.y, rect.p1.x, rect.p1.y));
    cv::imshow(WINDOW_NAME, image);
}

int waitAndExecutePressedKeyAction() {
    char pressedKey = (char) cv::waitKey(0);
    std::cout << "Key pressed: " << pressedKey << std::endl;
    std::vector<std::vector<double>> identity = createIdentityMatrix(3);
    Matrix3D mat3d;

    if (pressedKey == 'c') {
        return 1;
    }

    if (pressedKey == 'Q') { // left
        setTranslate2d(identity, -10, 0);
    }

    if (pressedKey == 'R') { // up
        setTranslate2d(identity, 0, -10);
    }

    if (pressedKey == 'S') { // right
        setTranslate2d(identity, 10, 0);
    }

    if (pressedKey == 'T') { // down
        setTranslate2d(identity, 0, 10);
    }

    if (pressedKey == 'e') {
        // rotateRectangle(identity, 15, lastClickedCoordinate);
        mat3d.setRotateX(5);
    }

    if (pressedKey == 'q') {
        // rotateRectangle(identity, -15, lastClickedCoordinate);
        mat3d.setRotateX(-5);
    }

    if (pressedKey == 'a') {
        mat3d.setRotateY(5);
    }

    if (pressedKey == 'd') {
        mat3d.setRotateY(-5);
    }

    if (pressedKey == 'z') {
        mat3d.setRotateZ(5);
    }

    if (pressedKey == 'v') {
        mat3d.setRotateZ(-5);
    }
    
    // rect.p1 = multiplyVector2d(rect.p1, identity);
    // rect.p2 = multiplyVector2d(rect.p2, identity);
    // rect.p3 = multiplyVector2d(rect.p3, identity);
    // rect.p4 = multiplyVector2d(rect.p4, identity);
    // drawRectangle(rect);

    draw3DTriangle(mat3d);

    return waitAndExecutePressedKeyAction();
}

int main() {
    cv::namedWindow(WINDOW_NAME);
    cv::setMouseCallback(WINDOW_NAME, mouseHandler);

    rect = {
        { 200, 200, 1 },
        { 300, 200, 1 },
        { 300, 300, 1 },
        { 200, 300, 1 }
    };

    vector3d p1 = { 500, 400, 0, 1 };
    vector3d p2 = { 600, 500, 0, 1 };
    vector3d p3 = { 400, 500, 0, 1 };
    vector3d p4 = { 500, 450, 100, 1 };

    t1 = { p1, p2, p3 };
    t2 = { p1, p2, p4 };
    t3 = { p2, p3, p4 };
    t4 = { p3, p1, p4 };

    // drawRectangle(rect);

    Matrix3D m;
    draw3DTriangle(m);

    waitAndExecutePressedKeyAction();
  
    return 0;
}
