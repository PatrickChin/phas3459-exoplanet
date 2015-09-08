#include <cstring>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <limits>
#include <vector>
#include <ctime>
#include <fstream>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "exoplanet.hpp"

using namespace std;
#define DEBUG(x) cout << ">" << #x << ": " << x << endl

int main(void)
{
    clock_t t1 = clock();


    stringstream response_stream;

    // response_stream <<  "NAMEDATE,PLANETDISCMETH,MASS,SEP,DIST\n"
    //                     "Kepler-107 d,2014,Transit,0.00371234,0.0780099,\n"
    //                     "WASP-14 b,2009,Transit,7.69234,0.0367693,160\n"
    //                     "Kepler-50 b,2011,Transit,0.0164915,0.0825598,\n"
    //                     "NN Ser d,2010,Timing,0.642621,3.70596,\n";

    curlpp::Easy request;
    request.setOpt(curlpp::Options::WriteStream(&response_stream)); // where to write the response to
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
        response_stream.ignore(numeric_limits<streamsize>::max(),'\n'); // ignore first line
        while (getline(response_stream, line))
        {
            exoplanet p = exoplanet::from_string(line.begin(), line.end());
            v.push_back(p);
        }
    } catch (exception &e) {
        cerr << " ERROR: " << e.what() << endl;
        return 0;
    }

    clock_t t3 = clock();

    /* Find closest planet */
    {
        exoplanet *closest = nullptr;
        int i = 0, sz = v.size();
        // find first planet with a known distance
        for (; i < sz; ++i)
            if (v[i].distance() >= 0.0)
            {
                closest = &v[i];
                break;
            }
        // compare remaining planets
        for (; i < sz; ++i)
            if (v[i].distance() >= 0.0 && v[i].distance() < closest->distance())
                closest = &v[i];

        if (closest)
            cout << "Closest planet:\n" << *closest << endl;
    }

    clock_t t4 = clock();

    /* Get stats for discovery methods */
    struct discovery_method_stats {
        int count = 0;
        int earliest_year;
        double lightest_mass;
    };

    map<string,discovery_method_stats> count;
    for (auto p : v)
    {
        auto *cur_method = &count[p.method()];
        if (++(*cur_method).count == 1)
        {
            /* new method discovered */
            (*cur_method).earliest_year = p.year();
            (*cur_method).lightest_mass = p.mass();
        }
        else
        {
            if (p.year() < (*cur_method).earliest_year)
                (*cur_method).earliest_year = p.year();

            if (p.mass() < (*cur_method).lightest_mass)
                (*cur_method).lightest_mass = p.mass();
        }

    }

    clock_t t5 = clock();

    for (auto m : count)
    {
        cout << "Method of discovery: " << m.first << endl
            << "    Planets discovered: " << m.second.count << endl
            << "    Earliest discovery: " << m.second.earliest_year << endl
            << "    Lightest planet:    " << m.second.lightest_mass << endl << endl;
    }

    clock_t t6 = clock();

    std::cout << fixed << setprecision(2)
              << "t1: " << 1000.0 * (t2-t1) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t2: " << 1000.0 * (t3-t2) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t3: " << 1000.0 * (t4-t3) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t4: " << 1000.0 * (t5-t4) / CLOCKS_PER_SEC << " ms\n" << endl
              << "t5: " << 1000.0 * (t6-t5) / CLOCKS_PER_SEC << " ms\n" << endl;

    return 0;
}
