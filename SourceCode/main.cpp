#include <stdio.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <time.h>

#define EPSILON 0.0001

int clamp(int number, int lower, int upper) {
  return std::max(lower, std::min(number, upper));
}

/// A structure holding the information about a color - its
/// red, green and blue components.
struct Color {
    /// The red green and blue components.
    int R, G, B;

    /// Constructors.
    Color() : R(0), G(0), B(0) {}
    Color(int red, int green, int blue) : R(red), G(green), B(blue) {}

    /// Operators.
    Color operator+(const Color &otherColor) const {
        const int newR=std::min(R+otherColor.R, 255);
        const int newG=std::min(G+otherColor.G, 255);
        const int newB=std::min(B+otherColor.B, 255);
        return Color(newR, newG, newB);
    }
    Color operator-(const Color &otherColor) const {
        const int newR=(R-otherColor.R);
        const int newG=(G-otherColor.G);
        const int newB=(B-otherColor.B);
        return Color(newR, newG, newB);
    }
    Color operator*(double multiplier) {
        if (multiplier<0) {
            return *this;
        }
        const int newR=int(std::min(R*multiplier, 255.0));
        const int newG=int(std::min(G*multiplier, 255.0));
        const int newB=int(std::min(B*multiplier, 255.0));
        return Color(newR, newG, newB);
    }

    /// Invert a color's value.
    Color invert() const {
        return Color(abs(255-R), abs(255-G), abs(255-B));
    }
    /// Interpolate between color values.
    Color interpolate(const Color &otherColor, double multiplier) const {
        return (otherColor-(*this))*multiplier+(*this);
    }

    /// Add random noise to a color.
    Color addNoise() const {
        const int grayValue=((rand()%2)==0)
            ? rand()%255
            : -rand()%255;
        const int newR=clamp(R+grayValue, 0, 255);
        const int newG=clamp(G+grayValue, 0, 255);
        const int newB=clamp(B+grayValue, 0, 255);
        return Color(newR, newG, newB);
    }
};

/// A base class to draw to a .ppm file.
class ImageDrawer {
private:
    /// The magic number for the .ppm file.
    const std::string magicNumber="P3";
    /// The path to the output .ppm file.
    std::string outputFilePath;
protected:
    /// The file resolution.
    size_t width, height;
    /// A matrix of pixels.
    Color **pixels;
    /// The max intensity value for a Color.
    const int maxValue=255;
public:
    /// Constructors.
    ImageDrawer(const std::string &newOutputFilePath, size_t newWidth, size_t newHeight)
        : outputFilePath(newOutputFilePath)
        , width(newWidth)
        , height(newHeight) {
        pixels=new Color*[height];
        for (int i=0; i<height; ++i) {
            pixels[i]=new Color[width];
        }
    }
    ~ImageDrawer() {
        for (int i=0; i<height; ++i) {
            delete [] pixels[i];
        }
        delete [] pixels;
    }

    /// A function to change the output .ppm file.
    void changeOutputFile(const std::string &newOutputFilePath) {
        outputFilePath=newOutputFilePath;
    }

    /// Fill the image with a solid background color.
    void fillSolidBackground(const Color &backgroundColor) {
        for (int i=0; i<height; ++i) {
            for (int j=0; j<width; ++j) {
                pixels[i][j]=backgroundColor;
            }
        }
    }

    /// Fill the image with a gradient - interpolate between two given colors.
    void fillGradientBackground(const Color &color1, const Color &color2) {
        for (int i=0; i<height; ++i) {
            const Color &interpolatedColor=color1.interpolate(color2, (double(i)/height));
            for (int j=0; j<width; ++j) {
                pixels[i][j]=interpolatedColor;
            }
        }
    }

    /// Output the image to the .ppm file, based on the pixels stored in this class.
    void draw() {
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
};

/// A class with the functionality to draw circles of a given radius.
class CircleDrawer : public ImageDrawer {
private:
    /// The radius of the circle.
    int radius;
    int centerX, centerY;
public:
    /// Constructor.
    /// If the center coordinates are not specified, make it in the center of the image.
    CircleDrawer(const std::string &newOutputFilePath, size_t newWidth, size_t newHeight, int newRadius, int newCenterX=-1, int newCenterY=-1)
        : ImageDrawer(newOutputFilePath, newWidth, newHeight)
        , radius(newRadius)
        , centerX(newCenterX)
        , centerY(newCenterY) {
        if (centerX==-1 || centerY==-1) {
            centerX=width/2;
            centerY=height/2;
        }
    }

    /// Change the parameters of the circle.
    /// If the center coordinates are not specified, make it in the center of the image.
    void changeCircle(int newRadius, int newCenterX=-1, int newCenterY=-1) {
        radius=newRadius;
        if (centerX==-1 || centerY==-1) {
            centerX=width/2;
            centerY=height/2;
        }
    }

    /// Fills a circle with a solid color.
    /// Note that this function does not fill in outside the circle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillSolidCircle(const Color &color) {
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

    /// Fills a circle with a gradient, interpolating between two colors.
    /// Note that this function does not fill in outside the circle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillGradientCircle(const Color &color1, const Color &color2) {
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
};

/// A class with the functionality to draw rectangles with a
class RectagnleDrawer : public ImageDrawer {
private:
    int fromX, fromY;
    int sizeX, sizeY;
public:
    /// Constructors.
    RectagnleDrawer(const std::string &newOutputFilePath, size_t newWidth, size_t newHeight)
        : ImageDrawer(newOutputFilePath, newWidth, newHeight)
        , fromX(0)
        , fromY(0)
        , sizeX(0)
        , sizeY(0){}
    RectagnleDrawer(
        const std::string &newOutputFilePath,
        size_t newWidth,
        size_t newHeight,
        int newFromX,
        int newFromY,
        int newSizeX,
        int newSizeY
    ) : ImageDrawer(newOutputFilePath, newWidth, newHeight), fromX(newFromX), fromY(newFromY), sizeX(newSizeX), sizeY(newSizeY) {}

    /// Change the parameters of a rectangle.
    void changeRectangle(
        int newFromX,
        int newFromY,
        int newSizeX,
        int newSizeY
    ) {
        fromX=newFromX;
        fromY=newFromY;
        sizeX=newSizeX;
        sizeY=newSizeY;
    }

    /// Fill a rectangle with solid color.
    /// Note that this function does not fill in outside the rectangle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillSolidRectangle(const Color &color) {
        const int lengthX=std::min(fromX+sizeX, int(width));
        const int lengthY=std::min(fromY+sizeY, int(height));
        for (int i=fromY; i<lengthY; ++i) {
            for (int j=fromX; j<lengthX; ++j) {
                pixels[i][j]=color;
            }
        }
    }

    /// Fill a rectangle with gradient interpolated between two colors.
    /// Note that this function does not fill in outside the rectangle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillGradientRectangle(const Color &color1, const Color &color2) {
        const int lengthX=std::min(fromX+sizeX, int(width));
        const int lengthY=std::min(fromY+sizeY, int(height));
        for (int i=fromY; i<lengthY; ++i) {
            for (int j=fromX; j<lengthX; ++j) {
                pixels[i][j]=color1.interpolate(color2, (double(i)/height));
            }
        }
    }

    void fillNoiseRectangle(const Color &color) {
        const int lengthX=std::min(fromX+sizeX, int(width));
        const int lengthY=std::min(fromY+sizeY, int(height));
        for (int i=fromY; i<lengthY; ++i) {
            for (int j=fromX; j<lengthX; ++j) {
                pixels[i][j]=color.addNoise();
            }
        }
    }
};

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
    RectagnleDrawer rectangleDrawer(outputFilePath, width, height);
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

int main(int argc, const char *argv[]) {
    task2();
    return 0;
}
