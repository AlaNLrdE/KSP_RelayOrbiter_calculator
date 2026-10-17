#include "KspBodyDatabase.hpp"
#include <algorithm>

const std::vector<CelestialBody> KspBodyDatabase::bodies_ = {
    { "Kerbol",  261'600'000.0, 1.1723328e18,  0.0,        432'000.0,       std::nullopt,        std::nullopt,             std::nullopt    },
    { "Moho",        250'000.0, 1.6860938e11,  0.0,      1'210'000.0,   9'646'663.0,     5'263'138'304.0,          "Kerbol"        },
    { "Eve",         700'000.0, 8.1717302e12, 90'000.0,     80'500.0,  85'109'365.0,     9'832'684'544.0,          "Kerbol"        },
    { "Gilly",        13'000.0, 8'289'450.0,   0.0,         28'255.0,     126'123.0,        31'500'000.0,            "Eve"          },
    { "Kerbin",      600'000.0, 3.5316000e12, 70'000.0,  21'549.425,   84'159'286.0,    13'599'840'256.0,          "Kerbol"        },
    { "Mun",         200'000.0, 6.5138398e10,  0.0,        138'984.38,  2'429'559.0,        12'000'000.0,           "Kerbin"        },
    { "Minmus",       60'000.0, 1.7658000e9,   0.0,      1'077'311.08,  2'247'428.0,        47'000'000.0,           "Kerbin"        },
    { "Duna",        320'000.0, 3.0136321e11, 50'000.0,  65'517.859,   47'921'949.0,    20'726'155'264.0,          "Kerbol"        },
    { "Ike",         130'000.0, 1.8568369e10,  0.0,         65'517.862,  1'049'598.0,         3'200'000.0,           "Duna"         },
    { "Dres",        138'000.0, 2.1484489e10,  0.0,         34'800.0,   32'832'840.0,    40'839'348'203.0,          "Kerbol"        },
    { "Jool",      6'000'000.0, 2.8252800e14, 200'000.0,   36'000.0, 2'455'985'200.0,   68'773'560'320.0,          "Kerbol"        },
    { "Laythe",      500'000.0, 1.9620000e12, 50'000.0,    52'980.879,  3'723'645.0,        27'184'000.0,            "Jool"         },
    { "Vall",        300'000.0, 2.0748150e11,  0.0,        105'962.09,  2'406'401.0,        43'152'000.0,            "Jool"         },
    { "Tylo",        600'000.0, 2.8252800e12,  0.0,        211'926.36, 10'856'518.0,        68'500'000.0,            "Jool"         },
    { "Bop",          65'000.0, 2.4868349e9,   0.0,        544'507.4,   1'221'060.0,       128'500'000.0,            "Jool"         },
    { "Pol",          44'000.0, 7.2170208e8,   0.0,        901'902.62,  1'042'138.0,       179'890'000.0,            "Jool"         },
    { "Eeloo",       210'000.0, 7.4410815e10,  0.0,         19'460.0,  119'082'940.0,   113'549'713'200.0,          "Kerbol"        },
};

std::optional<CelestialBody> KspBodyDatabase::find(std::string_view name) const {
    for (const auto& b : bodies_)
        if (b.name == name) return b;
    return std::nullopt;
}

std::vector<CelestialBody> KspBodyDatabase::all() const {
    return bodies_;
}

std::vector<CelestialBody> KspBodyDatabase::children_of(std::string_view parent_name) const {
    std::vector<CelestialBody> result;
    for (const auto& b : bodies_)
        if (b.parent.has_value() && *b.parent == parent_name)
            result.push_back(b);
    std::sort(result.begin(), result.end(),
        [](const CelestialBody& a, const CelestialBody& b) {
            return *a.orbital_radius < *b.orbital_radius;
        });
    return result;
}
