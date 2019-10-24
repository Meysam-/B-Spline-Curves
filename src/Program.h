#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <vector>
#include <utility>
#include <math.h>

#include "Geometry.h"
#include "InputHandler.h"
#include "RenderEngine.h"

class Program {

public:
	Program();
	void start();

private:
	GLFWwindow* window;
	RenderEngine* renderEngine;

	std::vector<Geometry*> geometryObjects;
	std::vector<std::pair<double, double>> controlPoints;
	std::vector<double> U; //knot values
	//std::vector<double> E; //control values
	std::vector<std::pair<double, double>> E;//control values

	bool show_test_window;
	ImVec4 clear_color;

	int order; //k
	float u_animate, u_inc; //paramter increments

	static void error(int error, const char* description);
	void setupWindow();
	void mainLoop();
	void drawUI();
	
	int delta(double u, int order, int m);
	std::pair<double, double> E_delta_1(double u, int order, int m);
	void updateKnotValues(int m, int k);
	void createTestGeometryObject();
	void createTestGeometryObject1();
	Geometry* createPoint(float x, float y, glm::vec3 color);
	Geometry* createCurve(glm::vec3 color);
	Geometry* createLine(double x1, double y1, double x2, double y2, glm::vec3 color);
};
