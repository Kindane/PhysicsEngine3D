#pragma once
#include <tuple>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>
#include "stl_reader.hpp"

namespace physics {
    using triangle = std::tuple<glm::vec3, glm::vec3, glm::vec3>;
    inline void show_vec3(glm::vec3& v);


    struct physics_model {
        std::vector<triangle> faces;
    };

    struct physics_object {
        physics_model &model;
        glm::vec3 velocity;
        glm::vec3 position;
        glm::quat rotate;
        glm::quat angleVelocity;
        float mass;
        float elasticity;
        float acceleration;
        public:
            physics_object(physics_model& model, glm::vec3 position,
            glm::vec3 velocity, glm::quat rotate, glm::quat angleVelocity, float mass, float elasticity, float acceleration): model(model) {
                this->position = position;
                this->velocity = velocity;
                this->rotate = rotate;
                this->angleVelocity = angleVelocity;
                this->mass = mass;
                this->elasticity = elasticity;
                this->acceleration = acceleration;
            }
    };

    void load_model(physics::physics_model &model, const char *path) {
        stl_reader::StlMesh<float, unsigned int> mesh(path);
        for (size_t itri = 0; itri < mesh.num_tris(); ++itri) {
            for (size_t icorner = 0; icorner < 3; ++icorner) {
                const float *c = mesh.tri_corner_coords(itri, icorner);
                // or alternatively:
                //const float* c = mesh.vrt_coords (mesh.tri_corner_ind (itri, icorner));
                physics::triangle temp(c[0], c[1], c[2]);
                model.faces.push_back(temp);
                //std::cout << "(" << c[0] << ", " << c[1] << ", " << c[2] << ") ";
            }
        }
    }

    void advance_step(physics_object& object, float timestamp) {
        object.position += object.velocity;
    }

    void resolve(physics_object &a, physics_object &b, float inegration_step) {
        for (auto triangleA: a.model.faces) {
            //std::cout << "\t\tfirst loop ";
            auto vecA1 = std::get<0>(triangleA);
            auto vecA2 = std::get<1>(triangleA);
            auto vecA3 = std::get<2>(triangleA);
            //show_vec3(vecA1);
            //std::cout << '\t';
            //show_vec3(vecA2);
            //std::cout << '\t';
            //show_vec3(vecA3);
            //std::cout << '\t';


            for (auto triangleB: b.model.faces) {
                //std::cout << "\tsecond loop ";
                float dist1 = glm::length((a.position + vecA1) - (b.position + std::get<0>(triangleB)));
                float dist2 = glm::length((a.position + vecA2) - (b.position + std::get<1>(triangleB)));
                float dist3 = glm::length((a.position + vecA3) - (b.position + std::get<2>(triangleB)));
                //std::cout << dist1 << " " << dist2 << " " << dist3 << std::endl;

                if (dist1 <= 0 || dist2 <= 0 || dist3 <= 0) { // COLLISION
                    std::cout << "==============collision==================\n";
                    // teta = ну, от центра масс до отчки соприкосновения вектор
                    a.velocity = a.velocity - (1 + a.elasticity)*(b.mass/(a.mass+b.mass)*(a.velocity-b.velocity));
                    b.velocity = b.velocity - (1 + b.elasticity)*(a.mass/(a.mass+b.mass)*(a.velocity-b.velocity));
                    a.velocity = -a.velocity;
                    b.velocity = -b.velocity;
                    return;
                }
            }
        }
    }
    inline void show_vec3(glm::vec3& v) {
        std::cout << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    }
}