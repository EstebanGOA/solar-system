/*  by Alun Evans 2016 LaSalle (aevanss@salleurl.edu) */

//include some standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//include OpenGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
vector <tinyobj::shape_t> shapes2;
vector <tinyobj::shape_t> shapes_planet;


//global variables to help us do things
int g_ViewportWidth = 1000; int g_ViewportHeight = 1000; // Default window size, in pixels
double mouse_x, mouse_y; //variables storing mouse position
const vec3 g_backgroundColor(0.2f, 0.2f, 0.2f); // background colour - a GLM 3-component vector



//Global variable for the texture ID

// Earth 
GLuint texture_id;
GLuint texture_id_earth_normal;
GLuint texture_clouds;
// Other planets and moon 

GLuint texture_mercury;
GLuint texture_venus;
GLuint texture_mars;
GLuint texture_jupiter;
GLuint texture_saturn;
GLuint texture_uranus;
GLuint texture_neptune;

GLuint texture_moon;


// Universe / Skybox
GLuint texture_universe;



vec3 g_light_dir(10.0, 0.0, 10.0);

//Variables FPS

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = g_ViewportWidth / 2.0, lastY = g_ViewportHeight / 2.0;
bool firstMouse = true;

float yaw1 = -90.0f;
float pitch1 = 0.0f;


float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 
GLuint g_simpleShader_earth = 0; //shader identifier
GLuint g_Vao_earth = 0; //vao
GLuint g_NumTriangles_earth = 0; //  Numbre of triangles we are painting.

//Variables skyblock 

GLuint g_simpleShader_sky = 0; 
GLuint g_Vao_sky = 0;
GLuint g_NumTriangles_sky = 0;

GLuint g_simpleShader_planet = 0;
GLuint g_Vao_planet = 0;
GLuint g_NumTriangles_planet = 0;

// Struct of planet
typedef struct {
    float rotation;
    float spin;
    GLuint texture;
    vec3 position;
}Planet;



float rot_angle = 0;
float spin_angle = 0;

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
    

    //load the shader

    Shader simpleShader("src/shader.vert", "src/shader.frag");
    g_simpleShader_planet = simpleShader.program;
    Shader simpleShader_earth("src/shader_earth.vert", "src/shader_earth.frag");
    g_simpleShader_earth = simpleShader_earth.program;
    Shader simpleShader_sky("src/shader_sky.vert", "src/shader_sky.frag");
    g_simpleShader_sky = simpleShader_sky.program;

    
    // Earth with bump
   g_Vao_earth = gl_createAndBindVAO();

    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader_earth, "a_vertex", 3);
    gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));
    gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]),
        shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader_earth, "a_normal", 3);

    // STEP 1: create a VBO for the texture coordinates
    gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]), shapes[0].mesh.texcoords.size() * sizeof(GLfloat), g_simpleShader_earth, "a_uv", 2);


    gl_unbindVAO();

    g_NumTriangles_earth = shapes[0].mesh.indices.size() / 3;




    //STEP 7: create the texture object

    Image* image = loadBMP("assets/earthmap1k.bmp");

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_clouds);
    
    Image* image_normal = loadBMP("assets/earthnormal.bmp");

    glGenTextures(1, &texture_id_earth_normal);
    glBindTexture(GL_TEXTURE_2D, texture_id_earth_normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_normal->width, image_normal->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_normal->pixels);


    Image* image_clouds = loadBMP("assets/clouds.bmp");

    glGenTextures(1, &texture_clouds);
    glBindTexture(GL_TEXTURE_2D, texture_clouds);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_clouds->width, image_clouds->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_clouds->pixels);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    // Planets without bump

    g_Vao_planet = gl_createAndBindVAO();

    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader_planet, "a_vertex", 3);
    gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));
    gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]),
        shapes[0].mesh.normals.size() * sizeof(float), g_simpleShader_planet, "a_normal", 3);

    gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]), shapes[0].mesh.texcoords.size() * sizeof(GLfloat), g_simpleShader_planet, "a_uv", 2);

    gl_unbindVAO();

    g_NumTriangles_planet = shapes[0].mesh.indices.size() / 3;




    //Mercury

    Image* image_mercury = loadBMP("assets/mercury.bmp");

    glGenTextures(1, &texture_mercury);
    glBindTexture(GL_TEXTURE_2D, texture_mercury);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_mercury->width, image_mercury->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_mercury->pixels);

    //Venus

    Image* image_venus = loadBMP("assets/venus.bmp");

    glGenTextures(1, &texture_venus);
    glBindTexture(GL_TEXTURE_2D, texture_venus);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_venus->width, image_venus->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_venus->pixels);


    // Mars
   
    Image* image_mars = loadBMP("assets/mars.bmp");

    glGenTextures(1, &texture_mars);
    glBindTexture(GL_TEXTURE_2D, texture_mars);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_mars->width, image_mars->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_mars->pixels);

    // Jupiter

    Image* image_jupiter = loadBMP("assets/jupiter.bmp");

    glGenTextures(1, &texture_jupiter);
    glBindTexture(GL_TEXTURE_2D, texture_jupiter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_jupiter->width, image_jupiter->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_jupiter->pixels);

    // Saturn

    Image* image_saturn = loadBMP("assets/saturn.bmp");

    glGenTextures(1, &texture_saturn);
    glBindTexture(GL_TEXTURE_2D, texture_saturn);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_saturn->width, image_saturn->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_saturn->pixels);

    // Uranus

    Image* image_uranus = loadBMP("assets/uranus.bmp");

    glGenTextures(1, &texture_uranus);
    glBindTexture(GL_TEXTURE_2D, texture_uranus);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_uranus->width, image_uranus->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_uranus->pixels);

    // Neptune 
    
    Image* image_neptune = loadBMP("assets/neptune.bmp");

    glGenTextures(1, &texture_neptune);
    glBindTexture(GL_TEXTURE_2D, texture_neptune);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_neptune->width, image_neptune->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_neptune->pixels);

    // Moon

    Image* image_moon = loadBMP("assets/moonmap1k.bmp");

    glGenTextures(1, &texture_moon);
    glBindTexture(GL_TEXTURE_2D, texture_moon);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_moon->width, image_moon->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_moon->pixels);

    
    
    // Skybox
    
   g_Vao_sky = gl_createAndBindVAO();

    gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), g_simpleShader_sky, "a_vertex", 3);


    gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));


    gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]), shapes[0].mesh.texcoords.size() * sizeof(GLfloat), g_simpleShader_sky, "a_uv", 2);


    gl_unbindVAO();
    
    g_NumTriangles_sky = shapes[0].mesh.indices.size() / 3;

    
    
    Image* image_skyblock = loadBMP("assets/milkyway.bmp");

    glGenTextures(1, &texture_universe);
    glBindTexture(GL_TEXTURE_2D, texture_universe);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_skyblock->width, image_skyblock->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_skyblock->pixels);
    
}

void drawEarth() {
    glUseProgram(g_simpleShader_earth);


    // View matrix

    mat4 view_matrix = glm::lookAt(
        cameraPos,
        cameraPos + cameraFront,
        cameraUp
    );

    // Projection matrix

    mat4 projection_matrix = glm::perspective(
        90.0f,
        1.0f,
        0.1f,
        100.0f
    );

    
    GLuint projection_loc = glGetUniformLocation(g_simpleShader_earth, "u_projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection_matrix));

    GLuint view_loc = glGetUniformLocation(g_simpleShader_earth, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLuint model_loc = glGetUniformLocation(g_simpleShader_earth, "u_model");

    gl_bindVAO(g_Vao_earth);


    mat4 model = translate(mat4(1.0f), vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));



    // color of the mesh
    GLuint colorLoc = glGetUniformLocation(g_simpleShader_earth, "u_color");
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

    GLuint light_loc = glGetUniformLocation(g_simpleShader_earth, "u_light_dir");
    glUniform3f(light_loc, g_light_dir.x, g_light_dir.y, g_light_dir.z);

    GLuint cam_pos_loc = glGetUniformLocation(g_simpleShader_earth, "u_cam_pos");
    glUniform3f(cam_pos_loc, 0.0, 0.0, 0.0);

    GLuint ambient_loc = glGetUniformLocation(g_simpleShader_earth, "u_ambient");
    glUniform3f(ambient_loc, 0.2, 0.2, 0.2);

    GLuint diffuse_loc = glGetUniformLocation(g_simpleShader_earth, "u_diffuse");
    glUniform3f(diffuse_loc, 1.0, 1.0, 1.0);

    GLuint specular_loc = glGetUniformLocation(g_simpleShader_earth, "u_specular");
    glUniform3f(specular_loc, 1.0, 1.0, 1.0);

    GLuint shininess_loc = glGetUniformLocation(g_simpleShader_earth, "u_shininess");
    glUniform1f(shininess_loc, 80.0);

    GLuint u_alpha = glGetUniformLocation(g_simpleShader_earth, "u_alpha");
    glUniform1f(u_alpha, 0.6);  
   

    GLuint u_texture = glGetUniformLocation(g_simpleShader_earth, "u_texture");
    glUniform1i(u_texture, 0);
    glUniform1i(u_texture, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    

    GLuint u_texture_normal = glGetUniformLocation(g_simpleShader_earth, "u_texture_normal");
    glUniform1i(u_texture_normal, 1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_id_earth_normal);
    // Draw to screen
    glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles_earth, GL_UNSIGNED_INT, 0);
}


void drawPlanet(Planet planet) {
    mat4 view_matrix = glm::lookAt(
        cameraPos,
        cameraPos + cameraFront,
        cameraUp
    );

    mat4 projection_matrix = glm::perspective(
        90.0f,
        1.0f,
        0.1f,
        100.0f
    );

    glUseProgram(g_simpleShader_planet);


    GLuint model_loc = glGetUniformLocation(g_simpleShader_planet, "u_model");
    GLuint projection_loc2 = glGetUniformLocation(g_simpleShader_planet, "u_projection");
    glUniformMatrix4fv(projection_loc2, 1, GL_FALSE, glm::value_ptr(projection_matrix));
    GLuint view_loc2 = glGetUniformLocation(g_simpleShader_planet, "u_view");
    glUniformMatrix4fv(view_loc2, 1, GL_FALSE, glm::value_ptr(view_matrix));
    gl_bindVAO(g_Vao_planet);

    rot_angle += radians(1000.0f) / 100;
    spin_angle += radians(1000.0f) / 100;

    vec3 axis = vec3(0.0f, 1.0f, 0.0f);


    mat4 model =   translate(mat4(1.0f), planet.position);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));



    // color of the mesh
    GLuint colorLoc = glGetUniformLocation(g_simpleShader_planet, "u_color");
    glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

    GLuint light_loc = glGetUniformLocation(g_simpleShader_planet, "u_light_dir");
    glUniform3f(light_loc, g_light_dir.x, g_light_dir.y, g_light_dir.z);

    GLuint cam_pos_loc = glGetUniformLocation(g_simpleShader_planet, "u_cam_pos");
    glUniform3f(cam_pos_loc, 0.0, 0.0, 0.0);

    GLuint ambient_loc = glGetUniformLocation(g_simpleShader_planet, "u_ambient");
    glUniform3f(ambient_loc, 0.2, 0.2, 0.2);

    GLuint diffuse_loc = glGetUniformLocation(g_simpleShader_planet, "u_diffuse");
    glUniform3f(diffuse_loc, 1.0, 1.0, 1.0);

    GLuint specular_loc = glGetUniformLocation(g_simpleShader_planet, "u_specular");
    glUniform3f(specular_loc, 1.0, 1.0, 1.0);

    GLuint shininess_loc = glGetUniformLocation(g_simpleShader_planet, "u_shininess");
    glUniform1f(shininess_loc, 80.0);


    GLuint u_texture = glGetUniformLocation(g_simpleShader_planet, "u_texture");
    glUniform1i(u_texture, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    // Draw to screen
    glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles_planet, GL_UNSIGNED_INT, 0);
    
}
void drawSkyblock() {
   
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUseProgram(g_simpleShader_sky);


    GLuint u_model = glGetUniformLocation(g_simpleShader_sky, "u_model");
    GLuint u_view = glGetUniformLocation(g_simpleShader_sky, "u_view");
    GLuint u_projection = glGetUniformLocation(g_simpleShader_sky, "u_projection");
    gl_bindVAO(g_Vao_sky);


    mat4 model_matrix = translate(mat4(1.0f), cameraPos);
    mat4 view_matrix = lookAt(cameraPos, cameraFront + cameraPos, vec3(0, 1, 0));
    mat4 projection_matrix = glm::perspective(120.0f, (float)(g_ViewportWidth / g_ViewportHeight), 0.1f, 50.0f);

    glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model_matrix));
    glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view_matrix));
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection_matrix));


    GLuint u_texture = glGetUniformLocation(g_simpleShader_sky, "u_texture");
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_universe);

    glDrawElements(GL_TRIANGLES, 3 * g_NumTriangles_sky, GL_UNSIGNED_INT, 0);

    

}   

// ------------------------------------------------------------------------------------------
// This function actually draws to screen and called non-stop, in a loop
// ------------------------------------------------------------------------------------------
void draw()
{

    //clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    drawSkyblock();
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // activate shader

    
    Planet moon;
    Planet mercury;
    Planet venus;
    Planet mars;
    Planet jupiter;
    Planet saturn;
    Planet uranus;
    Planet neptune;

    // Position
    moon.position = vec3(5.0f, 0.0f, 0.0f);
    mercury.position = vec3(-10.0f, 0.0f, 0.0f);
    venus.position = vec3(-5.0f, 0.0f, 0.0f);
    mars.position = vec3(10.0f, 0.0f, 0.0f);
    jupiter.position = vec3(15.0f, 0.0f, 0.0f);
    saturn.position = vec3(20.0f, 0.0f, 0.0f);
    uranus.position = vec3(25.0f, 0.0f, 0.0f);
    neptune.position = vec3(30.0f, 0.0f, 0.0f);







    // Assing the texture for each planet
    moon.texture = texture_moon;
    mercury.texture = texture_mercury;
    venus.texture = texture_venus;
    mars.texture = texture_mars;
    jupiter.texture = texture_jupiter;
    saturn.texture = texture_saturn;
    uranus.texture = texture_uranus;
    neptune.texture = texture_neptune;



    


    drawEarth();


    drawPlanet(moon);
    drawPlanet(mercury);
    drawPlanet(venus);
    drawPlanet(mars);
    drawPlanet(jupiter);
    drawPlanet(saturn);
    drawPlanet(uranus);
    drawPlanet(neptune);



    
   
    

    

   

}



// ------------------------------------------------------------------------------------------
// This function is called every time you press a screen
// ------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    //quit
    const float cameraSpeed = 0.1f;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);


    //Movement
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
void mouse_button_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }


    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw1 += xoffset;
    pitch1 += yoffset;



    glm::vec3 front;
    front.x = cos(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    front.y = sin(glm::radians(pitch1));
    front.z = sin(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    cameraFront = glm::normalize(front);
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
    glfwSetCursorPosCallback(window, mouse_button_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //load all the resources

    load();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;


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


