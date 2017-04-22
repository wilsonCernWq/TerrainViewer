#ifndef BEZIER_H
#define BEZIER_H
#include "icg_common.h"
#include "../quad/Quad.h"

const static Scalar H = .7;
const static Scalar R = 2;

class BezierCurve : public Quad {
public:
    class Hull{
    public:
        Hull() {
            _p1 = _p2 = _p3 = _p4 = vec3::Zero();
        }

        vec3& p1(){ return _p1; }
        vec3& p2(){ return _p2; }
        vec3& p3(){ return _p3; }
        vec3& p4(){ return _p4; }

        vec3* getP1() { return &_p1; }
        vec3* getP2() { return &_p2; }
        vec3* getP3() { return &_p3; }
        vec3* getP4() { return &_p4; }

    private:
        vec3 _p1;
        vec3 _p2;
        vec3 _p3;
        vec3 _p4;
    };
    
private:
    std::vector<Hull> _hulls;                  ///< Bezier control points
    std::vector<vec3> _vertices; ///< Bezier evaluated points
    std::vector<Scalar> _param;  ///< Arc length parameterization
    double last_time;
    double position;
    GLuint _vbo;
    
private:
    void bezier(Hull &p, int depth=0, int index = 0){
        Hull l;
        Hull r;
        if (index > 0) {
            p.p1() = _hulls[index-1].p4();
        }
        
        //--- endpoints
        l.p1() = p.p1();
        r.p4() = p.p4();
        
        //--- one-in
        l.p2() = (p.p1() + p.p2()) / 2.0;
        r.p3() = (p.p3() + p.p4()) / 2.0;
        
        //--- interior
        l.p3() = ( l.p2() + (p.p2() + p.p3())/2.0 )/2.0;
        r.p2() = ( r.p3() + (p.p2() + p.p3())/2.0 )/2.0;
        
        //--- middle
        l.p4() = ( l.p3() + r.p2() ) / 2.0;
        r.p1() = l.p4();
        
        //--- recursion v.s. draw
        if(depth<5){
            bezier(l,depth+1);
            bezier(r,depth+1);
        } else {
            _vertices.push_back(l.p1());
        }
        _vertices.push_back(p.p4());
    }

    void compute_parameterization(){
        _param.clear();;
        if (_vertices.empty()) {
            return;
        }
        _param.resize(_vertices.size(),0);

        for (int i = 0; i+1 < _vertices.size(); ++i) {
            _param[i] = sqrt(pow(_vertices[i+1][0] - _vertices[i][0], 2)
                                + pow(_vertices[i+1][1] - _vertices[i][1], 2)
                                + pow(_vertices[i+1][2] - _vertices[i][2], 2));
        }
    }
public:
    BezierCurve() {
        _vshader = "lib/bezier/bezier_vshader.glsl";
        _fshader = "lib/bezier/bezier_fshader.glsl";
        last_time = 0;
        position = 0;
    }

    static bool cmp(const Scalar &a, const Scalar &b){
        return (a<b);
    }

    void addSegment(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4) {
        _vertices.clear();
        _param.clear();

        Hull hull;
        hull.p1() = p1;
        hull.p2() = p2;
        hull.p3() = p3;
        hull.p4() = p4;
        _hulls.push_back(hull);

        for (int i = 0; i < _hulls.size(); i++) {
            bezier(_hulls[i], 0, i);
        }

        compute_parameterization();
    }

    void sample_point(double time, int speed, vec3 &sample) {
        if (_vertices.empty() || _param.empty()) {
            return;
        }

        if (last_time != 0) {
            double t = time - last_time;
            t = t / speed - floor(t / speed);
            position += t;
            if (position > 1) {
                position -= 1.0;
            }
        }
        last_time = time;

        ///===================== TODO =====================
        ///--- TODO H5.3: Uniformly sample a point in the curve using t in [0, 1]
        /// Assume piece-wise linear approximation
        /// The distance along the curve from _vertices[0] to sample is
        /// t * curve_length
        ///================================================
        Scalar total_length = 0;
        for (int i = 0; i < _param.size(); i++) {
            total_length += _param[i];
        }

        float distance = position * total_length;
        // Searching for the good edge
        int index = 0;
        while (distance > _param[index]) {
            distance -= _param[index];
            index++;
        }

        // Calcul the position
        vec3 unit_vector = (_vertices[index+1] - _vertices[index]).normalized();
        unit_vector *= distance;

        sample = _vertices[index] + unit_vector;
    }

    void ShaderParameters(GLuint program){

        /// Generate one buffer, put the resulting identifier in vertexbuffer
        glGenBuffers(ONE, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    }

    void draw(const mat4& model, const mat4& view, const mat4& projection){
        if (_vertices.empty()) return;

        _vertices.clear();
        _param.clear();

        for (int i = 0; i < _hulls.size(); i++) {
            bezier(_hulls[i], 0, i);
        }

        compute_parameterization();

        glUseProgram(_pid);
        glBindVertexArray(_vao);
        check_error_gl();

        ///--- Vertex Attribute ID for Vertex Positions
        GLuint position = glGetAttribLocation(_pid, "position");
        glEnableVertexAttribArray(position);
        glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

        ///--- vertices
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW);

        ///--- setup view matrices        
        GLuint projection_id = glGetUniformLocation(_pid, "projection");
        glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());
        mat4 MV = view*model;
        GLuint model_view_id = glGetUniformLocation(_pid, "model_view");
        glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, MV.data());
        check_error_gl();

        glDrawArrays(GL_LINE_STRIP, 0, _vertices.size());
        glDisableVertexAttribArray(position);
        glBindVertexArray(0);
        glUseProgram(0);
        check_error_gl();
    }

    std::vector<vec3*> getPoints() {
        std::vector<vec3*> points;
        points.push_back(_hulls.at(0).getP1());
        for (int i = 0; i < _hulls.size(); i++) {
            points.push_back(_hulls.at(i).getP2());
            points.push_back(_hulls.at(i).getP3());
            points.push_back(_hulls.at(i).getP4());
        }

        return points;
    }
};

#endif
