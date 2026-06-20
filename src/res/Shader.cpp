#include "Shader.hpp"

#include <cstdio>
#include <vector>

namespace ui {

Shader::~Shader() {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_program(other.m_program)
    , m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_program = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_program) glDeleteProgram(m_program);
        m_program = other.m_program;
        other.m_program = 0;
        m_uniformCache = std::move(other.m_uniformCache);
    }
    return *this;
}

GLuint Shader::compileStage(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(static_cast<size_t>(len) + 1);
        glGetShaderInfoLog(shader, len, nullptr, log.data());
        fprintf(stderr, "Shader compilation error (%s):\n%s\n",
                type == GL_VERTEX_SHADER ? "vertex" : "fragment", log.data());
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

bool Shader::compile(const char* vertexSrc, const char* fragmentSrc) {
    if (m_program) {
        glDeleteProgram(m_program);
        m_program = 0;
    }
    m_uniformCache.clear();

    GLuint vs = compileStage(GL_VERTEX_SHADER, vertexSrc);
    if (!vs) return false;

    GLuint fs = compileStage(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fs) {
        glDeleteShader(vs);
        return false;
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vs);
    glAttachShader(m_program, fs);
    glLinkProgram(m_program);

    GLint success = 0;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        GLint len = 0;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &len);
        std::vector<char> log(static_cast<size_t>(len) + 1);
        glGetProgramInfoLog(m_program, len, nullptr, log.data());
        fprintf(stderr, "Shader link error:\n%s\n", log.data());
        glDeleteProgram(m_program);
        m_program = 0;
    }

    // Shaders are linked, can detach and delete intermediates
    glDetachShader(m_program, vs);
    glDetachShader(m_program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return m_program != 0;
}

void Shader::bind() const {
    glUseProgram(m_program);
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLint Shader::uniformLocation(const char* name) const {
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) return it->second;
    GLint loc = glGetUniformLocation(m_program, name);
    m_uniformCache[name] = loc;
    return loc;
}

void Shader::setMat4(const char* name, const float* data) const {
    GLint loc = uniformLocation(name);
    if (loc != -1) glUniformMatrix4fv(loc, 1, GL_FALSE, data);
}

void Shader::setInt(const char* name, int value) const {
    GLint loc = uniformLocation(name);
    if (loc != -1) glUniform1i(loc, value);
}

} // namespace ui
