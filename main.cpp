#include <iostream> 
#include <fstream>
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
#include <opencv2/imgproc.hpp>
#include <vector>

const double pi = 3.14159265359;
const char* WINDOW_NAME = "Computacao_Grafica";
const int IMAGE_HEIGHT = 1000;
const int IMAGE_WIDTH = 1000;
cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));

struct vector3d { double x, y, z, w; };
struct coordinate { double x, y; };

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::vector<coordinate> bresenhamLine(double x, double y, double x2, double y2) {
    double w = x2 - x;
    double h = y2 - y;
    double dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0;

    if (w < 0) dx1 = -1; else if (w > 0) dx1 = 1;
    if (h < 0) dy1 = -1; else if (h > 0) dy1 = 1;
    if (w < 0) dx2 = -1; else if (w > 0) dx2 = 1;

    double longest = abs(w);
    double shortest = abs(h);
    if (longest <= shortest) {
        longest = abs(h);
        shortest = abs(w);
        if (h < 0) dy2 = -1; else if (h > 0) dy2 = 1;
        dx2 = 0;
    }

    std::vector<coordinate> coordinates = {};
    double numerator = (int) longest >> 1;
    for (double i = 0; i <= longest; i++) {
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
	
    void setSimplePerspective(double d_cp) {
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
		matrix[2][2] = 0;
		matrix[2][3] = 0;
		
		matrix[3][0] = 0;
		matrix[3][1] = 0;
		matrix[3][2] = 1 / d_cp;
		matrix[3][3] = 1;
    }

    Matrix3D multiplyMatrix(Matrix3D multiply) {
		Matrix3D result;
	    for (int l = 0; l < 4; l++) {
	        for (int i = 0; i < 4; i++) {
	            for (int h = 0; h < 4; h++) {
	                result.matrix[l][i] += matrix[l][h] * multiply.matrix[h][i];
	            }
	        }
	    }

	    return result;
    }

    vector3d multiplyVector3d(vector3d v) {
        vector3d output = {
            x: v.x * matrix[0][0] + v.y * matrix[0][1] + v.z * matrix[0][2] + v.w * matrix[0][3],
            y: v.x * matrix[1][0] + v.y * matrix[1][1] + v.z * matrix[1][2] + v.w * matrix[1][3],
            z: v.x * matrix[2][0] + v.y * matrix[2][1] + v.z * matrix[2][2] + v.w * matrix[2][3],
            w: v.x * matrix[3][0] + v.y * matrix[3][1] + v.z * matrix[3][2] + v.w * matrix[3][3]
        };

        if (output.w != 0 && output.w != 1) {
			output.x = output.x / output.w;
			output.y = output.y / output.w;
			output.z = output.z / output.w;
		}

        return output;
    };
};
struct GeometricShape3D {
    std::vector<vector3d> points;
    void draw(Matrix3D m) {
        for (vector3d& p : points) {
            p = m.multiplyVector3d(p);
        }

        int pointsSize = points.size();
        for (int i = 0; i < pointsSize; i++) {
            vector3d current = points[i];
            vector3d next = ((i + 1) < pointsSize) ? points[i +1] : points[0]; // If next is outside array, get starting point.
            paintCoordinates(image, bresenhamLine(current.x, current.y, next.x, next.y));
        }
	};
};

std::vector<GeometricShape3D> tankShapes;

void drawTank(Matrix3D m) {
    for (GeometricShape3D& shape : tankShapes) {
        shape.draw(m);
    }
}

void clearImage() {
    image = (IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0)); // Clear image i.e. sets to black again.
}

void displayImage() { // Turn image upsidedown & display it
    cv::Mat dst;
    cv::Point2f pt(image.cols/2., image.rows/2.);
    cv::warpAffine(image, dst, cv::getRotationMatrix2D(pt, 180, 1.0), cv::Size(image.cols, image.rows));
    image = dst;
    cv::imshow(WINDOW_NAME, image);
}

void waitAndExecutePressedKeyAction() {
    char pressedKey = (char) cv::waitKey(0);
    std::cout << "Key pressed: " << pressedKey << std::endl;
    Matrix3D mat3d;

    if (pressedKey == 'c') {
        return;
    }

    if (pressedKey == 'Q') { // left

    }

    if (pressedKey == 'R') { // up
        
    }

    if (pressedKey == 'S') { // right
        
    }

    if (pressedKey == 'T') { // down

    }

    if (pressedKey == 'e') {
        mat3d.setRotateX(5);
    }

    if (pressedKey == 'q') {
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
    
    clearImage();
    drawTank(mat3d);
    displayImage();

    return waitAndExecutePressedKeyAction();
}

class objLoader {
    struct trigFace { int v1, v2, v3; };
    struct rectFace { int v1, v2, v3, v4; };
    public:
        std::vector<vector3d> vertexes;
        std::vector<trigFace> trigFaces;
        std::vector<rectFace> rectFaces;
        void readfile(std::string filename) {
            std::ifstream file(filename);
            if (file.is_open()) {
                std::string line;            
                while (std::getline(file, line)) {
                    if (
                        line.empty()
                        || (line.at(0) != 'v' && line.at(0) != 'f')
                    ) {
                        continue;
                    }

                    std::istringstream iss(line);
                    std::string lineId;
                    iss >> lineId;
                    if (lineId == "v") {
                        vector3d v;
                        iss >> v.x >> v.y >> v.z;
                        v.x += IMAGE_WIDTH / 2;
                        v.y += IMAGE_HEIGHT / 2; // Traz pro centro do imagem
                        v.z += IMAGE_WIDTH / 2;
                        v.w = 1;
                        vertexes.push_back(v);
                        continue;
                    }

                    if (lineId == "f") {
                        std::vector<int> vertexPropsVec;
                        std::string vertexProps;
                        while (iss >> vertexProps) {
                            // use first value in f row, i.e. xx/*ignored* & [0], because that's the vertex index, the others are textures indexes
                            int vertexIndex = std::stoi(split(vertexProps, '/')[0]) -1; // -1 because .obj uses indexes starting from 1 instead of 0
                            vertexPropsVec.push_back(vertexIndex);
                        }

                        GeometricShape3D s;
                        for (auto& vertexPropIndex : vertexPropsVec) {
                            s.points.push_back(vertexes[vertexPropIndex]);
                        }
                        tankShapes.push_back(s);
                    }
                }
                file.close();
            }
        };
};

int main() {
    cv::namedWindow(WINDOW_NAME);

    objLoader obj;
    obj.readfile("tank.obj");

    Matrix3D m;
    drawTank(m);
    displayImage();

    waitAndExecutePressedKeyAction();
    return 0;
}
