#ifndef TRACKBALL_H
#define TRACKBALL_H

#include <glm/glm.hpp>

class TrackBall
{
public:
    TrackBall(int nScreenWidth, int nScreenHeight)
        : nWidth(nScreenWidth), nHeight(nScreenHeight), bIsLDrag(0), rotation(glm::mat4(1.0f)) {}

    TrackBall() : bIsLDrag(0), rotation(glm::mat4(1.0f)) {}

    glm::mat4 get_rotation_matrix() const;
    void process_mouse_left_click(float fXPos, float fYPos);
    void process_mouse_left_release(float fXPos, float fYPos);
    void process_mouse_left_movement(float fXPos, float fYPos);
    void update_screen_size(int nScreenWidth, int nScreenHeight);

private:
    glm::vec3 start, cur;
    int nWidth, nHeight;
    bool bIsLDrag;
    glm::mat4 rotation;

    glm::mat4 cal_cur_rot() const;
};

#endif // TRACKBALL_H
