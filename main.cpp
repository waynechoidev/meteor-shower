#include <iostream>
#include <vector>
#include <random>

#include <GL\glew.h>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "Window.h"
#include "Program.h"
#include "Texture.h"

const GLint WIDTH = 1920;
const GLint HEIGHT = 1080;
const GLint NUM_OF_PARTICLES = 300;

std::vector<GLfloat> createPoints(GLint num, const std::vector<GLfloat>& mins, const std::vector<GLfloat>& maxs);

int main()
{
	// Create window
	Window mainWindow = Window(WIDTH, HEIGHT);
	mainWindow.initialise();

	// Create programs
	Program drawProgram = Program();
	drawProgram.createFromFiles("draw.vert", "draw.geom", "draw.frag");
	GLuint projectionLoc = glGetUniformLocation(drawProgram.getID(), "projection");
	GLuint resolutionLoc = glGetUniformLocation(drawProgram.getID(), "resolution");
	GLuint currentTimeLoc = glGetUniformLocation(drawProgram.getID(), "currentTime");

	Program computeProgram = Program();
	computeProgram.createFromFiles("compute.vert", "compute.frag");
	const GLchar* feedbackVaryings[] = { "newPosition" };
	glTransformFeedbackVaryings(computeProgram.getID(), 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(computeProgram.getID());
	GLuint deltaTimeLoc = glGetUniformLocation(computeProgram.getID(), "deltaTime");
	GLuint windowSizeLoc = glGetUniformLocation(computeProgram.getID(), "windowSize");

	Program bgProgram = Program();
	bgProgram.createFromFiles("bg.vert","bg.frag");
	GLuint zIndexLoc = glGetUniformLocation(bgProgram.getID(), "zIndex");

	// BG Textures
	Texture bgTextureFront = Texture();
	bgTextureFront.initialise("textures/front.png");
	Texture bgTextureBack = Texture();
	bgTextureBack.initialise("textures/back.png");

	// Create vertices
	GLfloat expansionRatio = 1.2f;
	GLfloat expandedWidth = (GLfloat)WIDTH * expansionRatio;
	GLfloat expandedHeight = (GLfloat)HEIGHT * expansionRatio;
	std::vector<GLfloat> positions = createPoints(NUM_OF_PARTICLES, { -expandedWidth / 2.0f, -expandedHeight / 2.0f }, { expandedWidth / 2.0f, expandedHeight / 2.0f });
	std::vector<GLfloat> colors = createPoints(NUM_OF_PARTICLES, { 0.2f, 0.2f, 0.2f }, { 1.0f, 1.0f, 1.0f });
	std::vector<GLfloat> distances = createPoints(NUM_OF_PARTICLES, { 0.05f }, { 0.3f });
	std::vector<GLfloat> speeds = createPoints(NUM_OF_PARTICLES, { 100.0f }, { 500.0f });
	std::vector<GLfloat> bgVertices = {
		// pos				 // tex
		-1.0f, 1.0f,		 0.0f, 0.0f,
		-1.0f, -1.0f,		 0.0f, 1.0f,
		1.0f, 1.0f,			 1.0f, 0.0f,
		1.0f, -1.0f,		 1.0f, 1.0f
	};

	// Generate VBOs
	GLuint positionVBO[2];
	glGenBuffers(2, positionVBO);
	for (GLint i = 0; i < 2; ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, positionVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	GLuint distanceVBO;
	glGenBuffers(1, &distanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, distanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * distances.size(), distances.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint colorVBO;
	glGenBuffers(1, &colorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * colors.size(), colors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint speedVBO;
	glGenBuffers(1, &speedVBO);
	glBindBuffer(GL_ARRAY_BUFFER, speedVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * speeds.size(), speeds.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint bgVBO;
	glGenBuffers(1, &bgVBO);
	glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * bgVertices.size(), bgVertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Generate VAOs
	GLuint drawPositionVAO[2];
	glGenVertexArrays(2, drawPositionVAO);
	for (GLint i = 0; i < 2; ++i) {
		glBindVertexArray(drawPositionVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, positionVBO[i]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, distanceVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glBindVertexArray(0);
	}

	GLuint computePositionVAO[2];
	glGenVertexArrays(2, computePositionVAO);
	for (GLint i = 0; i < 2; ++i) {
		glBindVertexArray(computePositionVAO[i]);

		glBindBuffer(GL_ARRAY_BUFFER, positionVBO[i]);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ARRAY_BUFFER, speedVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));

		glBindVertexArray(0);
	}

	GLuint bgVAO;
	glGenVertexArrays(1, &bgVAO);
	glBindVertexArray(bgVAO);
	glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);

	// Generate Transform feedbacks
	GLuint transformFeedback[2];
	glGenTransformFeedbacks(2, transformFeedback);
	for (int i = 0; i < 2; ++i) {
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, positionVBO[i]);
	}
	
	// Projection
	glm::mat4 projection = glm::ortho(0.0f, (GLfloat)WIDTH, 0.0f, (GLfloat)HEIGHT, -1.0f, 1.0f);

	// Pass static uniforms
	drawProgram.use();
	glUniformMatrix4fv(projectionLoc, (GLsizei)1, GL_FALSE, glm::value_ptr(projection));
	glUniform2f(resolutionLoc, (GLfloat)WIDTH, (GLfloat)HEIGHT);
	computeProgram.use();
	glUniform2f(windowSizeLoc, (GLfloat)WIDTH, (GLfloat)HEIGHT);
	glUseProgram(0);

	GLfloat lastFrameTime = (GLfloat)glfwGetTime();
	GLint swapFlag = 0;
	while (!mainWindow.getShouldClose())
	{
		GLfloat currentFrameTime = (GLfloat)glfwGetTime();
		GLfloat deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		swapFlag = swapFlag ? 0 : 1;

		// Get + Handle user input events
		glfwPollEvents();

		// Clean window
		mainWindow.clear(0.0f, 0.0f, 0.0f, 1.0f);

		// BG
		bgProgram.use();
		glBindVertexArray(bgVAO);

		bgTextureFront.use();
		glUniform1f(zIndexLoc, -1.0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		bgTextureBack.use();
		glUniform1f(zIndexLoc, 0.1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);


		// Compute position
		computeProgram.use();
		glUniform1f(deltaTimeLoc, deltaTime);

		glBindVertexArray(computePositionVAO[swapFlag ? 0 : 1]);
		glEnable(GL_RASTERIZER_DISCARD);

		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedback[swapFlag]);
		glBeginTransformFeedback(GL_POINTS);
		glDrawArrays(GL_POINTS, 0, NUM_OF_PARTICLES);
		glEndTransformFeedback();

		glDisable(GL_RASTERIZER_DISCARD);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);

		glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_TRANSFORM_FEEDBACK_BARRIER_BIT);

		glBindVertexArray(0);

		// Draw
		drawProgram.use();
		glUniform1f(currentTimeLoc, currentFrameTime);

		glBindVertexArray(drawPositionVAO[swapFlag]);
		glDrawArrays(GL_POINTS, 0, NUM_OF_PARTICLES);
		glBindVertexArray(0);

		// Initialise
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		mainWindow.swapBuffers();
	}
}

std::vector<GLfloat> createPoints(GLint num, const std::vector<GLfloat>& mins, const std::vector<GLfloat>& maxs) {
	std::vector<GLfloat> result;

	// Set up random number generator
	std::random_device rd;
	std::mt19937 gen(rd());

	if (mins.size() != maxs.size()) {
		return result;
	}

	for (GLint i = 0; i < num; ++i) {
		for (size_t axis = 0; axis < mins.size(); ++axis) {
			std::uniform_real_distribution<GLfloat> distribution(mins[axis], maxs[axis]);
			result.push_back(distribution(gen));
		}
	}

	return result;
}
