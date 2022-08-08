//
// Created by JinxBIGBIG on 2022/6/27.
//

#ifndef ODRPARSER_DATAOPERATION_H
#define ODRPARSER_DATAOPERATION_H

#include <vector>
#include <iostream>
/*
计算几何
目录
㈠ 点的基本运算
1. 平面上两点之间距离 1
2. 判断两点是否重合 1
3. 矢量叉乘 1
4. 矢量点乘 2
5. 判断点是否在线段上 2
6. 求一点饶某点旋转后的坐标 2
7. 求矢量夹角 2
㈡ 线段及直线的基本运算
1. 点与线段的关系 3
2. 求点到线段所在直线垂线的垂足 4
3. 点到线段的最近点 4
4. 点到线段所在直线的距离 4
5. 点到折线集的最近距离 4
6. 判断圆是否在多边形内 5
7. 求矢量夹角余弦 5
8. 求线段之间的夹角 5
9. 判断线段是否相交 6
10.判断线段是否相交但不交在端点处 6
11.求线段所在直线的方程 6
12.求直线的斜率 7
13.求直线的倾斜角 7
14.求点关于某直线的对称点 7
15.判断两条直线是否相交及求直线交点 7
16.判断线段是否相交，如果相交返回交点 7
㈢ 多边形常用算法模块
1. 判断多边形是否简单多边形 8
2. 检查多边形顶点的凸凹性 9
3. 判断多边形是否凸多边形 9
4. 求多边形面积 9
5. 判断多边形顶点的排列方向，方法一 10
6. 判断多边形顶点的排列方向，方法二 10
7. 射线法判断点是否在多边形内 10
8. 判断点是否在凸多边形内 11
9. 寻找点集的graham算法 12
10.寻找点集凸包的卷包裹法 13
11.判断线段是否在多边形内 14
12.求简单多边形的重心 15
13.求凸多边形的重心 17
14.求肯定在给定多边形内的一个点 17
15.求从多边形外一点出发到该多边形的切线 18
16.判断多边形的核是否存在 19
㈣ 圆的基本运算
1 .点是否在圆内 20
2 .求不共线的三点所确定的圆 21
㈤ 矩形的基本运算
1.已知矩形三点坐标，求第4点坐标 22
㈥ 常用算法的描述 22
㈦ 补充
1．两圆关系： 24
2．判断圆是否在矩形内： 24
3．点到平面的距离： 25
4．点是否在直线同侧： 25
5．镜面反射线： 25
6．矩形包含： 26
7．两圆交点： 27
8．两圆公共面积： 28
9. 圆和直线关系： 29
10. 内切圆： 30
11. 求切点： 31
12. 线段的左右旋： 31
13．公式： 32
*/
/* 基本几何结构 */
struct POINT
{
    double x;
    double y;
    POINT(double a=0, double b=0) { x=a; y=b;} //constructor
};

struct LINESEG
{
    POINT s;
    POINT e;
    LINESEG(POINT a, POINT b) { s=a; e=b;}
    LINESEG() { }
};

struct LINE           // 直线的解析方程 a*x+b*y+c=0  为统一表示，约定 a >= 0
{
    double a;
    double b;
    double c;
    LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}
};

class DataOperation {

public:
    DataOperation();

    //(5, 10)——01010
    std::vector<int> Int2Binary(int figure, int intNumber);

    template<typename T>   //or class T
    void Print(std::vector<T> &t){
        std::cout << "Size = " << t.size() << std::endl;
        for(int i = 0; i < t.size(); i++)
        {
            std::cout << t.at(i);
        }
        std::cout << std::endl;
    };

    /**********************
 *                    *
 *   点的基本运算     *
 *                    *
 **********************/
    double dist(POINT p1, POINT p2);

    bool equal_point(POINT p1, POINT p2);

    double multiply(POINT sp, POINT ep, POINT op);

    double dotmultiply(POINT p1, POINT p2, POINT p0);

    bool online(LINESEG l, POINT p);

    POINT rotate(POINT o, double alpha, POINT p);

    double angle(POINT o, POINT s, POINT e);

    double relation(POINT p, LINESEG l);

    POINT perpendicular(POINT p, LINESEG l);

    double ptolinesegdist(POINT p, LINESEG l, POINT &np);

    double ptoldist(POINT p, LINESEG l);

    double ptopointset(int vcount, POINT *pointset, POINT p, POINT &q);

    bool CircleInsidePolygon(int vcount, POINT center, double radius, POINT *polygon);

    double lsangle(LINESEG l1, LINESEG l2);

    double cosine(LINESEG l1, LINESEG l2);

    bool intersect(LINESEG u, LINESEG v);

    bool intersect_A(LINESEG u, LINESEG v);

    bool intersect_l(LINESEG u, LINESEG v);

    LINE makeline(POINT p1, POINT p2);

    double slope(LINE l);

    double alpha(LINE l);

    POINT symmetry(LINE l, POINT p);

    bool lineintersect(LINE l1, LINE l2, POINT &p);

    bool intersection(LINESEG l1, LINESEG l2, POINT &inter);

    bool issimple(int vcount, POINT *polygon);

    void checkconvex(int vcount, POINT *polygon, bool *bc);

    bool isconvex(int vcount, POINT *polygon);

    double area_of_polygon(int vcount, POINT *polygon);

    bool isconterclock(int vcount, POINT *polygon);

    bool isccwize(int vcount, POINT *polygon);

    int insidepolygon(int vcount, POINT *Polygon, POINT q);

    bool InsideConvexPolygon(int vcount, POINT *polygon, POINT q);

    void Graham_scan(POINT *PointSet, POINT *ch, int n, int &len);

    void ConvexClosure(POINT *PointSet, POINT *ch, int n, int &len);

    bool LinesegInsidePolygon(int vcount, POINT *polygon, LINESEG l);

    void AddPosPart(double x, double y, double w, double &xtr, double &ytr, double &wtr);

    void AddNegPart(double x, double y, double w, double &xtl, double &ytl, double &wtl);

    void
    AddRegion(double x1, double y1, double x2, double y2, double &xtr, double &ytr, double &wtr, double &xtl,
              double &ytl,
              double &wtl);

    POINT cg_simple(int vcount, POINT *polygon);

    POINT gravitycenter(int vcount, POINT *polygon);

    POINT a_point_insidepoly(int vcount, POINT *polygon);

    void pointtangentpoly(int vcount, POINT *polygon, POINT p, POINT &rp, POINT &lp);

    bool core_exist(int vcount, POINT *polygon, POINT &p);

    bool point_in_circle(POINT o, double r, POINT p);

    bool cocircle(POINT p1, POINT p2, POINT p3, POINT &q, double &r);

    int line_circle(LINE l, POINT o, double r, POINT &p1, POINT &p2);

    POINT rect4th(POINT a, POINT b, POINT c);

    int CircleRelation(POINT p1, double r1, POINT p2, double r2);

    bool CircleRecRelation(POINT pc, double r, POINT pr1, POINT pr2, POINT pr3, POINT pr4);

    double P2planeDist(double x, double y, double z, double a, double b, double c, double d);

    bool SameSide(POINT p1, POINT p2, LINE line);

    void reflect(double a1, double b1, double c1, double a2, double b2, double c2, double &a, double &b, double &c);

    bool r2inr1(double A, double B, double C, double D);

    void c2point(POINT p1, double r1, POINT p2, double r2, POINT &rp1, POINT &rp2);

    double c2area(POINT p1, double r1, POINT p2, double r2);

    int clpoint(POINT p, double r, double a, double b, double c, POINT &rp1, POINT &rp2);

    void incircle(POINT p1, POINT p2, POINT p3, POINT &rp, double &r);

    void cutpoint(POINT p, double r, POINT sp, POINT &rp1, POINT &rp2);

    int rotat(LINESEG l1, LINESEG l2);
};


#endif //ODRPARSER_DATAOPERATION_H
