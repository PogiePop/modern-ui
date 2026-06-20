#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace ui {

class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;

    bool compile(const char* vertexSrc, const char* fragmentSrc);
    void bind() const;
    void unbind() const;

    GLint uniformLocation(const char* name) const;
    void setMat4(const char* name, const float* data) const;
    void setInt(const char* name, int value) const;

    GLuint program() const { return m_program; }
    bool valid() const { return m_program != 0; }

private:
    GLuint m_program = 0;

    GLuint compileStage(GLenum type, const char* src);
    mutable std::unordered_map<std::string, GLint> m_uniformCache;
};

} // namespace ui
