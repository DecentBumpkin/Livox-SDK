#include "trackball.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

glm::mat4 TrackBall::get_rotation_matrix() const
{
    if (bIsLDrag)
        return cal_cur_rot() * rotation;
    else
        return rotation;
}

void TrackBall::process_mouse_left_click(float fXPos, float fYPos)
{
    bIsLDrag = true;
    start.x = fXPos / (nWidth / 2) - 1.0f;
    start.y = -(fYPos / (nHeight / 2) - 1.0f);
    if ((start.x * start.x + start.y * start.y) > 1.0f)
        start.z = 0.0f;
    else
        start.z = sqrt(1 - (start.x * start.x + start.y * start.y));

    cur.x = start.x;
    cur.y = start.y;
    cur.z = start.z;
}

void TrackBall::process_mouse_left_release(float fXPos, float fYPos)
{
    bIsLDrag = false;
    rotation = cal_cur_rot() * rotation;
}

void TrackBall::process_mouse_left_movement(float fXPos, float fYPos)
{
    if (!bIsLDrag) return;
    cur.x = fXPos / (nWidth / 2) - 1.0f;
    cur.y = -(fYPos / (nHeight / 2) - 1.0f);
    if ((cur.x * cur.x + cur.y * cur.y) > 1.0f)
        cur.z = 0.0f;
    else
        cur.z = sqrt(1 - (cur.x * cur.x + cur.y * cur.y));
}

glm::mat4 TrackBall::cal_cur_rot() const
{
    glm::mat4 curRot(1.0f);
    if (start == cur)
        return curRot;

    glm::vec3 rotAxis = glm::normalize(glm::cross(start, cur));
    float fCosA = glm::dot(start, cur) / (glm::length(start) * glm::length(cur));
    fCosA = (fCosA > 1.0f) ? 1.0f : fCosA;
    float fA = acos(fCosA);
    curRot = glm::rotate(curRot, fA, rotAxis);
    return curRot;
}

void TrackBall::update_screen_size(int nScreenWidth, int nScreenHeight)
{
    nWidth = nScreenWidth;
    nHeight = nScreenHeight;
}
