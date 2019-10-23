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

Geometry* Program::createCurve(Geometry* p0, Geometry* p1, Geometry* p2, glm::vec3 color = glm::vec3(0, 1, 0)) {
	Geometry* curve = new Geometry(GL_LINE_STRIP);
	curve->color = color;
	for (double u = 0.0; u <= 1.0; u += 0.01) {
		//double x = p0->verts[0][0] * (1 - u) * (1 - u) + p1->verts[0][0] * 2 * u * (1 - u) + p2->verts[0][0] * u * u;
		//double y = p0->verts[0][1] * (1 - u) * (1 - u) + p1->verts[0][1] * 2 * u * (1 - u) + p2->verts[0][1] * u * u;
		double x = p0->verts[0][0] + p1->verts[0][0] * u + p2->verts[0][0] * u * u;
		double y = p0->verts[0][1] + p1->verts[0][1] * u + p2->verts[0][1] * u * u;
		curve->verts.push_back(glm::vec3(x, y, 0.f));
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

		ImGui::Begin("Hello World!");                          // Create a window called "Hello, world!" and append into it.

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Test Window", &show_test_window);      // Edit bools storing our window open/close state

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);
	
		if (ImGui::Button("Clean")) {                            // Buttons return true when clicked (most widgets return true when edited/activated)
			geometryObjects.clear();
		}
		ImGui::SameLine();
		ImGui::Checkbox("Click me!", &show_test_window);
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

	
	// Our state
	show_test_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	while(!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		while (InputHandler::clickedPositions.size() > 0) {
			if (geometryObjects.size() == 8) {
				geometryObjects.clear();
			}
			std::pair<int, int> point = InputHandler::clickedPositions.back();
			InputHandler::clickedPositions.pop_back();
			double factor = 20.0 / 750.0;
			//std::cout << point.first << " : " << point.second << std::endl;
			createPoint(point.first*factor - 10.0, 10.0 - point.second*factor);
		}

		if (geometryObjects.size() == 3) {
			Geometry* p0 = geometryObjects[0];
			Geometry* p1 = geometryObjects[1];
			Geometry* p2 = geometryObjects[2];
			createCurve(p0, p1, p2);

			double transform_x = 3;
			double transform_y = -3;
			Geometry* p0t = createPoint(p0->verts[0][0] + transform_x, p0->verts[0][1] + transform_y, glm::vec3(1, 1, 0));
			Geometry* p1t = createPoint(p1->verts[0][0] + transform_x, p1->verts[0][1] + transform_y, glm::vec3(1, 1, 0));
			Geometry* p2t = createPoint(p2->verts[0][0] + transform_x, p2->verts[0][1] + transform_y, glm::vec3(1, 1, 0));
			createCurve(p0t, p1t, p2t, glm::vec3(0, 1, 1));
		}

		//drawUI();

		// Rendering
		//ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);
		glPointSize(5);

		renderEngine->render(geometryObjects, glm::mat4(1.f));
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
