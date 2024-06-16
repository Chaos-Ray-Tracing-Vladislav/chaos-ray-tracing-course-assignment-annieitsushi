#include <time.h>
#include <fstream>

#include "draw.h"

/// Draws a sun image to a given .ppm file. The sun is a circle with a gradient, and
/// the sky is background with a gradient.
void drawSun(const std::string &outputFilePath, size_t width, size_t height) {
    const int sunRadius=std::min(width/2, height/2);
    const int offset=sunRadius/10;
    CircleDrawer sunDrawer(outputFilePath, width, height, sunRadius-offset);
    const Color skyColor1(173, 216, 230);
    const Color skyColor2(250, 218, 221);
    sunDrawer.fillGradientBackground(skyColor1, skyColor2);
    const Color sunColor1(255, 219, 111);
    const Color sunColor2(255, 127, 127);
    sunDrawer.fillGradientCircle(sunColor1, sunColor2);
    sunDrawer.draw();
}

/// Draws a rectangle with noise based on a given color.
void drawNoiseGrid(const std::string &outputFilePath, size_t width, size_t height) {
    srand(time(0));
    RectangleDrawer rectangleDrawer(outputFilePath, width, height);
    const int oneFourthWidth=width/4;
    const int oneForthHeight=height/4;
    for (int i=0; i<4; ++i) {
        const int fromY=i*oneForthHeight;
        for (int j=0; j<4; ++j) {
            const int fromX=j*oneFourthWidth;
            rectangleDrawer.changeRectangle(fromX, fromY, oneFourthWidth, oneForthHeight);
            const Color randomColor(rand()%255, rand()%255, rand()%255);
            rectangleDrawer.fillNoiseRectangle(randomColor);
        }
    }
     rectangleDrawer.draw();
}

// Homework task 2.
void task2() {
    drawSun("../Images/Homework_2/circle.ppm", 1920, 1080);
    drawNoiseGrid("../Images/Homework_2/grid.ppm", 1920, 1080);
}

void task3() {
    RayDrawer rayDrawer("../Images/Homework_3/normalized.ppm", 1920, 1080);
    rayDrawer.prepareRays();
    rayDrawer.fillPixelsFromRays();
    rayDrawer.draw();
}

void printCrossProduct(std::ofstream &outputStream, const Vector &A, const Vector &B) {
    outputStream<<"The cross product of ";
    outputStream<<A;
    outputStream<<" and ";
    outputStream<<B;
    outputStream<<" is:\n";
    outputStream<<A.crossProduct(B)<<'\n';
    outputStream<<'\n';
}

void printParallelogramArea(std::ofstream &outputStream, const Vector &A, const Vector &B) {
    outputStream<<"The parallelogram area formed by ";
    outputStream<<A;
    outputStream<<" and ";
    outputStream<<B;
    outputStream<<" is: "<<A.findParallelogramArea(B)<<'\n';
    outputStream<<'\n';
}

void printNormalVectorAndAreaOfTriangle(std::ofstream &outputStream, const Triangle &triangle) {
    outputStream<<"The normal vector of ";
    outputStream<<triangle;
    outputStream<<"\nis: ";
    outputStream<<triangle.getNormalVector();
    outputStream<<"\nAnd the area of the triangle is: "<<triangle.getArea()<<'\n';
    outputStream<<'\n';
}

void task4() {
    // Open the output stream.
    std::ofstream outputStream("../Images/Homework_4/calculations.txt", std::ofstream::out);
    if (!outputStream.is_open()) {
        printf("Couldn't open the given file path.\n");
        return;
    }
    const Vector A0(3.5, 0, 0), B0(1.75, 3.5, 0);
    printCrossProduct(outputStream, A0, B0);

    const Vector A1(3, -3, 1), B1(4, 9, 3);
    printCrossProduct(outputStream, A1, B1);

    const Vector A2(3, -3, 1), B2(4, 9, 3);
    printParallelogramArea(outputStream, A2, B2);

    const Vector A3(3, -3, 1), B3(-12, 12, -4);
    printParallelogramArea(outputStream, A3, B3);

    const Triangle triangle0(
        Vector(-1.75, -1.75, -3),
        Vector(1.75, -1.75, -3),
        Vector(0, 1.75, -3)
    );
    printNormalVectorAndAreaOfTriangle(outputStream, triangle0);

    const Triangle triangle1(
        Vector(0, 0, -1),
        Vector(1, 0, 1),
        Vector(-1, 0, 1)
    );
    printNormalVectorAndAreaOfTriangle(outputStream, triangle1);

    const Triangle triangle2(
        Vector(0.56, 1.11, 1.23),
        Vector(0.44, -2.368, -0.54),
        Vector(-1.56, 0.15, -1.92)
    );
    printNormalVectorAndAreaOfTriangle(outputStream, triangle2);

    // Close the output stream.
    outputStream.close();
    printf("File written.\n");
}

int main(int argc, const char *argv[]) {
    task3();
    task4();
    return 0;
}
