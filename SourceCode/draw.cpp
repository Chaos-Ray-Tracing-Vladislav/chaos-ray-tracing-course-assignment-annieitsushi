#include <math.h>
#include <stdio.h>
#include <fstream>

#include "draw.h"
#include "utils.h"

#define EPSILON 0.0001

// Color.
Color &Color::operator=(const Color &otherColor) {
    R=otherColor.R;
    B=otherColor.B;
    G=otherColor.G;

    return *this;
}

Color Color::operator+(const Color &otherColor) const {
    const int newR=std::min(R+otherColor.R, 255);
    const int newG=std::min(G+otherColor.G, 255);
    const int newB=std::min(B+otherColor.B, 255);
    return Color(newR, newG, newB);
}

Color Color::operator-(const Color &otherColor) const {
    const int newR=(R-otherColor.R);
    const int newG=(G-otherColor.G);
    const int newB=(B-otherColor.B);
    return Color(newR, newG, newB);
}

Color Color::operator*(double multiplier) const {
    if (multiplier<0) {
        return *this;
    }
    const int newR=int(std::min(R*multiplier, 255.0));
    const int newG=int(std::min(G*multiplier, 255.0));
    const int newB=int(std::min(B*multiplier, 255.0));
    return Color(newR, newG, newB);
}

Color Color::invert() const {
    return Color(abs(255-R), abs(255-G), abs(255-B));
}

Color Color::interpolate(const Color &otherColor, double multiplier) const {
    return (otherColor-(*this))*multiplier+(*this);
}

Color Color::addNoise() const {
    const int grayValue=((rand()%2)==0)
        ? rand()%255
        : -rand()%255;
    const int newR=clamp(R+grayValue, 0, 255);
    const int newG=clamp(G+grayValue, 0, 255);
    const int newB=clamp(B+grayValue, 0, 255);
    return Color(newR, newG, newB);
}

// ImageDrawer.
void ImageDrawer::fillSolidBackground(const Color &backgroundColor) {
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            pixels[i][j]=backgroundColor;
        }
    }
}

void ImageDrawer::fillGradientBackground(const Color &color1, const Color &color2) {
    for (int i=0; i<height; ++i) {
        const Color &interpolatedColor=color1.interpolate(color2, (double(i)/height));
        for (int j=0; j<width; ++j) {
            pixels[i][j]=interpolatedColor;
        }
    }
}

void ImageDrawer::draw() const {
    // Open the output stream.
    std::ofstream outputStream(outputFilePath, std::ofstream::out);
    // If it's not valid, print an error and early return.
    if (!outputStream.is_open()) {
        printf("Couldn't open the given file path.\n");
        return;
    }
    // Print the magic number.
    outputStream<<magicNumber<<'\n';
    // Print the resolution.
    outputStream<<width<<' '<<height<<'\n';
    // Print the max intensity for a Color value.
    outputStream<<maxValue<<'\n';
    // Print the Color values per Color.
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            outputStream<<pixels[i][j].R<<' '<<pixels[i][j].G<<' '<<pixels[i][j].B<<'\t';
        }
        outputStream<<'\n';
    }
    // Close the output stream.
    outputStream.close();
    printf("Image drawn.\n");
}

// CircleDrawer.
void CircleDrawer::fillSolidCircle(const Color &color) {
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            const double dX=pow(j-centerX, 2);
            const double dY=pow(i-centerY, 2);
            const double raduisSquared=pow(radius, 2);
            if ((dX+dY-raduisSquared)<EPSILON) {
                pixels[i][j]=color;
            }
        }
    }
}

void CircleDrawer::fillGradientCircle(const Color &color1, const Color &color2) {
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            const double dX=pow(j-centerX, 2);
            const double dY=pow(i-centerY, 2);
            const double raduisSquared=pow(radius, 2);
            if ((dX+dY-raduisSquared)<EPSILON) {
                pixels[i][j]=color1.interpolate(color2, (double(i)/height));
            }
        }
    }
}

// RectangleDrawer.
void RectangleDrawer::fillSolidRectangle(const Color &color) {
    const int lengthX=std::min(fromX+sizeX, int(width));
    const int lengthY=std::min(fromY+sizeY, int(height));
    for (int i=fromY; i<lengthY; ++i) {
        for (int j=fromX; j<lengthX; ++j) {
            pixels[i][j]=color;
        }
    }
}

void RectangleDrawer::fillGradientRectangle(const Color &color1, const Color &color2) {
    const int lengthX=std::min(fromX+sizeX, int(width));
    const int lengthY=std::min(fromY+sizeY, int(height));
    for (int i=fromY; i<lengthY; ++i) {
        for (int j=fromX; j<lengthX; ++j) {
            pixels[i][j]=color1.interpolate(color2, (double(i)/height));
        }
    }
}

void RectangleDrawer::fillNoiseRectangle(const Color &color) {
    const int lengthX=std::min(fromX+sizeX, int(width));
    const int lengthY=std::min(fromY+sizeY, int(height));
    for (int i=fromY; i<lengthY; ++i) {
        for (int j=fromX; j<lengthX; ++j) {
            pixels[i][j]=color.addNoise();
        }
    }
}

// RayDrawer.
void RayDrawer::prepareRays() {
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            float x=j, y=i;
            // Find center.
            x+=0.5;
            y+=0.5;
            // To NDC space - [0.0, 1.0].
            x/=width;
            y/=height;
            // To screen space - [-1.0, 1.0].
            x=(2.0*x)-1.0;
            y=1.0-(2.0*y);
            // Aspect ratio.
            x*=float(width)/height;
            // Direction.
            Vector direction=Vector(x, y, -1)-cameraPosition;
            // Normalize vector.
            direction.normalize();
            rays[i][j]=Ray(cameraPosition, direction);
        }
    }
}

void RayDrawer::fillPixelsFromRays() {
    for (int i=0; i<height; ++i) {
        for (int j=0; j<width; ++j) {
            const Vector &currentDirection=rays[i][j].getDirection().absolute()*255.0;
            pixels[i][j].R=int(currentDirection.getX());
            pixels[i][j].G=int(currentDirection.getY());
            pixels[i][j].B=int(currentDirection.getZ());
        }
    }
}
