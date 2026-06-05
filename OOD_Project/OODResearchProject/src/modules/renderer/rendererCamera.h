#pragma once
#include "rendererData.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace renderer {

    void CameraSetZoom(Camera2D* cam, float zoom) {
        cam->zoom = std::max(0.05f, zoom);
    }

    glm::mat4 CameraViewMatrix(Camera2D* cam) {
        glm::vec3 half_view(
            cam->viewport_size[0] * 0.5f,
            cam->viewport_size[1] * 0.5f,
            0.0f
        );

        glm::mat4 to_screen_center = glm::translate(glm::mat4(1.0f), half_view);
        glm::mat4 zoom = glm::scale(glm::mat4(1.0f), glm::vec3(cam->zoom, cam->zoom, 1.0f));
        glm::mat4 move_world = glm::translate(glm::mat4(1.0f), glm::vec3(-cam->position[0], -cam->position[1], 0.0f));

        return to_screen_center * zoom * move_world;
    }

    glm::mat4 CameraProjMatrix(Camera2D* cam) {
        return glm::ortho(0.0f, float(cam->viewport_size[0]), float(cam->viewport_size[1]), 0.0f, -1.0f, 1.0f);
    }

    glm::mat4 CameraViewProjMatrix(Camera2D* cam) {
        return CameraProjMatrix(cam) * CameraViewMatrix(cam);
    }
}