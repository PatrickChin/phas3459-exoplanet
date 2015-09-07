#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <limits>
#include <vector>
#include <ctime>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;
#define DEBUG(x) cout << ">" << #x << ": " << x << endl

struct exoplanet
{
    // enum discovery_method { direct_imaging, transit, radial_velocity, microlensing, timing };

    const string name;
    const int year;
    const string method; // of discovery
    const double mass, separation, distance = -1;

    exoplanet (string name, int year, string method, double mass,
            double separation, double distance = -1) :
        name(name), year(year), method(method), mass(mass),
        separation(separation), distance(distance) { }

    static exoplanet from_string(string s, char delim = ',')
    {
        string tmp;
        stringstream ss(s);

        getline(ss, tmp, delim);
        string name = tmp.c_str();

        getline(ss, tmp, delim);
        int year = stoi(tmp);

        getline(ss, tmp, delim);
        string method = tmp.c_str();

        getline(ss, tmp, delim);
        double mass = stod(tmp);

        getline(ss, tmp, delim);
        double sep = stod(tmp);

        getline(ss, tmp, delim);
        double dist = tmp.size() > 1 ? stod(tmp) : -1; // accounting for the '\0' string ending

        return exoplanet(name,year,method,mass,sep,dist);
    }
};

ostream& operator<<(ostream& os, const exoplanet p)
{
    return os << p.name << endl
        << "    Year of discovery: " << p.year << endl
        << "    Method of discovery: " << p.method << endl
        << "    Mass (Jupiter Masses): " << p.mass << endl
        << "    Separation (AU): " << p.separation << endl
        << "    Distance from Earth (pc): " << (p.distance == -1 ? "Unknown" : to_string(p.distance)) << endl;
}

int main(void)
{
    clock_t t1 = clock();

    curlpp::Easy request;
    stringstream response;

    request.setOpt(curlpp::Options::WriteStream(&response)); // where to write the response to

    try {
        request.setOpt(curlpp::Options::Url("http://www.hep.ucl.ac.uk/undergrad/3459/exam-data/exoplanets.txt"));
        request.perform();
    } catch (exception &e) {
        cerr << e.what() << endl;
        return 0;
    }

    clock_t t2 = clock();

    vector<exoplanet> v;

    try {
        string line;
        response.ignore(numeric_limits<streamsize>::max(),'\n'); // ignore first line
        while (getline(response, line))
        {
            exoplanet p = exoplanet::from_string(line);
            v.push_back(p);
        }
    } catch (exception &e) {
        cerr << e.what() << endl;
        return 0;
    }

    clock_t t3 = clock();

    /* Find closest planet */
    {
        exoplanet *closest = nullptr;
        int i = 0, sz = v.size();
        // find first planet with a known distance
        for (; i < sz; ++i)
            if (v[i].distance >= 0.0)
            {
                closest = &v[i];
                break;
            }
        // compare remaining planets
        for (; i < sz; ++i)
            if (v[i].distance >= 0.0 && v[i].distance < closest->distance)
                closest = &v[i];

        if (closest)
            cout << *closest << endl;
    }

    clock_t t4 = clock();

    /* Get stats for discovery methods */
    {
        struct discovery_method_stats {
            int count = 0;
            int earliest_year;
            double lightest_mass;
        };

        map<string,discovery_method_stats> count;
        for (auto p : v)
        {
            auto *cur_method = &count[p.method];
            if (++(*cur_method).count == 1)
            {
                /* new method discovered */
                (*cur_method).earliest_year = p.year;
                (*cur_method).lightest_mass = p.mass;
            }
            else
            {
                if (p.year < (*cur_method).earliest_year)
                    (*cur_method).earliest_year = p.year;

                if (p.mass < (*cur_method).lightest_mass)
                    (*cur_method).lightest_mass = p.mass;
            }
            
        }

        for (auto m : count)
        {
            cout << "Method of discovery: " << m.first << endl
                << "    Planets discovered: " << m.second.count << endl
                << "    Earliest discovery: " << m.second.earliest_year << endl
                << "    Lightest planet:    " << m.second.lightest_mass << endl << endl;
        }
    }

    clock_t t5 = clock();

    std::cout << fixed << setprecision(2)
              << "t1: " << 1000.0 * (t2-t1) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t2: " << 1000.0 * (t3-t2) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t3: " << 1000.0 * (t4-t3) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t4: " << 1000.0 * (t5-t4) / CLOCKS_PER_SEC << " ms\n" << endl;

    return 0;
}
