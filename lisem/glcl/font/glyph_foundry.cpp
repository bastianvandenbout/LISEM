#pragma once

// fontnik
#include "glyph_foundry.hpp"

#include "agg/agg_curves.h"

// boost
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>

// std
#include <cmath> // std::sqrt

namespace bg = boost::geometry;
namespace bgm = bg::model;
namespace bgi = bg::index;
typedef bgm::point<float, 2, bg::cs::cartesian> GlyphPoint;
typedef bgm::box<GlyphPoint> Box;
typedef std::vector<GlyphPoint> GlyphPoints;
typedef std::vector<GlyphPoints> Rings;
typedef std::pair<GlyphPoint, GlyphPoint> SegmentPair;
typedef std::pair<Box, SegmentPair> SegmentValue;
typedef bgi::rtree<SegmentValue, bgi::rstar<16>> Tree;


namespace sdf_glyph_foundry
{
    struct User {
        Rings rings;
        GlyphPoints ring;
    };

    void CloseRing(GlyphPoints &ring)
    {
        const GlyphPoint &first = ring.front();
        const GlyphPoint &last = ring.back();

        if (first.get<0>() != last.get<0>() ||
            first.get<1>() != last.get<1>())
        {
            ring.push_back(first);
        }
    }

    int MoveTo(const FT_Vector *to, void *ptr)
    {
        User *user = (User*)ptr;
        if (!user->ring.empty()) {
            CloseRing(user->ring);
            user->rings.push_back(user->ring);
            user->ring.clear();
        }
        user->ring.emplace_back(float(to->x) / 64.0, float(to->y) / 64.0);
        return 0;
    }

    int LineTo(const FT_Vector *to, void *ptr)
    {
        User *user = static_cast<User*>(ptr);
        user->ring.emplace_back(float(to->x) / 64.0, float(to->y) / 64.0);
        return 0;
    }

    int ConicTo(const FT_Vector *control,
                const FT_Vector *to,
                void *ptr)
    {
        User *user = static_cast<User*>(ptr);

        if (!user->ring.empty()) {
            GlyphPoint const& prev = user->ring.back();
            auto dx = prev.get<0>();
            auto dy = prev.get<1>();

            // pop off last GlyphPoint, duplicate of first GlyphPoint in bezier curve
            // WARNING: pop_back invalidates `prev`
            // http://en.cppreference.com/w/cpp/container/vector/pop_back
            user->ring.pop_back();

            agg_fontnik::curve3_div curve(dx,dy,
                                  float(control->x) / 64, float(control->y) / 64,
                                  float(to->x) / 64, float(to->y) / 64);

            curve.rewind(0);
            double x, y;
            unsigned cmd;

            while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y))) {
                user->ring.emplace_back(x, y);
            }
        }

        return 0;
    }

    int CubicTo(const FT_Vector *c1,
                const FT_Vector *c2,
                const FT_Vector *to,
                void *ptr)
    {
        User *user = static_cast<User*>(ptr);

        if (!user->ring.empty()) {

            GlyphPoint const& prev = user->ring.back();
            auto dx = prev.get<0>();
            auto dy = prev.get<1>();

            // pop off last GlyphPoint, duplicate of first GlyphPoint in bezier curve
            // WARNING: pop_back invalidates `prev`
            // http://en.cppreference.com/w/cpp/container/vector/pop_back
            user->ring.pop_back();

            agg_fontnik::curve4_div curve(dx,dy,
                                  float(c1->x) / 64, float(c1->y) / 64,
                                  float(c2->x) / 64, float(c2->y) / 64,
                                  float(to->x) / 64, float(to->y) / 64);

            curve.rewind(0);
            double x, y;
            unsigned cmd;

            while (agg_fontnik::path_cmd_stop != (cmd = curve.vertex(&x, &y))) {
                user->ring.emplace_back(x, y);
            }
        }

        return 0;
    }

    // GlyphPoint in polygon ray casting algorithm
    bool PolyContainsGlyphPoint(const Rings &rings, const GlyphPoint &p)
    {
        bool c = false;

        for (const GlyphPoints &ring : rings) {
            auto p1 = ring.begin();
            auto p2 = p1 + 1;

            for (; p2 != ring.end(); p1++, p2++) {
                if (((p1->get<1>() > p.get<1>()) != (p2->get<1>() > p.get<1>())) && (p.get<0>() < (p2->get<0>() - p1->get<0>()) * (p.get<1>() - p1->get<1>()) / (p2->get<1>() - p1->get<1>()) + p1->get<0>())) {
                    c = !c;
                }
            }
        }

        return c;
    }

    double SquaredDistance(const GlyphPoint &v, const GlyphPoint &w)
    {
        const double a = v.get<0>() - w.get<0>();
        const double b = v.get<1>() - w.get<1>();
        return a * a + b * b;
    }

    GlyphPoint ProjectGlyphPointOnLineSegment(const GlyphPoint &p,
                                    const GlyphPoint &v,
                                    const GlyphPoint &w)
    {
      const double l2 = SquaredDistance(v, w);
      if (l2 == 0) return v;

      const double t = ((p.get<0>() - v.get<0>()) * (w.get<0>() - v.get<0>()) + (p.get<1>() - v.get<1>()) * (w.get<1>() - v.get<1>())) / l2;
      if (t < 0) return v;
      if (t > 1) return w;

      return GlyphPoint {
          v.get<0>() + t * (w.get<0>() - v.get<0>()),
          v.get<1>() + t * (w.get<1>() - v.get<1>())
      };
    }

    double SquaredDistanceToLineSegment(const GlyphPoint &p,
                                        const GlyphPoint &v,
                                        const GlyphPoint &w)
    {
        const GlyphPoint s = ProjectGlyphPointOnLineSegment(p, v, w);
        return SquaredDistance(p, s);
    }

    double MinDistanceToLineSegment(const Tree &tree,
                                    const GlyphPoint &p,
                                    int radius)
    {
        const int squared_radius = radius * radius;

        std::vector<SegmentValue> results;
        tree.query(bgi::intersects(
            Box{
                GlyphPoint{p.get<0>() - radius, p.get<1>() - radius},
                GlyphPoint{p.get<0>() + radius, p.get<1>() + radius}
            }),
            std::back_inserter(results));

        double sqaured_distance = std::numeric_limits<double>::infinity();

        for (const auto &value : results) {
            const SegmentPair &segment = value.second;
            const double dist = SquaredDistanceToLineSegment(p,
                                                             segment.first,
                                                             segment.second);
            if (dist < sqaured_distance && dist < squared_radius) {
                sqaured_distance = dist;
            }
        }

        return std::sqrt(sqaured_distance);
    }

    void RenderSDF(glyph_info &glyph,
                         int size,
                         int buffer,
                         float cutoff,
                         FT_Face ft_face)
    {

        if (FT_Load_Glyph (ft_face, glyph.glyph_index, FT_LOAD_NO_HINTING)) {
            return;
        }

        int advance = ft_face->glyph->metrics.horiAdvance / 64;
        int ascender = ft_face->size->metrics.ascender / 64;
        int descender = ft_face->size->metrics.descender / 64;

        glyph.line_height = ft_face->size->metrics.height;
        glyph.advance = advance;
        glyph.ascender = ascender;
        glyph.descender = descender;

        FT_Outline_Funcs func_interface = {
            .move_to = &MoveTo,
            .line_to = &LineTo,
            .conic_to = &ConicTo,
            .cubic_to = &CubicTo,
            .shift = 0,
            .delta = 0
        };

        User user;

        if (ft_face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
            // Decompose outline into bezier curves and line segments
            FT_Outline outline = ft_face->glyph->outline;
            if (FT_Outline_Decompose(&outline, &func_interface, &user)) return;

            if (!user.ring.empty()) {
                CloseRing(user.ring);
                user.rings.push_back(user.ring);
            }

            if (user.rings.empty()) {
                return;
            }
        } else {
            return;
        }

        // Calculate the real glyph bbox.
        double bbox_xmin = std::numeric_limits<double>::infinity(),
               bbox_ymin = std::numeric_limits<double>::infinity();

        double bbox_xmax = -std::numeric_limits<double>::infinity(),
               bbox_ymax = -std::numeric_limits<double>::infinity();

        for (const GlyphPoints &ring : user.rings) {
            for (const GlyphPoint &GlyphPoint : ring) {
                if (GlyphPoint.get<0>() > bbox_xmax) bbox_xmax = GlyphPoint.get<0>();
                if (GlyphPoint.get<0>() < bbox_xmin) bbox_xmin = GlyphPoint.get<0>();
                if (GlyphPoint.get<1>() > bbox_ymax) bbox_ymax = GlyphPoint.get<1>();
                if (GlyphPoint.get<1>() < bbox_ymin) bbox_ymin = GlyphPoint.get<1>();
            }
        }

        bbox_xmin = std::round(bbox_xmin);
        bbox_ymin = std::round(bbox_ymin);
        bbox_xmax = std::round(bbox_xmax);
        bbox_ymax = std::round(bbox_ymax);

        // Offset so that glyph outlines are in the bounding box.
        for (GlyphPoints &ring : user.rings) {
            for (GlyphPoint &GlyphPoint : ring) {
                GlyphPoint.set<0>(GlyphPoint.get<0>() + -bbox_xmin + buffer);
                GlyphPoint.set<1>(GlyphPoint.get<1>() + -bbox_ymin + buffer);
            }
        }

        if (bbox_xmax - bbox_xmin == 0 || bbox_ymax - bbox_ymin == 0) return;

        glyph.left = bbox_xmin;
        glyph.top = bbox_ymax;
        glyph.width = bbox_xmax - bbox_xmin;
        glyph.height = bbox_ymax - bbox_ymin;

        Tree tree;
        float offset = 0.5;
        int radius = 8;
        int radius_by_256 = (256 / radius);

        for (const GlyphPoints &ring : user.rings) {
            auto p1 = ring.begin();
            auto p2 = p1 + 1;

            for (; p2 != ring.end(); p1++, p2++) {
                const int segment_x1 = std::min(p1->get<0>(), p2->get<0>());
                const int segment_x2 = std::max(p1->get<0>(), p2->get<0>());
                const int segment_y1 = std::min(p1->get<1>(), p2->get<1>());
                const int segment_y2 = std::max(p1->get<1>(), p2->get<1>());

                tree.insert(SegmentValue {
                    Box {
                        GlyphPoint {segment_x1, segment_y1},
                        GlyphPoint {segment_x2, segment_y2}
                    },
                    SegmentPair {
                        GlyphPoint {p1->get<0>(), p1->get<1>()},
                        GlyphPoint {p2->get<0>(), p2->get<1>()}
                    }
                });
            }
        }

        // Loop over every pixel and determine the positive/negative distance to the outline.
        unsigned int buffered_width = glyph.width + 2 * buffer;
        unsigned int buffered_height = glyph.height + 2 * buffer;
        unsigned int bitmap_size = buffered_width * buffered_height;
        glyph.bitmap.resize(bitmap_size);

        for (unsigned int y = 0; y < buffered_height; y++) {
            for (unsigned int x = 0; x < buffered_width; x++) {
                unsigned int ypos = buffered_height - y - 1;
                unsigned int i = ypos * buffered_width + x;
                GlyphPoint pt{x + offset, y + offset };
                double d = MinDistanceToLineSegment(tree, pt, radius) * radius_by_256;

                // Invert if GlyphPoint is inside.
                const bool inside = PolyContainsGlyphPoint(user.rings, pt);
                if (inside) {
                    d = -d;
                }

                // Shift the 0 so that we can fit a few negative values
                // into our 8 bits.
                d += cutoff * 256;

                // Clamp to 0-255 to prevent overflows or underflows.
                int n = d > 255 ? 255 : d;
                n = n < 0 ? 0 : n;

                glyph.bitmap[i] = static_cast<char>(255 - n);
            }
        }
    }

} // ns sdf_glyph_foundry
