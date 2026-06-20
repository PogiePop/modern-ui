#include "QuadBatcher.hpp"
#include "res/Shader.hpp"
#include <glad/glad.h>
#include <array>
#include <cmath>
#include <cstdio>

namespace ui {

static const char* kVertexSrc = R"(#version 330 core
uniform mat4 uProj;
in vec2 aPos; in vec2 aUV; in vec4 aCol;
out vec2 vUV; out vec4 vCol;
void main() {
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
    vUV = aUV; vCol = aCol;
}
)";

static const char* kFragSrc = R"(#version 330 core
uniform sampler2D uTex;
in vec2 vUV; in vec4 vCol;
out vec4 oColor;
void main() {
    oColor = vCol * texture(uTex, vUV);
}
)";

static const char* kRoundedFrag = R"(#version 330 core
uniform vec4 uRect; uniform float uRadius;
uniform vec4 uColor; uniform vec2 uFbSize;
out vec4 oColor;

float sdRoundBox(vec2 p, vec2 h, float r) {
    vec2 d = abs(p) - h + r;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r;
}

void main() {
    vec2 p = vec2(gl_FragCoord.x, uFbSize.y - gl_FragCoord.y);
    vec2 c = uRect.xy + uRect.zw * 0.5;
    float d = sdRoundBox(p - c, uRect.zw * 0.5, uRadius);
    float a = 1.0 - smoothstep(-1.0, 1.0, d);
    oColor = uColor * vec4(1,1,1,a);
}
)";

QuadBatcher::QuadBatcher() = default;
QuadBatcher::~QuadBatcher() { shutdown(); }

bool QuadBatcher::init() {
    Shader s;
    if (!s.compile(kVertexSrc, kFragSrc)) { fprintf(stderr, "QuadBatcher: main shader failed\n"); return false; }
    m_shader = std::make_unique<Shader>(std::move(s));

    Shader rs;
    if (rs.compile(kVertexSrc, kRoundedFrag)) m_roundedShader = std::make_unique<Shader>(std::move(rs));

    createWhiteTexture();
    setupVAO();
    m_vertices.reserve(4096);
    return true;
}

void QuadBatcher::shutdown() {
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_whiteTexture) { glDeleteTextures(1, &m_whiteTexture); m_whiteTexture = 0; }
    m_shader.reset(); m_roundedShader.reset(); m_vertices.clear();
}

void QuadBatcher::createWhiteTexture() {
    unsigned char w[4] = {255,255,255,255};
    glGenTextures(1, &m_whiteTexture); glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, w);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0); m_currentTexture = m_whiteTexture;
}

void QuadBatcher::setupVAO() {
    glGenVertexArrays(1, &m_vao); glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)8);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)16);
    glBindVertexArray(0);
}

static void vpush(std::vector<QuadVertex>& v, float x, float y, float u, float vv, float r, float g, float b, float a) {
    v.push_back({x,y,u,vv,r,g,b,a});
}

void QuadBatcher::addQuad(const Rect& r, float u0, float v0, float u1, float v1, float cr, float cg, float cb, float ca) {
    float x1 = r.x + r.width, y1 = r.y + r.height;
    vpush(m_vertices, r.x, r.y, u0, v0, cr, cg, cb, ca);
    vpush(m_vertices,  x1, r.y, u1, v0, cr, cg, cb, ca);
    vpush(m_vertices,  x1,  y1, u1, v1, cr, cg, cb, ca);
    vpush(m_vertices, r.x, r.y, u0, v0, cr, cg, cb, ca);
    vpush(m_vertices,  x1,  y1, u1, v1, cr, cg, cb, ca);
    vpush(m_vertices, r.x,  y1, u0, v1, cr, cg, cb, ca);
}

void QuadBatcher::begin(const glm::mat4& p) {
    m_projection = p; m_currentTexture = m_whiteTexture; m_vertices.clear();
    if (p[1][1] != 0) m_fbHeight = (int)(2.0f / -p[1][1]);
}

void QuadBatcher::end() { flush(); }

void QuadBatcher::drawRect(const Rect& r, const Color& c) {
    bindTexture(m_whiteTexture); addQuad(r, 0,0,1,1, c.r,c.g,c.b,c.a);
}

void QuadBatcher::drawGradientRect(const Rect& r, const Color& top, const Color& bot) {
    bindTexture(m_whiteTexture);
    float x1 = r.x + r.width, y1 = r.y + r.height;
    vpush(m_vertices, r.x, r.y, 0,0, top.r,top.g,top.b,top.a);
    vpush(m_vertices,  x1, r.y, 1,0, top.r,top.g,top.b,top.a);
    vpush(m_vertices,  x1,  y1, 1,1, bot.r,bot.g,bot.b,bot.a);
    vpush(m_vertices, r.x, r.y, 0,0, top.r,top.g,top.b,top.a);
    vpush(m_vertices,  x1,  y1, 1,1, bot.r,bot.g,bot.b,bot.a);
    vpush(m_vertices, r.x,  y1, 0,1, bot.r,bot.g,bot.b,bot.a);
}

void QuadBatcher::drawTexturedRect(const Rect& d, float u0, float v0, float u1, float v1, const Color& t, GLuint tex) {
    bindTexture(tex); addQuad(d, u0,v0,u1,v1, t.r,t.g,t.b,t.a);
}

void QuadBatcher::drawRoundedRect(const Rect& r, const Color& c, float rad, int) {
    drawRoundedGradientRect(r, c, c, rad, 0);
}

void QuadBatcher::drawRoundedGradientRect(const Rect& rect, const Color& top, const Color& bot, float radius, int) {
    if (radius <= 0.5f || !m_roundedShader) { drawGradientRect(rect, top, bot); return; }
    float r = radius;
    if (r*2 > rect.width) r = rect.width*0.5f;
    if (r*2 > rect.height) r = rect.height*0.5f;
    flush();

    m_roundedShader->bind();
    m_roundedShader->setMat4("uProj", &m_projection[0][0]);
    GLint rl = glGetUniformLocation(m_roundedShader->program(), "uRect");
    GLint al = glGetUniformLocation(m_roundedShader->program(), "uRadius");
    GLint cl = glGetUniformLocation(m_roundedShader->program(), "uColor");
    GLint fl = glGetUniformLocation(m_roundedShader->program(), "uFbSize");
    if (rl != -1) glUniform4f(rl, rect.x, rect.y, rect.width, rect.height);
    if (al != -1) glUniform1f(al, r);
    if (cl != -1) glUniform4f(cl, top.r, top.g, top.b, top.a);
    if (fl != -1) glUniform2f(fl, (float)rect.width+rect.x+2, (float)m_fbHeight);

    float x0 = rect.x-2, y0 = rect.y-2, x1 = rect.x+rect.width+2, y1 = rect.y+rect.height+2;
    QuadVertex q[6] = {
        {x0,y0,0,0,1,1,1,1},{x1,y0,1,0,1,1,1,1},{x1,y1,1,1,1,1,1,1},
        {x0,y0,0,0,1,1,1,1},{x1,y1,1,1,1,1,1,1},{x0,y1,0,1,1,1,1,1},
    };
    glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(q), q, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0); glUseProgram(0);
}

void QuadBatcher::bindTexture(GLuint t) {
    if (t == 0) t = m_whiteTexture;
    if (m_currentTexture != t) { flush(); m_currentTexture = t; }
}

void QuadBatcher::flush() {
    if (m_vertices.empty() || !m_shader) return;
    m_shader->bind();
    m_shader->setMat4("uProj", &m_projection[0][0]);
    m_shader->setInt("uTex", 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_currentTexture);
    glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(m_vertices.size()*sizeof(QuadVertex)), m_vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());
    glBindVertexArray(0); glBindTexture(GL_TEXTURE_2D, 0); glUseProgram(0);
    m_vertices.clear();
}

void QuadBatcher::pushClip(const Rect& rect, int fbH) {
    flush(); glEnable(GL_SCISSOR_TEST);
    GLint sy = fbH - (GLint)(rect.y + rect.height);
    glScissor((GLint)rect.x, sy, (GLsizei)rect.width, (GLsizei)rect.height);
}
void QuadBatcher::popClip(int) { flush(); glDisable(GL_SCISSOR_TEST); }

} // namespace ui
