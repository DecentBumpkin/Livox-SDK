#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

const glm::vec3 DEFAULT_CAM_POSITION  = glm::vec3(0.0f, 0.0f, 50.0f);
const glm::vec3 DEFAULT_CAM_DIRECTION = glm::vec3(0.0f, 0.0f, -1.0f);
const glm::vec3 DEFAULT_CAM_UP        = glm::vec3(0.0f, 1.0f, 0.0f);
const float     DEFAULT_ZOOM          = 100.0f;
const float     DEFAULT_ZOOM_SPEED    = 0.06f;
const float     DEFAULT_PAN_SPEED     = 0.03f;
const float     MIN_ZOOM              = -400.0f;
const float     MAX_ZOOM              = 400.0f;

class Camera
{
public:
    Camera(glm::vec3 camPos        = DEFAULT_CAM_POSITION,
           glm::vec3 camDir        = DEFAULT_CAM_DIRECTION,
           glm::vec3 camUp         = DEFAULT_CAM_UP,
           float     fCamZoom      = DEFAULT_ZOOM,
           float     fCamZoomSpeed = DEFAULT_ZOOM_SPEED,
           float     fCamPanSpeed  = DEFAULT_PAN_SPEED)
        : position(camPos), direction(camDir), up(camUp),
          fZoom(fCamZoom), fZoomSpeed(fCamZoomSpeed), fPanSpeed(fCamPanSpeed),
          fXPanTotal(0), fYPanTotal(0), fScrollTotal(0), bIsRDrag(false) {}

    float get_zoom() const;
    glm::mat4 get_view_matrix() const;
    void process_mouse_right_click(float fXPos, float fYPos);
    void process_mouse_right_release(float fXPos, float fYPos);
    void process_mouse_right_movement(float fXPos, float fYPos);
    void process_mouse_scroll(float fDelta);

private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;
    float fZoom;
    float fZoomSpeed;
    float fPanSpeed;

    float fXPanTotal, fYPanTotal;
    float fXPosLast_R, fYPosLast_R;
    float fScrollTotal;
    bool bIsRDrag;
};

#endif // CAMERA_H
