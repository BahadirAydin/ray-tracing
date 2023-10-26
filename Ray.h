#include "parser.h"

class Ray {
public:
  void set_origin(const parser::Vec3f &origin) { this->origin = origin; }

  void set_direction(const parser::Vec3f &direction) {
    this->direction = direction;
  }

private:
  parser::Vec3f origin;
  parser::Vec3f direction;
};
