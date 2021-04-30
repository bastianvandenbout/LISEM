#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include "linear/lsm_vector2.h"
#include "polygon.h"
namespace mapbox {

namespace detail {

// get squared distance from a point to a segment
double getSegDistSq(const LSMVector2& p,
               const LSMVector2& a,
               const LSMVector2& b) {
    auto x = a.x;
    auto y = a.y;
    auto dx = b.x - x;
    auto dy = b.y - y;

    if (dx != 0 || dy != 0) {

        auto t = ((p.x - x) * dx + (p.y - y) * dy) / (dx * dx + dy * dy);

        if (t > 1) {
            x = b.x;
            y = b.y;

        } else if (t > 0) {
            x += dx * t;
            y += dy * t;
        }
    }

    dx = p.x - x;
    dy = p.y - y;

    return dx * dx + dy * dy;
}

// signed distance from point to polygon outline (negative if point is outside)
double pointToPolygonDist(const LSMVector2& point, LSMPolygon& polygon) {
    bool inside = false;
    auto minDistSq = std::numeric_limits<double>::infinity();

    for (int k = 0; k < polygon.GetRingCount(); k++) {
        Ring * ring = polygon.GetRing(k);
        for (std::size_t i = 0, len = ring->size(), j = len - 1; i < len; j = i++) {
            const auto a = ring->GetVertex(i);
            const auto b = ring->GetVertex(j);

            if ((a.y > point.y) != (b.y > point.y) &&
                (point.x < (b.x - a.x) * (point.y - a.y) / (b.y - a.y) + a.x)) inside = !inside;

            minDistSq = std::min(minDistSq, getSegDistSq(point, a, b));
        }
    }

    return (inside ? 1 : -1) * std::sqrt(minDistSq);
}

struct Cell {
    Cell(const LSMVector2& c_, double h_,LSMPolygon& polygon)
        : c(c_),
          h(h_),
          d(pointToPolygonDist(c, polygon)),
          max(d + h * std::sqrt(2))
        {}

    LSMVector2 c; // cell center
    double h; // half the cell size
    double d; // distance from cell center to polygon
    double max; // max distance to polygon within a cell
};

// get polygon centroid
Cell getCentroidCell(LSMPolygon& polygon) {
    double area = 0;
    LSMVector2 c(0.0,0.0);
    Ring * ring = polygon.GetRing(0);


    for (std::size_t i = 0, len = ring->size(), j = len - 1; i < len; j = i++) {
        auto a = ring->GetVertex(i);
        auto b = ring->GetVertex(j);
        auto f = a.x * b.y - b.x * a.y;
        c.x += (a.x + b.x) * f;
        c.y += (a.y + b.y) * f;
        area += f * 3;
    }

    return Cell(area == 0 ? ring->GetVertex(0) : c / area, 0, polygon);
}

} // namespace detail

LSMVector2 polylabel(LSMPolygon& polygon, double precision = 1, bool debug = false) {
    using namespace detail;

    // find the bounding box of the outer ring
    BoundingBox envelope = polygon.GetBoundingBox();

    LSMVector2 size {
        (float)(envelope.GetMaxX() - envelope.GetMinX()),
        (float)(envelope.GetMaxY() - envelope.GetMinY())
    };

    double cellSize = std::min(size.x, size.y);
    double h = cellSize / 2;

    // a priority queue of cells in order of their "potential" (max distance to polygon)
    auto compareMax = [] (const Cell& a, const Cell& b) {
        return a.max < b.max;
    };
    using Queue = std::priority_queue<Cell, std::vector<Cell>, decltype(compareMax)>;
    Queue cellQueue(compareMax);

    if (cellSize == 0) {
        return LSMVector2(envelope.GetMinX(),envelope.GetMinY());
    }

    // cover polygon with initial cells
    for (double x = envelope.GetMinX(); x < envelope.GetMaxX(); x += cellSize) {
        for (double y = envelope.GetMinY(); y < envelope.GetMaxY(); y += cellSize) {
            cellQueue.push(Cell({(float)(x + h), (float)(y + h)}, h, polygon));
        }
    }

    // take centroid as the first best guess
    auto bestCell = getCentroidCell(polygon);

    // second guess: bounding box centroid
    Cell bboxCell(LSMVector2(envelope.GetMinX(),envelope.GetMinY()) + size / 2.0, 0, polygon);
    if (bboxCell.d > bestCell.d) {
        bestCell = bboxCell;
    }

    auto numProbes = cellQueue.size();
    while (!cellQueue.empty()) {
        // pick the most promising cell from the queue
        auto cell = cellQueue.top();
        cellQueue.pop();

        // update the best cell if we found a better one
        if (cell.d > bestCell.d) {
            bestCell = cell;
            if (debug) std::cout << "found best " << ::round(1e4 * cell.d) / 1e4 << " after " << numProbes << " probes" << std::endl;
        }

        // do not drill down further if there's no chance of a better solution
        if (cell.max - bestCell.d <= precision) continue;

        // split the cell into four cells
        h = cell.h / 2;
        cellQueue.push(Cell({(float)(cell.c.x - h), (float)(cell.c.y - h)}, h, polygon));
        cellQueue.push(Cell({(float)(cell.c.x + h), (float)(cell.c.y - h)}, h, polygon));
        cellQueue.push(Cell({(float)(cell.c.x - h),(float)( cell.c.y + h)}, h, polygon));
        cellQueue.push(Cell({(float)(cell.c.x + h),(float)( cell.c.y + h)}, h, polygon));
        numProbes += 4;
    }

    if (debug) {
        std::cout << "num probes: " << numProbes << std::endl;
        std::cout << "best distance: " << bestCell.d << std::endl;
    }

    return bestCell.c;
}

} // namespace mapbox
