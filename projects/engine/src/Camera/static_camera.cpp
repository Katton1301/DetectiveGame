#include <camera/static_camera.hpp>

const GLfloat YAW        = -90.0f;
const GLfloat PITCH      =  0.0f;
const GLfloat SPEED      =  1.0f;
const GLfloat SENSITIVTY =  0.25f;
const GLfloat ZOOM       =  45.0f;
const GLfloat VISUAL_RANGE_MIN = 0.1f;
const GLfloat VISUAL_RANGE_MAX = 100.0f;

TStaticCamera::TStaticCamera()
    : TStaticCamera(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f)
{
}

TStaticCamera::TStaticCamera(glm::vec3 position, GLfloat windowRatio)
    : TStaticCamera(
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

TStaticCamera::TStaticCamera(
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

TStaticCamera::TStaticCamera(
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
    m_viewMatrix = glm::mat4(1.0f);
    recalcProjection();
}

glm::mat4 const & TStaticCamera::ViewMatrix()
{
    return m_viewMatrix;
}

void TStaticCamera::setWindowRatio(GLfloat _windowRatio)
{
    m_windowRatio = _windowRatio;
    recalcProjection();
}

void TStaticCamera::updateCameraVectors()
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

void TStaticCamera::recalcProjection()
{
    *m_projectionPtr =
        glm::perspective(Zoom(), m_windowRatio, m_visual_range_min, m_visual_range_max);
}
