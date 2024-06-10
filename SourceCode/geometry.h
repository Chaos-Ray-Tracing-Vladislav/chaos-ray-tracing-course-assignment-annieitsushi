#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <math.h>
#include <fstream>

/// A class that describes a vector and operations with it.
class Vector {
private:
    /// The coordinates of the vector.
    float x, y, z;
public:
    /// Constructors.
    Vector() : x(0.0), y(0.0), z(0.0) {}
    Vector(float xCoordinate, float yCoordinate, float zCoordinate)
        : x(xCoordinate)
        , y(yCoordinate)
        , z(zCoordinate) {}
    Vector(const Vector &otherVector)
        : x(otherVector.x)
        , y(otherVector.y)
        , z(otherVector.z) {}
    /// Operations.
    Vector operator+(const Vector& otherVector) const {
        return Vector(x+otherVector.x, y+otherVector.y, z+otherVector.z);
    }
    Vector operator-(const Vector& otherVector) const {
        return Vector(x-otherVector.x, y-otherVector.y, z-otherVector.z);
    }
    Vector operator*(float multiplier) const {
        return Vector(x*multiplier, y*multiplier, z*multiplier);
    }
    float length() const {
        return std::sqrt(x*x+y*y+z*z);
    }
    void normalize();
    float dotProduct(const Vector &otherVector) {
        return x*otherVector.x+y*otherVector.y+z*otherVector.z;
    }
    Vector crossProduct(const Vector &otherVector) const;
    Vector absolute() const {
        return Vector(fabs(x), fabs(y), fabs(z));
    }
    float findParallelogramArea(const Vector &otherVector) const;
    /// Getters.
    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }
    float getZ() const {
        return z;
    }
    /// Output operator.
    friend std::ofstream &operator<<(std::ofstream &outputStream, const Vector &vector);
};

/// A structure that represents a ray by its origin and direction.
class Ray {
private:
    Vector origin;
    Vector direction;
public:
    Ray() : origin(), direction() {}
    Ray(const Vector &originParameter, const Vector directionParameter)
    : origin(originParameter)
    , direction(directionParameter) {}
    const Vector &getOrigin() const {
        return origin;
    }
    const Vector &getDirection() const {
        return direction;
    }
};

/// A class representing a triangle by its three vertices.
class Triangle {
private:
    Vector v0, v1, v2;
public:
    /// Constructors.
    Triangle(): v0(), v1(), v2() {}
    Triangle(const Vector &vector0, const Vector &vector1, const Vector &vector2)
        : v0(vector0), v1(vector1), v2(vector2) {}
    /// Getters.
    const Vector &getV0() const {
        return v0;
    }
    const Vector &getV1() const {
        return v1;
    }
    const Vector &getV2() const {
        return v2;
    }
    /// Find the normal vector of the triangle.
    Vector getNormalVector() const;
    /// Calculate the area of the triangle.
    float getArea() const;
    /// Output operator.
    friend std::ofstream &operator<<(std::ofstream &outputStream, const Triangle &trianle);
};

#endif
