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

    // Initialize GL state cache
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&m_boundProgram);
    m_boundVAO = 0;
    m_boundTexture = m_whiteTexture;
    m_scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
    m_blendEnabled = glIsEnabled(GL_BLEND);
    if (!m_blendEnabled) { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); m_blendEnabled = true; }

    return true;
}

void QuadBatcher::shutdown() {
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
    if (m_whiteTexture) { glDeleteTextures(1, &m_whiteTexture); m_whiteTexture = 0; }
    m_shader.reset(); m_roundedShader.reset(); m_vertices.clear();
    m_boundProgram = 0; m_boundVAO = 0; m_boundTexture = 0;
}

void QuadBatcher::createWhiteTexture() {
    unsigned char w[4] = {255,255,255,255};
    glGenTextures(1, &m_whiteTexture);
    m_boundTexture = m_whiteTexture;
    glBindTexture(GL_TEXTURE_2D, m_whiteTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, w);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_currentTexture = m_whiteTexture;
}

void QuadBatcher::setupVAO() {
    glGenVertexArrays(1, &m_vao); glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao); glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)8);
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)16);
    glBindVertexArray(0);
    m_boundVAO = 0;
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
    // Ensure blend is enabled
    if (!m_blendEnabled) { glEnable(GL_BLEND); m_blendEnabled = true; }
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

    GLuint prog = m_roundedShader->program();
    if (m_boundProgram != prog) { glUseProgram(prog); m_boundProgram = prog; }

    m_roundedShader->setMat4("uProj", &m_projection[0][0]);
    GLint rl = glGetUniformLocation(prog, "uRect");
    GLint al = glGetUniformLocation(prog, "uRadius");
    GLint cl = glGetUniformLocation(prog, "uColor");
    GLint fl = glGetUniformLocation(prog, "uFbSize");
    if (rl != -1) glUniform4f(rl, rect.x, rect.y, rect.width, rect.height);
    if (al != -1) glUniform1f(al, r);
    if (cl != -1) glUniform4f(cl, top.r, top.g, top.b, top.a);
    if (fl != -1) glUniform2f(fl, (float)rect.width+rect.x+2, (float)m_fbHeight);

    if (m_boundVAO != m_vao) { glBindVertexArray(m_vao); m_boundVAO = m_vao; }

    float x0 = rect.x-2, y0 = rect.y-2, x1 = rect.x+rect.width+2, y1 = rect.y+rect.height+2;
    QuadVertex q[6] = {
        {x0,y0,0,0,1,1,1,1},{x1,y0,1,0,1,1,1,1},{x1,y1,1,1,1,1,1,1},
        {x0,y0,0,0,1,1,1,1},{x1,y1,1,1,1,1,1,1},{x0,y1,0,1,1,1,1,1},
    };
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(q), q, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    m_drawCallCount++;

    m_boundProgram = 0; glUseProgram(0);
}

void QuadBatcher::bindTexture(GLuint t) {
    if (t == 0) t = m_whiteTexture;
    if (m_currentTexture != t) { flush(); m_currentTexture = t; }
}

void QuadBatcher::flush() {
    if (m_vertices.empty() || !m_shader) return;

    // State caching: only change what's needed (EUI-NEO pattern)
    GLuint prog = m_shader->program();
    if (m_boundProgram != prog) { glUseProgram(prog); m_boundProgram = prog; }

    m_shader->setMat4("uProj", &m_projection[0][0]);
    m_shader->setInt("uTex", 0);

    if (m_boundTexture != m_currentTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_currentTexture);
        m_boundTexture = m_currentTexture;
    }

    if (m_boundVAO != m_vao) { glBindVertexArray(m_vao); m_boundVAO = m_vao; }

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(m_vertices.size()*sizeof(QuadVertex)), m_vertices.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)m_vertices.size());
    m_drawCallCount++;
    m_flushCount++;
    m_vertices.clear();
}

void QuadBatcher::pushClip(const Rect& rect, int fbH) {
    flush();
    // Save current state onto stack
    m_clipStack.push_back({m_scissorX, m_scissorY, m_scissorW, m_scissorH, m_scissorEnabled});

    // Compute new scissor rect (in OpenGL framebuffer coords)
    GLint sx = (GLint)rect.x;
    GLint sy = fbH - (GLint)(rect.y + rect.height);
    GLsizei sw = (GLsizei)rect.width;
    GLsizei sh = (GLsizei)rect.height;

    // Intersect with current scissor if already enabled
    if (m_scissorEnabled) {
        GLint rx = (std::max)(sx, m_scissorX);
        GLint ry = (std::max)(sy, m_scissorY);
        GLint rr = (std::min)(sx + sw, m_scissorX + m_scissorW);
        GLint rb = (std::min)(sy + sh, m_scissorY + m_scissorH);
        sx = rx; sy = ry;
        sw = (std::max)(0, rr - rx);
        sh = (std::max)(0, rb - ry);
    }

    glEnable(GL_SCISSOR_TEST);
    m_scissorEnabled = true;
    glScissor(sx, sy, sw, sh);
    m_scissorX = sx; m_scissorY = sy; m_scissorW = sw; m_scissorH = sh;
}

void QuadBatcher::popClip(int) {
    flush();
    if (m_clipStack.empty()) {
        if (m_scissorEnabled) { glDisable(GL_SCISSOR_TEST); m_scissorEnabled = false; }
        return;
    }
    // Restore previous state
    ClipState prev = m_clipStack.back();
    m_clipStack.pop_back();
    if (prev.enabled) {
        glEnable(GL_SCISSOR_TEST);
        m_scissorEnabled = true;
        glScissor(prev.x, prev.y, prev.w, prev.h);
        m_scissorX = prev.x; m_scissorY = prev.y;
        m_scissorW = prev.w; m_scissorH = prev.h;
    } else {
        glDisable(GL_SCISSOR_TEST);
        m_scissorEnabled = false;
    }
}

} // namespace ui
