#ifndef DRAW_H
#define DRAW_H

#include <iostream>

#include "geometry.h"

/// A structure holding the information about a color - its
/// red, green and blue components.
struct Color {
    /// The red green and blue components.
    int R, G, B;

    /// Constructors.
    Color() : R(0), G(0), B(0) {}
    Color(int red, int green, int blue) : R(red), G(green), B(blue) {}

    /// Operators.
    Color &operator=(const Color &otherColor);
    Color operator+(const Color &otherColor) const;
    Color operator-(const Color &otherColor) const;
    Color operator*(double multiplier) const;
    /// Invert a color's value.
    Color invert() const;
    /// Interpolate between color values.
    Color interpolate(const Color &otherColor, double multiplier) const;
    /// Add random noise to a color.
    Color addNoise() const;
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
    void fillSolidBackground(const Color &backgroundColor);
    /// Fill the image with a gradient - interpolate between two given colors.
    void fillGradientBackground(const Color &color1, const Color &color2);
    /// Output the image to the .ppm file, based on the pixels stored in this class.
    void draw() const;
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
    void fillSolidCircle(const Color &color);
    /// Fills a circle with a gradient, interpolating between two colors.
    /// Note that this function does not fill in outside the circle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillGradientCircle(const Color &color1, const Color &color2);
};

/// A class with the functionality to draw rectangles with a
class RectangleDrawer : public ImageDrawer {
private:
    int fromX, fromY;
    int sizeX, sizeY;
public:
    /// Constructors.
    RectangleDrawer(const std::string &newOutputFilePath, size_t newWidth, size_t newHeight)
        : ImageDrawer(newOutputFilePath, newWidth, newHeight)
        , fromX(0)
        , fromY(0)
        , sizeX(0)
        , sizeY(0){}
    RectangleDrawer(
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
    void fillSolidRectangle(const Color &color);
    /// Fill a rectangle with gradient interpolated between two colors.
    /// Note that this function does not fill in outside the rectangle, so that it can be used to fill multiple
    /// circles in the same image. To draw a background color fillSolidBackground(..) or fillGradientBackground(..)
    /// must be called beforehand.
    void fillGradientRectangle(const Color &color1, const Color &color2);
    void fillNoiseRectangle(const Color &color);
};

/// A class that draws based on rays from a camera.
class RayDrawer : public ImageDrawer {
private:
    Vector cameraPosition;
    Ray **rays;
public:
    /// Constructors.
    RayDrawer(const std::string &newOutputFilePath, size_t newWidth, size_t newHeight)
        : ImageDrawer(newOutputFilePath, newWidth, newHeight)
        , cameraPosition(0.0, 0.0, 0.0) {
        rays=new Ray*[height];
        for (int i=0; i<height; ++i) {
            rays[i]=new Ray[width];
        }
    }
    ~RayDrawer() {
        for (int i=0; i<height; ++i) {
            delete [] rays[i];
        }
        delete [] rays;
    }
    /// Centralize and normalize the rays per pixel for screen space.
    void prepareRays();
    /// Draw a color in each pixel depending on the corresponding normalized ray to the pixel.
    void fillPixelsFromRays();
};

#endif
