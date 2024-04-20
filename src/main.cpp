#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "common.hpp"

using std::set;
using std::vector;
/*  
    为了便于确定2D图形的点坐标，整个程序的坐标系统采用screen space
    即以屏幕中心为原点，左上角为[-WIDHT/2, HEIGHT/2]
    绘制图形时会将其归一化到NDC空间并映射到viewport坐标系下
*/

// 正弦函数
void createSin()
{
    vector<int> vertices;
    for (int x = -200; x <= 200; x += 5)
    {
        int y = int(100.0 * sin(x * 3.1416f / 180.0));
        vertices.push_back(x);
        vertices.push_back(y);
    }
    drawCurve(LineType::BRESENHAM, ShapeType::MY_LINE_STRIP, vertices);
}

// 矩形
void createRect()
{
    vector<int> vertices;
    vertices.push_back(-100);
    vertices.push_back(-100);
    vertices.push_back(-100);
    vertices.push_back(100);
    vertices.push_back(100);
    vertices.push_back(100);
    vertices.push_back(100);
    vertices.push_back(-100);
    drawCurve(LineType::DDA, ShapeType::MY_LINE_LOOP, vertices);
}

// 圆形
void createSphere()
{
    vector<int> vertices;
    int x0 = 0;         //起始点
    int y0 = 0;
    int Radius = 180;   //半径
    // 360度
    for (int i = 0; i < 360; i += 5)
    {
        int x = int(Radius * 1.0 * cos(i * 3.1416f / 180.0)) + x0;
        int y = int(Radius * 1.0 * sin(i * 3.1416f / 180.0)) + y0;
        vertices.push_back(x);
        vertices.push_back(y);
    }
    drawCurve(LineType::DDA, ShapeType::MY_LINE_LOOP, vertices);
}

void createFillShape()
{
    // 顶点坐标(0,200),(100,0),(-150,-150)的三角形
    vector<int> vertices;
    vertices.push_back(0);
    vertices.push_back(200);
    vertices.push_back(100);    
    vertices.push_back(0);    
    vertices.push_back(-150);    
    vertices.push_back(-150);    
    drawCurve(LineType::BRESENHAM, ShapeType::MY_LINE_LOOP, vertices);
    scanLine(vertices);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Draw A Shape!", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // 绘制图形
        //createSphere();
        //createRect();
        //createSin();
        createFillShape();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}