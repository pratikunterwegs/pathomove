#include <functional>
#include <iostream>
#include <random>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/index/rtree.hpp>


namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<float, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<point, unsigned> value;

int main(int argc, char *argv[])
{
    bgi::rtree< value, bgi::quadratic<16> > rtree;

    // create some values
    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0, 10);
    auto next_rand = std::bind ( distribution, generator );
    float x, y;
    for ( unsigned i = 0 ; i < 100000 ; ++i )
    {
        x = next_rand();
        y = next_rand();
        point p = point(x, y);
        rtree.insert(std::make_pair(p, i));
    }

    // search for nearest neighbours
    std::vector<value> returned_values;
    point sought = point(5, 5);
    box bounding_box(point(5 - 2, 5 - 2), point(5 + 2, 5 + 2));
    rtree.query(
                bgi::within(bounding_box) &&
                bgi::satisfies([&](value const& v) {return bg::distance(v.first, sought) < 2;}),
                std::back_inserter(returned_values));

    // print returned values
    std::cout << returned_values.size() << std::endl;
    //value to_print_out;
    //for (size_t i = 0; i < returned_values.size(); i++) {
        //to_print_out = returned_values[i];
        //float x = to_print_out.first.get<0>();
        //float y = to_print_out.first.get<1>();
        //std::cout << "Select point: " << to_print_out.second << std::endl;
        //std::cout << "x: " << x << ", y: " << y << std::endl;
    //}

    return 0;
}