#ifndef PDF_H_
#define PDF_H_
#include "onb.h"

class pdf {
public:
    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf {
public:
    cosine_pdf(const vec3& w) { uvw.build_from_w(w); }
    virtual double value(const vec3& direction) const {
        double cosine = dot(unit_vector(direction), uvw.w());
        if (cosine > 0)
            return cosine / PI;
        else
            return 0;
    }
    virtual vec3 generate() const {
        return uvw.local(random_cosine_direction());
    }
    onb uvw;
};

class hittable_pdf : public pdf {
public:
    hittable_pdf(hitable* p, const vec3& origin) : ptr(p), o(origin) {}
    virtual double value(const vec3& direction) const {
        return ptr->pdf_value(o, direction);
    }
    virtual vec3 generate() const {
        return ptr->random(o);
    }
    vec3 o;
    hitable* ptr;
};

class mixture_pdf :public pdf {
public:
    mixture_pdf(pdf* p0, pdf* p1) { p[0] = p0; p[1] = p1; }
    virtual double value(const vec3& direction) const {
        return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
    }
    virtual vec3 generate() const {
        if (drand48() < 0.5)
            return p[0]->generate();
        else
            return p[1]->generate();
    }
    pdf* p[2];
};
#endif