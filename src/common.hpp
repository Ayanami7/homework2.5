#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <set>

#define WIDTH 1280
#define HEIGHT 720

enum struct ShapeType
{
    MY_LINE_STRIP,
    MY_LINE_LOOP,
};

enum struct LineType
{
    DDA,
    BRESENHAM,
    MIDPOINT
};

/*DDA算法
采用增量的方式,y=kx,x增加1,则y增加k
但斜率大于1时,会导致点分布稀疏，视觉上不连续
故当k大于1时,将y=kx转换为x=my,即取dx和dy较大的一方为参考轴
*/
void DDA(int x1, int y1, int x2, int y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    // 取xy上差值较大的作为参考
    float step = std::max(abs(dx), abs(dy));

    // 获得增量
    dx /= step;
    dy /= step;
    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for(int i = 0; i < step; i++)
    {
        // 屏幕坐标映射到NDC
        glVertex2f(2.0 * x / WIDTH, 2.0 * y / HEIGHT);
        x += dx;
        y += dy;
    }
    glEnd();
}

/*Breakenham算法
对于点x0, 若1*k<0.5则舍去,否则+1
推广到xi点为i*k<i-0.5;等价于 (2i)dy-(2i-1)dx<0
采用整数计算,通过判断决策变量p的符号来决定下一个点的位置
优势是不需要进行浮点计算,提高了计算效率
*/
void Bresenham(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int x = x1;
    int y = y1;
    // 确定线条增长方向
    int s1 = x2 > x1 ? 1 : -1;
    int s2 = y2 > y1 ? 1 : -1;

    bool change = false;
    // 交换xy,以绘制斜率大于1的直线
    if (dy > dx)
    {
        std::swap(dx, dy);
        change = true;
    }

    // 推导得的关键判别式
    int p = 2 * dy - dx;
    glBegin(GL_POINTS);
    for (int i = 0; i <= dx; i++)
    {
        glVertex2f(2.0 * x / WIDTH, 2.0 * y / HEIGHT);
        while (p >= 0)
        {
            if (change)
                x += s1;
            else
                y += s2;
            p -= 2 * dx;
        }
        if (change)
            y += s2;
        else
            x += s1;
        p += 2 * dy;
    }
    glEnd();
}

void MidPoint(int x1, int y1, int x2, int y2)
{   
    int dx = x2 - x1;
    int dy = y1 - y2;
    int d = 2 * dy + dx;
    int d1 = 2 * dy;
    int d2 = 2 * (dy + dx);
    int x = x1;
    int y = y1;

    glBegin(GL_POINTS);
    glVertex2f(2.0 * x / WIDTH, 2.0 * y / HEIGHT);
    while (x < x2)
    {
        if (d <= 0)
        {
            x++;
            y++;
            d += d2;
        }
        else
        {
            x++;
            d += d1;
        }
        glVertex2f(2.0 * x / WIDTH, 2.0 * y / HEIGHT);
    }
    glEnd();
}

void drawCurve(LineType draw, ShapeType shape, std::vector<int> &points)
{
    for (int i = 0; i < points.size() - 2; i += 2)
    {
        if (draw == LineType::DDA)
            DDA(points[i], points[i + 1], points[i + 2], points[i + 3]);
        else if (draw == LineType::BRESENHAM)
            Bresenham(points[i], points[i + 1], points[i + 2], points[i + 3]);
        else if (draw == LineType::MIDPOINT)
            MidPoint(points[i], points[i + 1], points[i + 2], points[i + 3]);
    }

    // 闭合曲线
    if (shape == ShapeType::MY_LINE_LOOP)
    {
        if(draw == LineType::DDA)
            DDA(points[points.size() - 2], points[points.size() - 1], points[0], points[1]);
        else if(draw == LineType::BRESENHAM)
            Bresenham(points[points.size() - 2], points[points.size() - 1], points[0], points[1]);
        else if(draw == LineType::MIDPOINT)
            MidPoint(points[points.size() - 2], points[points.size() - 1], points[0], points[1]);
    }
}

void scanLine(std::vector<int> &vertices)
{
    // 会自下而上扫描整个屏幕空间，并计算与各个存在的边线的交点，共循环HEIGHT次
    // *HEIGHT为窗口高度
    for (int i = -(HEIGHT / 2); i < HEIGHT / 2; i++)
    {
        std::set<int> xset;
        int y = i;
        int x = 0;
        // 对于一条扫描线，遍历处理全部可能存在的边线，计算交点，并存储在set中
        for (int j = 0; j < vertices.size() / 2; j++)
        {
            int p1 = j % (vertices.size()/2);
            int p2 = (j + 1) % (vertices.size()/2);

            int y1 = vertices[p1 * 2 + 1];
            int y2 = vertices[p2 * 2 + 1];
            // 若扫描线与边线平行，则跳过
            if(y1 == y2)
                continue;
            // 若扫描线不与边线相交，则跳过
            if (!(y > std::min(y1, y2) && y <= std::max(y1, y2)))
                continue;
            int x1 = vertices[p1 * 2 + 0];
            int x2 = vertices[p2 * 2 + 0];
            // 计算交点
            x = x1 + (y - y1) * (x2 - x1) / (y2 - y1);
            xset.insert(x);
        }

        // 暂时只支持处理凸图形，默认为两个交点
        // 为便于扩展到凹图形，采用了set来存储交点
        if(xset.size() == 0)
            continue;
        glBegin(GL_POINTS);
        glColor3f(0.1f, 0.5f, 1.0f);
        for (int q = *xset.begin(); q < *xset.rbegin(); q++)
        {
            glVertex2f(2.0 * q / WIDTH, 2.0 * y / HEIGHT);
        }
        glEnd();
    }
} 