#include<iostream>
#include<string>
using namespace std;

class shape{
protected:
    int x,y;
public:
    virtual void draw(){cout << "shape draw" << endl;}
    void setOrigin(int x, int y){
        this->x = x;
        this->y = y;
    }
};

class rectangle : public shape{
private:
    int width, height;
public:
    void setWidth(int width){this->width = width;}
    void setHeight(int height){this->height = height;}
    void draw(){cout << "rectangle draw" << endl;}
};

class circle : public shape{
private:
    int radius;
public:
    void setRadius(int radius){this->radius = radius;}
    void draw(){cout << "circle draw" << endl;}
};

class triangle : public shape{
private:
    int base, height;
public:
    void draw(){cout << "triangle draw" << endl;}
};

int main(){
    shape *ps[3];
    ps[0] = new rectangle();
    ps[1] = new triangle();
    ps[2] = new circle();
    for(int i = 0;i < 3;i++) ps[i]->draw();
    return 0;
}