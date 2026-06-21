// EUI-NEO: Container is pure layout — Row/Column/Stack. ScrollArea handles scrolling.
#include "Container.hpp"
#include "Widget.hpp"
#include "core/Painter.hpp"
#include "core/Event.hpp"
#include "res/Theme.hpp"
#include <algorithm>
#include <cmath>
#include <functional>

namespace ui {

namespace { using ChildLayoutInfo = Container::ChildLayoutInfo;
const ChildLayoutInfo& getLayout(const std::vector<ChildLayoutInfo>& l, size_t i) {
    if (i < l.size()) return l[i]; static ChildLayoutInfo d; return d;
}}

Container::Container(LayoutDirection dir) : m_direction(dir) {}
void Container::setDirection(LayoutDirection d) { m_direction = d; }
void Container::setBackgroundColor(const Color& c) { m_bgColor = c; }
void Container::clearBackgroundColor() { m_bgColor.reset(); }
Color Container::backgroundColor() const { return m_bgColor.value_or(Color{0,0,0,0}); }
void Container::setChildSizing(size_t i, Sizing h, Sizing v) {
    if (i>=m_childLayout.size()) m_childLayout.resize(i+1); m_childLayout[i]={h,v};
}

Size Container::measure(const Size& a) const {
    float w=0,h=0;
    if (m_children.empty()) { w=m_padding.horizontal(); h=m_padding.vertical(); }
    else if (m_direction==LayoutDirection::Horizontal) {
        float tw=m_padding.horizontal(),mh=0; int n=0;
        for (auto& c:m_children) if (c->visible()) n++;
        float ts=m_spacing*(n>0?n-1:0),ac=a.width-m_padding.horizontal()-ts; if(ac<0)ac=0;if(n==0)n=1;
        for (auto& c:m_children){if(!c->visible())continue;
            Size s=c->measure({ac/n,a.height-m_padding.vertical()});if(s.width<c->minWidth())s.width=c->minWidth();
            if(s.height<c->minHeight())s.height=c->minHeight();if(s.width>c->maxWidth())s.width=c->maxWidth();
            if(s.height>c->maxHeight())s.height=c->maxHeight();tw+=s.width;if(s.height>mh)mh=s.height;}
        w=tw+ts;h=mh+m_padding.vertical();
    } else {
        float th=m_padding.vertical(),mw=0; int n=0;
        for (auto& c:m_children) if (c->visible()) n++;
        float ts=m_spacing*(n>0?n-1:0),ac=a.height-m_padding.vertical()-ts; if(ac<0)ac=0;if(n==0)n=1;
        for (auto& c:m_children){if(!c->visible())continue;
            Size s=c->measure({a.width-m_padding.horizontal(),ac/n});if(s.width<c->minWidth())s.width=c->minWidth();
            if(s.height<c->minHeight())s.height=c->minHeight();if(s.width>c->maxWidth())s.width=c->maxWidth();
            if(s.height>c->maxHeight())s.height=c->maxHeight();if(s.width>mw)mw=s.width;th+=s.height;}
        w=mw+m_padding.horizontal();h=th+ts;
    }
    if(w<m_minWidth)w=m_minWidth;if(h<m_minHeight)h=m_minHeight;
    if(w>m_maxWidth)w=m_maxWidth;if(h>m_maxHeight)h=m_maxHeight;
    return {w,h};
}

void Container::layout() {
    if (m_responsive&&m_bounds.width>0){float s=std::clamp(m_bounds.width/1280.0f,0.5f,1.5f);
        m_padding={m_basePadding.top*s,m_basePadding.right*s,m_basePadding.bottom*s,m_basePadding.left*s};m_spacing=m_baseSpacing*s;}

    float cx=m_padding.left,cy=m_padding.top;
    float cw=m_bounds.width-m_padding.horizontal(),ch=m_bounds.height-m_padding.vertical();
    if(cw<0)cw=0;if(ch<0)ch=0;
    if (m_children.empty()) return;

    // Pre-measure all visible children to avoid redundant measure() calls
    struct ChildMeasure { float primary=0; float cross=0; };
    std::vector<ChildMeasure> measures(m_children.size());

    if (m_direction==LayoutDirection::Horizontal) {
        float ts=m_spacing*(m_children.size()-1),uw=ts,tf=0;std::vector<float>ws(m_children.size(),0);
        for(size_t i=0;i<m_children.size();++i){auto&c=m_children[i];if(!c->visible())continue;
            auto&lo=getLayout(m_childLayout,i);
            if(lo.horz.type==SizingType::Fixed){ws[i]=lo.horz.value;uw+=ws[i];}
            else if(lo.horz.type==SizingType::Intrinsic){
                Size ms=c->measure({cw,ch});
                measures[i]={ms.width,ms.height};
                ws[i]=ms.width;uw+=ws[i];}
            else if(lo.horz.type==SizingType::Fill)tf+=lo.horz.value;}
        float rem=cw-uw;if(rem<0)rem=0;float eg=0,xg=0;
        if(tf==0&&m_children.size()>1){switch(m_align){
            case Align::SpaceBetween:xg=rem/(m_children.size()-1);rem=0;break;
            case Align::SpaceAround:xg=rem/m_children.size();eg=xg*0.5f;rem=0;break;
            case Align::SpaceEvenly:xg=rem/(m_children.size()+1);eg=xg;rem=0;break;
            case Align::Center:eg=rem*0.5f;rem=0;break;case Align::End:eg=rem;rem=0;break;default:break;}}
        float x=cx+eg;
        for(size_t i=0;i<m_children.size();++i){auto&c=m_children[i];if(!c->visible())continue;
            auto&lo=getLayout(m_childLayout,i);float cw2=ws[i];
            if(lo.horz.type==SizingType::Fill)cw2=tf>0?rem*lo.horz.value/tf:0;
            float ch2=ch;
            if(lo.vert.type==SizingType::Fixed)ch2=lo.vert.value;
            else if(lo.vert.type==SizingType::Intrinsic){
                // Use cached cross measurement if available, otherwise measure
                if(measures[i].cross>0)ch2=std::min(measures[i].cross,ch);
                else ch2=std::min(c->measure({cw2,ch}).height,ch);}
            if(cw2<c->minWidth())cw2=c->minWidth();if(ch2<c->minHeight())ch2=c->minHeight();
            if(cw2>c->maxWidth())cw2=c->maxWidth();if(ch2>c->maxHeight())ch2=c->maxHeight();
            c->setBounds({x,cy,cw2,ch2});x+=cw2+m_spacing+xg;}
    } else {
        float ts=m_spacing*(m_children.size()-1),uh=ts,tf=0;std::vector<float>hs(m_children.size(),0);
        for(size_t i=0;i<m_children.size();++i){auto&c=m_children[i];if(!c->visible())continue;
            auto&lo=getLayout(m_childLayout,i);
            if(lo.vert.type==SizingType::Fixed){hs[i]=lo.vert.value;uh+=hs[i];}
            else if(lo.vert.type==SizingType::Intrinsic){
                Size ms=c->measure({cw,ch});
                measures[i]={ms.width,ms.height};
                hs[i]=ms.height;uh+=hs[i];}
            else if(lo.vert.type==SizingType::Fill)tf+=lo.vert.value;}
        float rem=ch-uh;if(rem<0)rem=0;float eg=0,xg=0;
        if(tf==0&&m_children.size()>1){switch(m_align){
            case Align::SpaceBetween:xg=rem/(m_children.size()-1);rem=0;break;
            case Align::SpaceAround:xg=rem/m_children.size();eg=xg*0.5f;rem=0;break;
            case Align::SpaceEvenly:xg=rem/(m_children.size()+1);eg=xg;rem=0;break;
            case Align::Center:eg=rem*0.5f;rem=0;break;case Align::End:eg=rem;rem=0;break;default:break;}}
        float y=cy+eg;
        for(size_t i=0;i<m_children.size();++i){auto&c=m_children[i];if(!c->visible())continue;
            auto&lo=getLayout(m_childLayout,i);float ch2=hs[i];
            if(lo.vert.type==SizingType::Fill)ch2=tf>0?rem*lo.vert.value/tf:0;
            float cw2=cw;
            if(lo.horz.type==SizingType::Fixed)cw2=lo.horz.value;
            else if(lo.horz.type==SizingType::Intrinsic){
                // Use cached cross measurement if available, otherwise measure
                if(measures[i].primary>0)cw2=std::min(measures[i].primary,cw);
                else cw2=std::min(c->measure({cw,ch2}).width,cw);}
            if(cw2<c->minWidth())cw2=c->minWidth();if(ch2<c->minHeight())ch2=c->minHeight();
            if(cw2>c->maxWidth())cw2=c->maxWidth();if(ch2>c->maxHeight())ch2=c->maxHeight();
            c->setBounds({cx,y,cw2,ch2});y+=ch2+m_spacing+xg;}
    }
    for (auto& c : m_children) c->layout();
}

void Container::paint(Painter& painter) {
    Rect r = screenRect();
    // Use explicit bg color if set, otherwise try theme
    if (m_bgColor.has_value()) {
        painter.drawRect(r, *m_bgColor);
    } else if (m_theme) {
        painter.drawRect(r, m_theme->color(ColorRole::Bg));
    }

    for (auto& child : m_children) {
        if (!child->visible()) continue;
        float ox=child->slideOffsetX(),oy=child->slideOffsetY();
        if(ox!=0||oy!=0) child->setPosition(child->bounds().x+ox, child->bounds().y+oy);
        if(child->hasShadow()&&child->opacity()>0.01f){Rect sr=child->screenRect();
            Color sc=child->shadowColor();sc.a*=child->opacity();painter.drawShadow(sr,4,sc,child->shadowOffset(),child->shadowRadius());}
        Rect cs=child->screenRect();cs.x-=1;cs.y-=1;cs.width+=2;cs.height+=2;
        painter.pushClip(cs);child->paint(painter);painter.popClip();
        if(ox!=0||oy!=0) child->setPosition(child->bounds().x, child->bounds().y);
    }
}

Widget* Container::hitTest(Point localPoint) {
    if (!m_visible) return nullptr;
    if (localPoint.x<0||localPoint.y<0||localPoint.x>=m_bounds.width||localPoint.y>=m_bounds.height) return nullptr;
    for (auto it=m_children.rbegin();it!=m_children.rend();++it){
        if(!(*it)->visible())continue;Point cl=localPoint;
        cl.x-=(*it)->bounds().x;cl.y-=(*it)->bounds().y;Widget*h=(*it)->hitTest(cl);if(h)return h;}
    return this;
}

bool Container::onScrollWheel(float,float) { return false; }

} // namespace ui
