// #pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

class exoplanet
{
    string name_;
    int year_;
    string method_;
    double mass_, separation_, distance_;

    exoplanet(string &name_, int year_, string &method_, double mass_,
            double separation_, double distance_)
        : name_(name_), year_(year_), method_(method_), mass_(mass_),
            separation_(separation_), distance_(distance_) { }

public:
    const string name() { return this->name_; }
    const int year() { return this->year_; }
    const string method() { return this->method_; }
    const double mass() { return this->mass_; }
    const double separation() { return this->separation_; }
    const double distance() { return this->distance_; }

    static exoplanet from_string(string::const_iterator begin, string::const_iterator end)
    {
        string::const_iterator pos = begin;
        while (*pos != ',') { ++pos; }
        string name(begin, pos);

        begin = ++pos;
        while (*pos != ',') { ++pos; }
        int year = atoi(&(*begin));

        begin = ++pos;
        while (*pos != ',') { ++pos; }
        string method(begin, pos);

        begin = ++pos;
        while (*pos != ',') { ++pos; }
        double mass = atof(&(*begin));

        begin = ++pos;
        while (*pos != ',') { ++pos; }
        double separation = atof(&(*begin));

        begin = ++pos;
        while (pos != end) { ++pos; }
        double distance = /* *begin == '\n' || */ *begin == '\r' ? -1 : atof(&(*begin));

        return exoplanet(name, year, method, mass, separation, distance);
    }

    friend ostream &operator<<(ostream &os, const exoplanet p);
};

class exoplanet_list
{
public:
    exoplanet_list (const string &s)
    {
        int n = count(s.cbegin(), s.cend(), '\n');
        v.reserve(n+1);
    }

private:
    vector<exoplanet> v;
};

ostream &operator<<(ostream &os, const exoplanet p)
{
    return os << p.name_ << endl
        << "    Year of discovery: " << p.year_ << endl
        << "    Method of discovery: " << p.method_ << endl
        << "    Mass (Jupiter Masses): " << p.mass_ << endl
        << "    Separation (AU): " << p.separation_ << endl
        << "    Distance from Earth (pc): " <<
                (p.distance_ == -1 ? "Unknown" : to_string(p.distance_)) << endl;
}

