#pragma once
#include "stl_reader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include <tuple>
#include <vector>

namespace physics {
    using triangle = std::tuple<glm::vec3, glm::vec3, glm::vec3>;
    inline void _show_vec3(glm::vec3 &v);

    struct physics_model {
        std::vector<triangle> faces;
    };

    struct physics_object {
            physics_model &model;
            glm::vec3 position;
            glm::vec3 velocity;
            glm::vec3 acceleration;
            glm::quat rotate;
            glm::quat angleVelocity;
            float mass;
            float elasticity;
    };

    // load model from .stl file
    void load_model(physics::physics_model &model, const char *path) {
        stl_reader::StlMesh<float, unsigned int> mesh(path);
        for (size_t itri = 0; itri < mesh.num_tris(); ++itri) {
            for (size_t icorner = 0; icorner < 3; ++icorner) {
                const float *c = mesh.tri_corner_coords(itri, icorner);
                model.faces.push_back(physics::triangle(c[0], c[1], c[2]));
            }
        }
    }

    // Move object
    // timestamp = 1/fps
    void advance_step(physics_object &object, float timestamp) {
        // x = x0 + V0x*t + a * t^2 * 0.5
        object.position += (object.velocity * timestamp + object.acceleration * timestamp * timestamp * 0.5f);
    }

    // resolve collision of two objects
    void resolve(physics_object &a, physics_object &b, float inegration_step) {
        for (auto triangleA : a.model.faces) {
            // std::cout << "\t\tfirst loop ";
            auto vecA1 = std::get<0>(triangleA);
            auto vecA2 = std::get<1>(triangleA);
            auto vecA3 = std::get<2>(triangleA);
            // show_vec3(vecA1);
            // std::cout << '\t';
            // show_vec3(vecA2);
            // std::cout << '\t';
            // show_vec3(vecA3);
            // std::cout << '\t';

            for (auto triangleB : b.model.faces) {
                // std::cout << "\tsecond loop ";
                float dist1 = glm::length((a.position + vecA1) -
                                          (b.position + std::get<0>(triangleB)));
                float dist2 = glm::length((a.position + vecA2) -
                                          (b.position + std::get<1>(triangleB)));
                float dist3 = glm::length((a.position + vecA3) -
                                          (b.position + std::get<2>(triangleB)));
                // std::cout << dist1 << " " << dist2 << " " << dist3 << std::endl;

                if (dist1 <= 0 || dist2 <= 0 || dist3 <= 0) { // COLLISION
                    std::cout << "==============collision==================\n";
                    // teta = ну, от центра масс до отчки соприкосновения вектор
                    a.velocity =
                        a.velocity - (1 + a.elasticity) * (b.mass / (a.mass + b.mass) *
                                                           (a.velocity - b.velocity));
                    b.velocity =
                        b.velocity - (1 + b.elasticity) * (a.mass / (a.mass + b.mass) *
                                                           (a.velocity - b.velocity));
                    a.velocity = -a.velocity;
                    b.velocity = -b.velocity;
                    return;
                }
            }
        }
    }
    inline void _show_vec3(glm::vec3 &v) {
        std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    }
} // namespace physics
