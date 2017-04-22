#ifndef TREE_H
#define TREE_H
#include "icg_common.h"
#include "frustum/Frustum.h"
#include <vector>

class Tree {
private:
    typedef Eigen::Transform<float,3,Eigen::Affine> Transform;
    typedef std::vector<Frustum>::iterator ItF;

private:
    float _base_r1;
    float _base_r2;
    float _base_h;
    float _sratio = 0.5;
    float _ang;
    vec3 _position; //< base position
    vec3 _rtnormal; //< rotational normal
    int _N; //< degree

    //< temp buffer
    mat4 _V;
    mat4 _P;
    float _time; /*seconds*/

    //< object buffer
    Frustum _seed;
    std::vector<Frustum> _seeds;

public:

    void init(vec3 pos, vec3 normal, float ang, int n, float r1 = .5, float r2 = 1.0, float h = 4.0){
        _N = n;
        _base_r1 = r1;
        _base_r2 = r2;
        _base_h = h;
        _position = pos;
        _rtnormal = normal;
        _ang = ang;

        //---> Method 1
        //-- allocate memory
        Frustum seed;
        setF(seed, pos, normal, ang, n);

        //---> Method 2
        _seed.init(r1, r2, h);
    }

private:
    //---> RULE: F -> F[<F][^F][>F]
    void setF(Frustum& seed, vec3 pos, vec3 normal, float ang, int n){
        if (n > 0) {
            float r1 = _base_r1 * pow(_sratio, _N - n);
            float r2 = _base_r2 * pow(_sratio, _N - n);
            float h = _base_h * pow(_sratio, _N - n);
            seed.init(r1, r2, h);
            seed.setT(pos);
            seed.setR(normal, ang);
            _seeds.push_back(seed);

            Frustum seed1; seed1.aline(seed);
            Frustum seed2; seed2.aline(seed);
            Frustum seed3; seed3.aline(seed);
            setF(seed1, vec3(0,0,h), vec3( 1.000,  0.000, 0), 40.0, n-1);
            setF(seed2, vec3(0,0,h), vec3(-0.500,  0.866, 0), 40.0, n-1);
            setF(seed3, vec3(0,0,h), vec3(-0.500, -0.866, 0), 40.0, n-1);

        } else {
            float r1 = _base_r1 * pow(_sratio, _N - n);
            float r2 = _base_r2 * pow(_sratio, _N - n);
            float h = _base_h * pow(_sratio, _N - n);
            seed.init(r1, r2, h);
            seed.setT(pos);
            seed.setR(normal, ang);
            _seeds.push_back(seed);
        }
    }

    void drawF(vec3 pos, vec3 normal, float ang, float h, int n){
        if (n > 0) {
            _seed.setT(pos);
            _seed.setR(normal, ang);
            _seed.setS(_sratio);
            drawit(_seed);

            vec3 peak = vec3(0,0,h);
            drawF(peak, vec3( 1.000,  0.000, 0), 40.0, h, n-1);
            drawF(peak, vec3(-0.500,  0.866, 0), 40.0, h, n-1);
            drawF(peak, vec3(-0.500, -0.866, 0), 40.0, h, n-1);

            _seed.setS(1.0f / _sratio);
            _seed.setR(normal, -ang);
            _seed.setT(-pos);

        } else {
            _seed.setT(pos);
            _seed.setR(normal, ang);
            _seed.setS(_sratio);
            drawit(_seed);
            _seed.setS(1.0f / _sratio);
            _seed.setR(normal, -ang);
            _seed.setT(-pos);
        }
    }

    void drawit(Frustum& seed){ seed.draw(_V, _P, _time);  }

public:
    void draw(const mat4& V, const mat4& P, float time /*seconds*/){
        _V = V;
        _P = P;
        _time = time;
        //<...>
        //drawF(_position, _rtnormal, _ang, _base_h , _N);
        for (ItF it = _seeds.begin(); it != _seeds.end(); ++it){
            drawit(*it);
        }
    }

};

#endif
