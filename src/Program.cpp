#include "Program.h"


Program::Program() {
	window = nullptr;
	renderEngine = nullptr;
}

// Error callback for glfw errors
void Program::error(int error, const char* description) {
	std::cerr << description << std::endl;
}

// Called to start the program
void Program::start() {
	setupWindow();
	GLenum err = glewInit();
	if (glewInit() != GLEW_OK) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	/*
	bool err = gl3wInit() != 0;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
	}
	*/
	renderEngine = new RenderEngine(window);
	InputHandler::setUp(renderEngine);
	mainLoop();
}

// Creates GLFW window for the program and sets callbacks for input
void Program::setupWindow() {
	glfwSetErrorCallback(Program::error);
	if (glfwInit() == 0) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_SAMPLES, 16);
	window = glfwCreateWindow(749, 749, "589 Boilerplate", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // V-sync on

	glfwSetKeyCallback(window, InputHandler::key);
	glfwSetMouseButtonCallback(window, InputHandler::mouse);
	glfwSetCursorPosCallback(window, InputHandler::motion);
	glfwSetScrollCallback(window, InputHandler::scroll);
	glfwSetWindowSizeCallback(window, InputHandler::reshape);

	// Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

	const char* glsl_version = "#version 430 core";

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

// Creates an object from specified vertices - no texture. Default object is a 2D triangle.
void Program::createTestGeometryObject() {
	Geometry* testObject = new Geometry();

	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	testObject->verts.push_back(glm::vec3(0.f, 5.f, 0.f));
	testObject->color = glm::vec3(1, 0, 0);
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

void Program::createTestGeometryObject1() {
	Geometry* testObject = new Geometry(GL_POINTS);
	glPointSize(5);
	glColor3f(1, 0, 0);
	testObject->verts.push_back(glm::vec3(-5.f, -3.f, 0.f));
	//testObject->verts.push_back(glm::vec3(5.f, -3.f, 0.f));
	renderEngine->assignBuffers(*testObject);
	renderEngine->updateBuffers(*testObject);
	geometryObjects.push_back(testObject);
}

Geometry* Program::createPoint(float x, float y, glm::vec3 color = glm::vec3(1, 0, 0)) {
	Geometry* point = new Geometry(GL_POINTS);
	point->color = color;
	point->verts.push_back(glm::vec3(x, y, 0.f));
	renderEngine->assignBuffers(*point);
	renderEngine->updateBuffers(*point);
	geometryObjects.push_back(point);
	return point;
}

void Program::updateKnotValues(int m, int k) {
	Program::U.clear();
	for (int i = 0; i < k; i++) {
		Program::U.push_back(0.0);
	}
	double step = 1.0 / (m - k + 2);
	for (int i = 1; i < m - k + 2; i++) {
		Program::U.push_back(Program::U.back() + step);
	}
	for (int i = 0; i < k; i++) {
		Program::U.push_back(1.0);
	}
}

int Program::delta(double u, int k, int m) {
	// m = number of control points - 1
	// k = order
	for (int i = 0; i < m + k; i++) {
		if (u >= Program::U[i] && u < Program::U[i + 1]) {
			return i;
		}
	}
	return -1;
}

std::pair<double, double> Program::E_delta_1(double u, int k, int m) {
	int d = Program::delta(u, k, m);
	std::vector<std::pair<double, double>> C;
	for (int i = 0; i < k; i++) {
		C.push_back(Program::controlPoints[d - i]);
	}
	for (int r = k; r >= 2; r--) {
		int i = d;
		for (int s = 0; s <= r - 2; s++) {
			double omega = (u - Program::U[i]) / (Program::U[i + r - 1] - Program::U[i]);
			C[s].first = omega * C[s].first + (1 - omega) * C[s + 1].first;
			C[s].second = omega * C[s].second + (1 - omega) * C[s + 1].second;
			i--;
		}
	}
	return C[0];
}

Geometry* Program::createCurve(glm::vec3 color = glm::vec3(0, 1, 0)) {
	if (Program::controlPoints.size() < order) {
		return NULL;
	}
	Geometry* curve = new Geometry(GL_LINE_STRIP);
	curve->color = color;
	for (double u = 0.0; u < 1.0; u += Program::u_inc) {
		std::pair<float, float> res = Program::E_delta_1(u, order, Program::controlPoints.size() - 1);
		curve->verts.push_back(glm::vec3(res.first, res.second, 0.f));
	}
	renderEngine->assignBuffers(*curve);
	renderEngine->updateBuffers(*curve);
	geometryObjects.push_back(curve);
	return curve;
}


void Program::drawUI() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		ImGui::Begin("B-Spline!");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Test Window", &show_test_window);      // Edit bools storing our window open/close state

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//ImGui::SameLine();
		ImGui::InputInt("order", &order);
		ImGui::SliderFloat("step size", &u_inc, 0.001f, 1.0f);
		ImGui::SliderFloat("animation", &u_animate, 0.0f, 1.0f);
	
		if (ImGui::Button("Clean")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
			Program::controlPoints.clear();
		}
		ImGui::SameLine();
		//ImGui::Checkbox("Click me!", &show_test_window);
		ImGui::End();
	}
	// 3. Show another simple window.
	if (show_test_window)
	{
		ImGui::Begin("Another Window", &show_test_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("This is an example popup window.");
		if (ImGui::Button("Close Me"))
			show_test_window = false;
		ImGui::End();
	}
}

// Main loop
void Program::mainLoop() {
	//createTestGeometryObject();
	//createPoint(0, 0);
	//createPoint(-10, 10);
	order = 4;
	u_inc = 0.001;
	u_animate = 0;
	
	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		while (InputHandler::rightClickedPositions.size() > 0) {
			/**
			if (geometryObjects.size() == 8) {
				geometryObjects.clear();
			}/**/

			std::pair<int, int> point = InputHandler::rightClickedPositions.back();
			InputHandler::rightClickedPositions.pop_back();
			double factor = 20.0 / 750.0;
			//std::cout << point.first << " : " << point.second << std::endl;
			float x = point.first*factor - 10.0;
			float y = 10.0 - point.second*factor;

			Program::controlPoints.push_back(std::make_pair(x, y));
		}
		while (InputHandler::clickedPositions.size() > 0) {
			std::pair<int, int> point = InputHandler::clickedPositions.back();
			InputHandler::clickedPositions.pop_back();
			double factor = 20.0 / 750.0;
			//Do remore the control point
			double epsilon = 0.1;
			int delete_index = -1;
			float x = point.first * factor - 10.0;
			float y = 10.0 - point.second * factor;
			for (int i = 0; i < Program::controlPoints.size(); i++) {
				if (abs(x - Program::controlPoints[i].first) < epsilon && abs(y - Program::controlPoints[i].second) < epsilon) {
					delete_index = i;
					break;
				}
			}
			if (delete_index > -1){
				Program::controlPoints.erase(Program::controlPoints.begin() + delete_index);
			}
		}

		Program::updateKnotValues(Program::controlPoints.size() - 1, order);
		geometryObjects.clear();
		for (int i = 0; i < Program::controlPoints.size(); i++){
			float x = Program::controlPoints[i].first;
			float y = Program::controlPoints[i].second;
			createPoint(x, y);
		}
		createCurve();

		drawUI();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(5);

		renderEngine->render(geometryObjects, glm::mat4(1.f));
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	// Clean up, program needs to exit
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	delete renderEngine;
	glfwTerminate();
}
