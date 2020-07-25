#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

float Camera::get_zoom() const
{
    return fZoom;
}

glm::mat4 Camera::get_view_matrix() const
{
    glm::vec3 curPos(position);
    curPos.x = curPos.x - fPanSpeed * fXPanTotal;
    curPos.y = curPos.y + fPanSpeed * fYPanTotal;
    curPos = curPos + fScrollTotal * fZoomSpeed * direction;
    return glm::lookAt(curPos, curPos + direction, up);
}

void Camera::process_mouse_scroll(float fDelta)
{
    if (fScrollTotal >= MIN_ZOOM && fScrollTotal <= MAX_ZOOM)
        fScrollTotal += fDelta;
    if (fScrollTotal <= MIN_ZOOM)
        fScrollTotal = MIN_ZOOM;
    if (fScrollTotal >= MAX_ZOOM)
        fScrollTotal = MAX_ZOOM;
}

void Camera::process_mouse_right_movement(float fXPos, float fYPos)
{
    if (!bIsRDrag) return;
    fXPanTotal += fXPos - fXPosLast_R;
    fYPanTotal += fYPos - fYPosLast_R;
    fXPosLast_R = fXPos;
    fYPosLast_R = fYPos;
}

void Camera::process_mouse_right_click(float fXPos, float fYPos)
{
    bIsRDrag = true;
    fXPosLast_R = fXPos;
    fYPosLast_R = fYPos;
}

void Camera::process_mouse_right_release(float fXPos, float fYPos)
{
    fXPanTotal += fXPos - fXPosLast_R;
    fYPanTotal += fYPos - fYPosLast_R;
    bIsRDrag = false;
}
