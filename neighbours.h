
// boost geometry libraries
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>

#include "landscape.h"
#include "agents.h"

// make namespaces
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

// apparently some types
typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

// make rtree
bgi::rtree< value, bgi::quadratic<16> > rtree;

// put items in rtree, since their locations are fixed
