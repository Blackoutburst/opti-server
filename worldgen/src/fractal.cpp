#include "fractal.hpp"

// float de(glm::vec3 p){  //(distance bound doesn't hold)
//     glm::vec3 v=glm::vec3(0,1.5,6.3);
//     return glm::min(6.0f-glm::length((p-v).xy+glm::sin(p.yx())), glm::dot(glm::cos(p),glm::sin(p.yzx())))+glm::sin(glm::sin(p.z*3.5f)+v.z)*0.1f+1.0f;
// }

// if ( de(glm::vec3(world_x, world_y - 200, world_z) * 0.01f) < 0.005f) {
float de(glm::vec3 p) {
    constexpr float SCALE = 0.0025f;

    if (!(p.x > -1.0f/SCALE && p.x > -1.0f/SCALE && p.y < 0.6f/SCALE && p.y < 0.8f/SCALE && p.z > -0.62f/SCALE && p.z < 1.2f/SCALE)) return 1.0f;
    p *= SCALE;

    const glm::vec3 va = glm::vec3(  0.0,  0.57735,  0.0 );
    const glm::vec3 vb = glm::vec3(  0.0, -1.0,  1.15470 );
    const glm::vec3 vc = glm::vec3(  1.0, -1.0, -0.57735 );
    const glm::vec3 vd = glm::vec3( -1.0, -1.0, -0.57735 );
    float a = 0.0f;
    float s = 1.0f;
    float r = 1.0f;
    float dm;
    glm::vec3 v;

    for(int i=0; i<16; i++) {
        float d, t;
        d = glm::dot(p-va,p-va);              v=va; dm=d; t=0.0f;
        d = glm::dot(p-vb,p-vb); if( d < dm ) { v=vb; dm=d; t=1.0f; }
        d = glm::dot(p-vc,p-vc); if( d < dm ) { v=vc; dm=d; t=2.0f; }
        d = glm::dot(p-vd,p-vd); if( d < dm ) { v=vd; dm=d; t=3.0f; }
        p = v + 2.0f*(p - v); r*= 2.0f;
        a = t + 4.0f*a; s*= 4.0f;
    }
    return (glm::sqrt(dm)-1.0f)/r;
  }
