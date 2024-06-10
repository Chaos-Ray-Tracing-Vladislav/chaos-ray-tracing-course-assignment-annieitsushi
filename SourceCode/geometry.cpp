#include "geometry.h"

// Vector.
void Vector::normalize() {
    float vectorLength=length();
    x=x/vectorLength;
    y=y/vectorLength;
    z=z/vectorLength;
}

Vector Vector::crossProduct(const Vector &otherVector) const {
    return Vector(
        y*otherVector.z-z*otherVector.y,
        z*otherVector.x-x*otherVector.z,
        x*otherVector.y-y*otherVector.x
    );
}

float Vector::findParallelogramArea(const Vector &otherVector) const {
    return crossProduct(otherVector).length();
}

std::ofstream &operator<<(std::ofstream &outputStream, const Vector &vector) {
    outputStream<<"Vector("<<vector.x<<", "<<vector.y<<", "<<vector.z<<")";
}

// Triangle.
Vector Triangle::getNormalVector() const {
    // Get the vector between the first and second vertex.
    const Vector &e0=v1-v0;
    // Get the vector between the first and the third vertex.
    const Vector &e1=v2-v0;
    // Their cross product normalized is the normal vector of the triangle.
    Vector normalVector=e0.crossProduct(e1);
    normalVector.normalize();
    return normalVector;
}

float Triangle::getArea() const {
    // Get the vector between the first and second vertex.
    const Vector &e0=v1-v0;
    // Get the vector between the first and the third vertex.
    const Vector &e1=v2-v0;
    // The magnitude of the cross product is the area of the parallelogram between the vectors.
    // Half of it is the area of the triangle.
    return e0.findParallelogramArea(e1)*0.5;
}

std::ofstream &operator<<(std::ofstream &outputStream, const Triangle &triangle) {
    outputStream<<"Triangle(\n\t";
    outputStream<<triangle.getV0();
    outputStream<<",\n\t";
    outputStream<<triangle.getV1();
    outputStream<<",\n\t";
    outputStream<<triangle.getV2();
    outputStream<<"\n)";
}
