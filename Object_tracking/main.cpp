//尝试设置场景和坐标,按键控制小方块6DOF运动+尝试opencv
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//===================
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

//蓝颜色的HSV范围  
int iLowH = 100;
int iHighH = 130;

int iLowS = 110;
int iHighS = 255;

int iLowV = 110;
int iHighV = 255;
//=====================

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// 设置
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

float factor = 0.2f;	

//定义好的36个顶点都列出来（6个面 * 每个面2个三角形 * 
//每个三角形3个顶点），每个面顶点都包含了纹理坐标
//未使用索引EBO！！！
float vertices[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//=
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//==
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,//==
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,//=

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f
};

//定义10个位置
glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
};



float pitch = 0.0;
float yaw = 0.0;
float roll = 0.0;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//获取最大属性数量
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "最大支持的属性数量为：" << nrAttributes << std::endl;

	//启用深度测试
	//OpenGL存储深度信息在一个叫做Z缓冲(Z-buffer)的缓冲中，它允许OpenGL决定何时覆盖一个像素而何时不覆盖。
	glEnable(GL_DEPTH_TEST);

	//！创建我们的着色器
	Shader shader("Shader282.vs", "Shader282.fs");

	//第一个环境====================================================
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//顶点坐标
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//纹理坐标
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//纹理1
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	//设置纹理包装和过滤的方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(/*"zhou_yu.bmp"*/"../container.jpg", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "无法加载问题，请检查代码或资源是否有误。" << std::endl;
	stbi_image_free(data);

	//纹理2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glActiveTexture(GL_TEXTURE1);	//激活纹理1对象
	glBindTexture(GL_TEXTURE_2D, texture2);
	//设置纹理环绕和过滤的方式
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//加载图片
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data2 = stbi_load("../awesomeface.png", &width, &height, &nrChannels, 0);
	if (data2) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "无法加载问题，请检查代码或资源是否有误。" << std::endl;
	stbi_image_free(data2);

	//告诉OpenGL哪个采样器属于哪个纹理单元
	shader.use();
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);


	//观察矩阵（Z方向的平移？）
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -8.0f));

	//投影矩阵（透视投影）
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	//==========
	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())
	{
		cout << "不能初始化摄像头\n";
	}
	namedWindow("frame");
	namedWindow("frame_hsv");
	namedWindow("mask");

	Mat frame, frame_hsv, mask;

	Mat g_cannyMat_output;//输出图
	vector<vector<Point>> g_vContours;//【轮廓】
	vector<Vec4i> g_vHierarchy;//层次
	int area, max_area, max_index;
	vector<Point> pointsf;
	//===================

	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//因为使用了深度测试，要在每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shader.use();
		glBindVertexArray(VAO);//==========

		//1个盒子移动
		glm::mat4 model;
		model = glm::translate(model, cubePositions[0]);
		model = glm::rotate(model, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));//x 俯仰
		model = glm::rotate(model, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));//y 航向
		model = glm::rotate(model,glm::radians(roll), glm::vec3(0.0f, 0.0f, 0.1f));//z
		shader.setMat4("model", glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);

		shader.setMat4("view", glm::value_ptr(view));//观察矩阵
		shader.setMat4("projection", glm::value_ptr(projection));//投影矩阵

		//地面=========
		glm::mat4 model3;
		model3 = glm::translate(model3, glm::vec3(0.0f, -3.0f, -10.0f));
		model3 = glm::rotate(model3, glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
		model3 = glm::scale(model3, glm::vec3(25.5, 25.5, 1.0));
		shader.setMat4("model", glm::value_ptr(model3));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();


		//=====================
		cap >> frame;
		flip(frame, frame, 1);

		cvtColor(frame, frame_hsv, COLOR_BGR2HSV);
		inRange(frame_hsv, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), mask);
		imshow("frame_hsv", frame_hsv);

		//开操作 (去除一些噪点)  如果二值化后图片干扰部分依然很多，增大下面的size  
		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
		morphologyEx(mask, mask, MORPH_OPEN, element);
		//闭操作 (连接一些连通域)    
		morphologyEx(mask, mask, MORPH_CLOSE, element);

		// 【寻找轮廓】
		findContours(mask, g_vContours, g_vHierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE, Point(0, 0));

		max_area = 0;
		if (g_vContours.size() > 0)
		{
			for (int i = 0; i < g_vContours.size(); i++)
			{
				area = (int)(fabs(contourArea(g_vContours[i], true)));
				if (area > max_area)
				{
					max_area = area;
					max_index = i;//最大面积索引
				}
			}
			//cout << max_area << endl;
			if (max_area > 2000)//【最大面积太小】
			{
				pointsf = g_vContours[max_index];
				RotatedRect box = fitEllipse(pointsf);
				//====================800 600
				cout << box.center.x << box.center.y << box.size.width << box.size.height << endl;
				//=================【更新位置】==================
				cubePositions[0].x = (box.center.x-400)/400*2;
				cubePositions[0].y = -(box.center.y-300)/300*2;
				cubePositions[0].z = (box.size.width + box.size.height - 200) / 100;

				ellipse(frame, box, Scalar(0, 0, 255), 5);
				//drawContours(frame, g_vContours, max_index, Scalar(255, 0, 0), 2, 8, g_vHierarchy, 0, Point());
			}

		}

		imshow("frame", frame);
		imshow("mask", mask);

		char c = waitKey(10);
		if (c == 27)
			break;
		//==================
	}

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)//前
		cubePositions[0].z -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)//后
		cubePositions[0].z += 0.01;

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)//左
		cubePositions[0].x -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)//右
		cubePositions[0].x += 0.01;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)//上
		cubePositions[0].y += 0.01;

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)//下
		cubePositions[0].y -= 0.01;

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)//
	{
		pitch += 0.1;
		if (pitch >= 35) pitch = 35;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)//
	{
		pitch -= 0.1;
		if (pitch <= -35) pitch = -35;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)//
	{
		yaw += 0.1;
		if (yaw >= 35) yaw = 35;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)//
	{
		yaw -= 0.1;
		if (yaw <= -35) yaw = -35;
	}
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)//
	{
		roll += 0.1;
		if (roll >= 35) roll = 35;
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)//
	{
		roll -= 0.1;
		if (roll <= -35) roll = -35;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
