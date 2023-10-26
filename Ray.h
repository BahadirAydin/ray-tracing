#ifndef RAY_H
#define RAY_H

#include "parser.h"
#include <iostream>

class Ray {
public:
    void set_origin(const parser::Vec3f &origin) { this->origin = origin; }

    void set_direction(const parser::Vec3f &direction) {
        this->direction = direction;
    }

    void print() {
        std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z 
                  << " " << "direction: " <<direction.x << " " << direction.y << " " 
                  << direction.z << std::endl;
    }

private:
    parser::Vec3f origin;
    parser::Vec3f direction;
};

#endif // RAY_H
