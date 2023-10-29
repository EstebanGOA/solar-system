/*  by Alun Evans 2016 LaSalle (aevanss@salleurl.edu) */

//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>

//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//STEP 5: INCLUDE THE imageloader
#include "imageloader.h"

//include some custom code files
#include "glfunctions.h" //include all OpenGL stuff
#include "Shader.h" // class to compile shaders

#include <string>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;
using namespace glm;

const char filename[] = "assets/sphere.obj";
vector <tinyobj::shape_t> shapes;

//global variables to help us do things
int g_ViewportWidth = 1280; int g_ViewportHeight = 720; // Default window size, in pixels
double mouse_x, mouse_y; //variables storing mouse position
const vec3 g_backgroundColor(0.2f, 0.2f, 0.2f); // background colour - a GLM 3-component vector

//global variables related to lights
vec3 g_light_dir(0.0, 0.0, 0.0);

GLuint g_simpleShader = 0; //shader identifier
GLuint g_simpleShader_sky = 0; // skybox shader identifier
GLuint g_simpleShader_earth = 0;
GLuint g_simpleShader_sun = 0;


// Global values for the skybox
GLuint skyboxVAO = 0; // vao for the skybox
vec3 center = vec3(0.0f, 0.0f, -1.0f);
vec3 eye = vec3(0.0f, 0.0f, 0.0f);

// Global values for the planets
GLuint sunVAO = 0, mercuryVAO = 0, venusVAO = 0, earthVAO = 0, moonVAO = 0, marsVAO = 0, saturnVAO = 0, uranusVAO = 0, neptuneVAO = 0;
GLfloat planetScale = 0.5f;

//STEP 6: add a global variable for the texture ID
GLuint texture_sun, texture_mercury, texture_venus, texture_earth, texture_moon, texture_mars, texture_juptier, texture_saturn, texture_uranus, texture_neptune;
GLuint texture_earth_normal;
GLuint texture_skybox;

// Because all planets use the same obj their are going to have the same number of triangles. 
GLuint numberOfTriangles = 0; //  Numbre of triangles we are painting.

// View Matrix
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
float yawAxis = -90.0f, pitchAxis = 0.0f;
bool firstMouse;
float lastX = g_ViewportWidth / 2, lastY = g_ViewportHeight / 2;


// Struct to store all Planet information
typedef struct {
    GLfloat rotation;
    GLfloat spin;
    GLuint texture;
    GLuint normals;
    vec3 position;
} Planet;

void createVAO(GLuint *VAO, GLuint shader) {
    *VAO = gl_createAndBindVAO();
    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]), shapes[0].mesh.texcoords.size() * sizeof(GLfloat), shader, "a_uv", 2);
    gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), shader, "a_vertex", 3);
    gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), shader, "a_normal", 3);
    gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));
    gl_unbindVAO();
}

void loadTexture(Image *image, GLuint *texture) {
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D( 
        GL_TEXTURE_2D,          // target
        0,                      // level = 0, no mipmap
        GL_RGB,                 // how the data will be stored
        image->width,           // width of the image
        image->height,          // height of the image
        0,                      // border
        GL_RGB,                 // format of original data
        GL_UNSIGNED_BYTE,       // type of data
        image->pixels);         // pointer to the start of data
}

// ------------------------------------------------------------------------------------------
// This function manually creates a square geometry (defined in the array vertices[])
// ------------------------------------------------------------------------------------------
void load()
{
    //**********************
    // CODE TO SET GEOMETRY
    //**********************
    
	bool ret = tinyobj::LoadObj(shapes, filename);

	if (ret)
		cout << "OBJ File: " << filename << " sucessfully loaded\n";
	else
		cout << "OBJ File: " << filename << " cannot be found or is not a valid OBJ\n";


     //**********************
    // CODE TO LOAD EVERYTHING INTO MEMORY
    //**********************

    // Shader for the planets
    Shader simpleShader("src/shader.vert", "src/shader.frag");
    g_simpleShader = simpleShader.program;

    // Shader for the sky box
    Shader simpleSkyShader("src/shader_sky.vert", "src/shader_sky.frag");
    g_simpleShader_sky = simpleSkyShader.program;
    
    // Shader for the sun (because of light) 
    Shader simpleSunShader("src/shader_sun.vert", "src/shader_sun.frag");
    g_simpleShader_sun = simpleSunShader.program;

    // Shader for the earth (because of bumps)
    Shader simpleEarthShader("src/shader_earth.vert", "src/shader_earth.frag");
    g_simpleShader_earth = simpleEarthShader.program;

    // Create the VAOs where we store all geometry realted to planets
    createVAO(&sunVAO, g_simpleShader_sun);
    createVAO(&mercuryVAO, g_simpleShader);
    createVAO(&venusVAO, g_simpleShader);
    createVAO(&earthVAO, g_simpleShader_earth);
    createVAO(&moonVAO, g_simpleShader);
    createVAO(&marsVAO, g_simpleShader);
    createVAO(&saturnVAO, g_simpleShader);
    createVAO(&uranusVAO, g_simpleShader);
    createVAO(&neptuneVAO, g_simpleShader);

    createVAO(&skyboxVAO, g_simpleShader_sky);

    // All planets use the same shape so there is no need for more variables to store triangles
    numberOfTriangles = shapes[0].mesh.indices.size() / 3;
    
    //Create texture objects for each planet
    Image* sunImage = loadBMP("assets/sun.bmp");
    loadTexture(sunImage, &texture_sun);
    
    Image* mercuryImage = loadBMP("assets/mercury.bmp");
    loadTexture(mercuryImage, &texture_mercury);

    Image* venusImage = loadBMP("assets/venus.bmp");
    loadTexture(venusImage, &texture_venus);

    Image* earthImage = loadBMP("assets/earthmap1k.bmp");
    loadTexture(earthImage, &texture_earth);
    
    Image* moonImage = loadBMP("assets/moonmap1k.bmp");
    loadTexture(moonImage, &texture_moon);

    Image* marsImage = loadBMP("assets/mars.bmp");
    loadTexture(marsImage, &texture_mars);

    Image* jupiterImage = loadBMP("assets/jupiter.bmp");
    loadTexture(jupiterImage, &texture_juptier);

    Image* saturnImage = loadBMP("assets/saturn.bmp");
    loadTexture(saturnImage, &texture_saturn);

    Image* uranusImage = loadBMP("assets/uranus.bmp");
    loadTexture(uranusImage, &texture_uranus);

    Image* neptuneImage = loadBMP("assets/neptune.bmp");
    loadTexture(neptuneImage, &texture_neptune);
 
    // Create the texture object for the skybox
    Image* skyboxImage = loadBMP("assets/milkyway.bmp");
    loadTexture(skyboxImage, &texture_skybox);

    // Create the texture object for the earth normals
    Image* earth_normal = loadBMP("assets/earthnormal.bmp");
    loadTexture(earth_normal, &texture_earth_normal);

}

void drawSkybox() {

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUseProgram(g_simpleShader_sky);

    GLuint u_model = glGetUniformLocation(g_simpleShader_sky, "u_model");
    GLuint u_view = glGetUniformLocation(g_simpleShader_sky, "u_view");
    GLuint u_projection = glGetUniformLocation(g_simpleShader_sky, "u_projection");

    gl_bindVAO(skyboxVAO);

    // Set MVP
    mat4 model_matrix = translate(mat4(1.0f), cameraPos);
    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    mat4 projection_matrix = glm::perspective(90.0f, (float)(g_ViewportWidth / g_ViewportHeight), 0.1f, 50.0f);

    // Send all values to shader
    glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    GLuint u_texture = glGetUniformLocation(g_simpleShader_sky, "u_texture");
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_skybox);

    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}


void drawSun() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // activate shader
    glUseProgram(g_simpleShader_sun);

    // SET MVP
    GLuint model_loc = glGetUniformLocation(g_simpleShader_sun, "u_model");
   
    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_sun, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));
    
    GLuint projection_loc = glGetUniformLocation(g_simpleShader_sun, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        50.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    Planet sun;
    sun.position = vec3(0.0f, 0.0f, 0.0f);
    sun.texture = texture_sun;

    gl_bindVAO(sunVAO);

    mat4 model = translate(mat4(1.0f), sun.position);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    GLuint u_texture = glGetUniformLocation(g_simpleShader_sun, "u_texture");
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sun.texture);

    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);
    
}


void drawPlanetWithNormal(Planet planet, GLuint VAO, GLuint model_loc, GLuint u_texture, GLuint u_texture_normal) {

    gl_bindVAO(VAO);
    mat4 model = translate(mat4(1.0f), planet.position) * rotate(mat4(1.0), ((float)glfwGetTime() * 3.0f), vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    glUniform1i(u_texture_normal, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, planet.normals);

    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}

void drawPlanetsWithNormal() {
    
    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // activate shader
    glUseProgram(g_simpleShader_earth);

    // SET PROJECTION MATRIX
    GLuint projection_loc = glGetUniformLocation(g_simpleShader_earth, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        50.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    GLuint model_loc = glGetUniformLocation(g_simpleShader_earth, "u_model");
    // GLuint colorLoc = glGetUniformLocation(g_simpleShader, "u_color"); // Al añadir una textura al objeto no me hace falta añadirle color
    GLuint u_texture = glGetUniformLocation(g_simpleShader_earth, "u_texture");
    GLuint u_texture_normal = glGetUniformLocation(g_simpleShader_earth, "u_texture_normal");

    // update shader with light values.
    GLuint light_loc = glGetUniformLocation(g_simpleShader_earth, "u_light_dir");
    glUniform3f(light_loc, g_light_dir.x, g_light_dir.y, g_light_dir.z);
    GLuint cam_pos_loc = glGetUniformLocation(g_simpleShader_earth, "u_cam_pos");
    glUniform3f(cam_pos_loc, cameraPos.x, cameraPos.y, cameraPos.z);
    GLuint ambient_loc = glGetUniformLocation(g_simpleShader_earth, "u_ambient");
    glUniform3f(ambient_loc, 0.1f, 0.1f, 0.1f);
    GLuint diffuse_loc = glGetUniformLocation(g_simpleShader_earth, "u_diffuse");
    glUniform3f(diffuse_loc, 1.0f, 1.0f, 1.0f);
    GLuint specular_loc = glGetUniformLocation(g_simpleShader_earth, "u_specular");
    glUniform3f(specular_loc, 1.0f, 1.0f, 1.0f);
    GLuint shininess_loc = glGetUniformLocation(g_simpleShader_earth, "u_shininess");
    glUniform1f(shininess_loc, 120.0f);

    // Create planets
    Planet earth;
    earth.position = vec3(0.0f, 0.0f, -5.0f);
    earth.texture = texture_earth;
    earth.normals = texture_earth_normal;

    // Draw planets
    drawPlanetWithNormal(earth, earthVAO, model_loc, u_texture, u_texture_normal);

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_earth, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

}

void drawPlanet(Planet planet, GLuint VAO, GLuint model_loc, GLuint u_texture) {

    gl_bindVAO(VAO);
    mat4 model = translate(mat4(1.0f), planet.position) * rotate( mat4(1.0), ((float) glfwGetTime() * 3.0f), vec3(0.0f, 1.0f, 0.0f) );
    
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    /*
    glUniform1i(u_texture_planet, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, u_texture_planet);
    */

    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}

void drawPlanets() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
   
    // activate shader
    glUseProgram(g_simpleShader);

    // SET PROJECTION MATRIX
    GLuint projection_loc = glGetUniformLocation(g_simpleShader, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float) g_ViewportWidth / (float) g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        50.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    GLuint model_loc = glGetUniformLocation(g_simpleShader, "u_model");
    // GLuint colorLoc = glGetUniformLocation(g_simpleShader, "u_color"); // Al añadir una textura al objeto no me hace falta añadirle color
    GLuint u_texture = glGetUniformLocation(g_simpleShader, "u_texture");

    // update shader with light values.
    GLuint light_loc = glGetUniformLocation(g_simpleShader, "u_light_dir");
    glUniform3f(light_loc, g_light_dir.x, g_light_dir.y, g_light_dir.z);
    GLuint cam_pos_loc = glGetUniformLocation(g_simpleShader, "u_cam_pos");
    glUniform3f(cam_pos_loc, cameraPos.x, cameraPos.y, cameraPos.z);
    GLuint ambient_loc = glGetUniformLocation(g_simpleShader, "u_ambient");
    glUniform3f(ambient_loc, 0.1f, 0.1f, 0.1f);
    GLuint diffuse_loc = glGetUniformLocation(g_simpleShader, "u_diffuse");
    glUniform3f(diffuse_loc, 1.0f, 1.0f, 1.0f);
    GLuint specular_loc = glGetUniformLocation(g_simpleShader, "u_specular");
    glUniform3f(specular_loc, 1.0f, 1.0f, 1.0f);
    GLuint shininess_loc = glGetUniformLocation(g_simpleShader, "u_shininess");
    glUniform1f(shininess_loc, 120.0f);
    
    // Create planets
    Planet mercury;
    mercury.position = vec3(-10.0f, 0.0f, 0.0f);
    mercury.texture = texture_mercury;

    Planet venus;
    venus.position = vec3(-5.0f, 0.0f, 0.0f);
    venus.texture = texture_venus;
    
    Planet moon;
    moon.position = vec3(5.0f, 0.0f, 0.0f);
    moon.texture = texture_moon;

    Planet mars;
    mars.position = vec3(10.0f, 0.0f, 0.0f);
    mars.texture = texture_mars;
    
    Planet jupiter;
    jupiter.position = vec3(15.0f, 0.0f, 0.0f);
    jupiter.texture = texture_juptier;

    Planet saturn;
    saturn.position = vec3(20.0f, 0.0f, 0.0f);
    saturn.texture = texture_saturn;

    Planet uranus;
    uranus.position = vec3(25.0f, 0.0f, 0.0f);
    uranus.texture = texture_uranus;

    Planet neptune;
    neptune.position = vec3(30.0f, 0.0f, 0.0f);
    neptune.texture = texture_uranus;

    GLfloat rotationSpeed = (float) glfwGetTime() * 50;
    
    // Draw plaents without normals (bumps)
    drawPlanet(mercury, mercuryVAO, model_loc, u_texture);
    drawPlanet(venus, venusVAO, model_loc, u_texture);
    drawPlanet(moon, moonVAO, model_loc, u_texture);
    drawPlanet(mars, marsVAO, model_loc, u_texture);
    drawPlanet(saturn, saturnVAO, model_loc, u_texture);
    drawPlanet(uranus, uranusVAO, model_loc, u_texture);
    drawPlanet(neptune, neptuneVAO, model_loc, u_texture);

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

}

// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{

    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSkybox();
    drawSun();
    drawPlanetsWithNormal();
    drawPlanets();

}

// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// ------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    const float cameraSpeed = 0.1f;

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
    //reload
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        load();
    if (key == GLFW_KEY_A)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (key == GLFW_KEY_D)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (key == GLFW_KEY_W)
        cameraPos += cameraSpeed * cameraFront;
    if (key == GLFW_KEY_S)
        cameraPos -= cameraSpeed * cameraFront;
}

// ------------------------------------------------------------------------------------------
// This function is called every time you click the mouse
// ------------------------------------------------------------------------------------------
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        cout << "Left mouse down at" << mouse_x << ", " << mouse_y << endl;
    }
}

void onMouseMove(GLFWwindow* window, double x, double y) {
    if (firstMouse)
    {
        lastX = x;
        lastY = y;
        firstMouse = false;
    }

    float xoffset = x - lastX;
    float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yawAxis += xoffset;
    pitchAxis += yoffset;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yawAxis)) * cos(glm::radians(pitchAxis));
    direction.y = sin(glm::radians(pitchAxis));
    direction.z = sin(glm::radians(yawAxis)) * cos(glm::radians(pitchAxis));
    cameraFront = glm::normalize(direction);
}

int main(void)
{
    //setup window and boring stuff, defined in glfunctions.cpp
    GLFWwindow* window;
    if (!glfwInit())return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(g_ViewportWidth, g_ViewportHeight, "Hello OpenGL!", NULL, NULL);
    if (!window) {glfwTerminate();    return -1;}
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    //input callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Disable mouse when window popup
    glfwSetCursorPosCallback(window, onMouseMove); // Call onMouseMove on every mouse input

    //load all the resources
    load();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        draw();
        
        // Swap front and back buffers
        glfwSwapBuffers(window);
        
        // Poll for and process events
        glfwPollEvents();
        
        //mouse position must be tracked constantly (callbacks do not give accurate delta)
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
    }

    //terminate glfw and exit
    glfwTerminate();
    return 0;
}