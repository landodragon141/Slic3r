#ifndef slic3r_PointSpace_hpp
#define slic3r_PointSpace_hpp

#include <vector>
#include <set>
#include <utility>

#include "Point.hpp"
#include "Line.hpp"

namespace Slic3r {

bool zorder_compare(const Point& r, const Point& l);

class Polyline;
class P2tPoint;

static const int VERTS_PER_POLYGON=6;

class ConfSpace {
public:
    typedef int idx_type;
    typedef float weight_type;
    
    struct Poly {
        int verts[VERTS_PER_POLYGON];  // vertices of polygon
        int neis[VERTS_PER_POLYGON];   // neighbor indexes
        bool constrained[VERTS_PER_POLYGON];
        unsigned char vertCount;
        int type;                      // used for costs etc.
        // pathfinding state
        enum {Open,Visited,Closed,
              Path                     // for debugging
        } color;
        Point entry_point;                  // midpoint of side where poly vas entered
        Poly* parent;
        weight_type cost;                   // cost from first point
        weight_type total;                  // cost including heuristic
        
        Poly() : vertCount(0), type(-1) {}
    };

    struct Vertex {
        Point point;
        std::vector<idx_type> polys;        // polygons containing this vertex
        Vertex(const Point& p) : point(p) {}
    };

    struct VertexIdxComparator
    {
        bool operator ()(const Point& a, const Point& b) { return zorder_compare(a,b); }
    };
    
    std::vector<Vertex> vertices;
    typedef std::map<Point, idx_type, VertexIdxComparator> index_type;
    index_type index;

    std::vector<Poly> polys;
    
    weight_type infinity;
    
    struct tri_polygon_type {
        Polygon poly;
        int left, right;
        tri_polygon_type(Polygon p, int r, int l) : poly(p), left(l), right(r) {};
    };
    std::vector<tri_polygon_type> tri_polygons;   // temporary storage of polygons for triangulation, with cost
public:
    ConfSpace();
    Vertex& operator[](idx_type idx) {return vertices[idx]; };
    std::pair<idx_type,bool> insert(const Point& val);
    idx_type find(const Point& val);

//    void add_ExPolygon(const ExPolygon* poly, weight_type cost);
    void add_Polygon(const Polygon poly, int r=-1, int l=-1);
    void triangulate();
    idx_type poly_find_left(idx_type v1, idx_type v2);
    idx_type poly_find_left(const Point& p1, const Point& p2);
    void polys_set_class(idx_type first, int type);

    void points(Points* p);
    void edge_lines(Lines* l);

    void points_in_range(Point& from, float range, Points* p);
    idx_type nearest_point(Point& from);

    idx_type find_poly(const Point& val);

    void poly_get_portal(idx_type from, idx_type to, Point* left, Point* right);
    bool path_dijkstra(const Point& from, const Point& to, std::vector<idx_type>* ret);
    void path_straight(const Point& from, const Point& to, const std::vector<idx_type>& path, Polyline* ret);

    bool path(const Point& from, const Point& to, Polyline*  ret);

    void SVG_dump_path(const char* fname, Point& from, Point& to, const Polyline& straight_path);
protected:
    void path_init();
};


}

#endif
