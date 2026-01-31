#include "camera.hpp"

const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  1.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;
const GLfloat VISUAL_RANGE_MIN = 0.1f;
const GLfloat VISUAL_RANGE_MAX = 100.0f;

TCamera::TCamera()
    : TCamera(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f)
{
}

TCamera::TCamera(glm::vec3 position, GLfloat windowRatio)
    : TCamera(
        position,
        windowRatio,
        glm::vec3(0.0f, 1.0f, 0.0f),
        YAW,
        PITCH,
        VISUAL_RANGE_MIN,
        VISUAL_RANGE_MAX
    )
{

}

TCamera::TCamera(
        glm::vec3 position,
        GLfloat windowRatio,
        glm::vec3 up,
        GLfloat yaw,
        GLfloat pitch,
        GLfloat visual_range_min,
        GLfloat visual_range_max
    )
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_movementSpeed(SPEED)
    , m_mouseSensitivity(SENSITIVTY)
    , m_zoom(ZOOM)
{
    m_position = position;
    m_windowRatio = windowRatio;
    m_worldUp = up;
    m_yaw = yaw;
    m_pitch = pitch;
    m_visual_range_min = visual_range_min;
    m_visual_range_max = visual_range_max;
    updateCameraVectors();
    m_projectionPtr = std::make_shared<glm::mat4>();
    recalcProjection();
}

TCamera::TCamera(
    GLfloat posX,
    GLfloat posY,
    GLfloat posZ,
    GLfloat upX,
    GLfloat upY,
    GLfloat upZ,
    GLfloat windowRatio,
    GLfloat yaw,
    GLfloat pitch,
    GLfloat visual_range_min,
    GLfloat visual_range_max
)
    : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
    , m_movementSpeed(SPEED)
    , m_mouseSensitivity(SENSITIVTY)
    , m_zoom(ZOOM)
{
    m_position = glm::vec3(posX, posY, posZ);
    m_worldUp = glm::vec3(upX, upY, upZ);
    m_windowRatio = windowRatio;
    m_yaw = yaw;
    m_pitch = pitch;
    m_visual_range_min = visual_range_min;
    m_visual_range_max = visual_range_max;
    updateCameraVectors();
    m_projectionPtr = std::make_shared<glm::mat4>();
    recalcProjection();
}

glm::mat4 TCamera::GetViewMatrix()
{
    return glm::lookAt(m_position, m_position + m_front, m_up);
}

void TCamera::ProcessKeyboard(TCameraMovement direction, GLfloat deltaTime)
{
    GLfloat velocity = m_movementSpeed * deltaTime;
    if (direction == FORWARD)
        m_position += m_front * velocity;
    if (direction == BACKWARD)
        m_position -= m_front * velocity;
    if (direction == LEFT)
        m_position -= m_right * velocity;
    if (direction == RIGHT)
        m_position += m_right * velocity;
}

void TCamera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch /*= true*/)
{
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw   += xoffset;
    m_pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Eular angles
    updateCameraVectors();
}

void TCamera::ProcessMouseScroll(GLfloat yoffset)
{
    if (m_zoom >= 1.0f && m_zoom <= 45.0f)
        m_zoom -= yoffset;
    if (m_zoom <= 1.0f)
        m_zoom = 1.0f;
    if (m_zoom >= 45.0f)
        m_zoom = 45.0f;
}

void TCamera::setWindowRatio(GLfloat _windowRatio)
{
    m_windowRatio = _windowRatio;
    recalcProjection();
}
void TCamera::setVisualRangeMin(GLfloat _min)
{
    m_visual_range_min = _min;
    recalcProjection();
}

void TCamera::setVisualRangeMax(GLfloat _max)
{
    m_visual_range_max = _max;
    recalcProjection();
}

void TCamera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // Also re-calculate the Right and Up vector
    m_right = glm::normalize(glm::cross(m_front, m_worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    m_up    = glm::normalize(glm::cross(m_right, m_front));
}

void TCamera::recalcProjection()
{
    *m_projectionPtr =
        glm::perspective(Zoom(), m_windowRatio, m_visual_range_min, m_visual_range_max);
}
