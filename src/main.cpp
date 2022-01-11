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

#include "Line.h";

using namespace std;
using namespace glm;

const char filename[] = "assets/sphere.obj";
vector <tinyobj::shape_t> shapes;
vector <tinyobj::shape_t> shapes2;
vector <tinyobj::shape_t> shapes3;




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
GLuint g_simpleShader_clouds = 0;
GLuint g_simpleShader_lines = 0;
GLuint g_simpleShader_circle = 0;
GLuint g_simpleShader_ring = 0;
GLuint g_simpleShader_rock = 0;

// Global values for the skybox
GLuint skyboxVAO = 0; // vao for the skybox
vec3 center = vec3(0.0f, 0.0f, -1.0f);
vec3 eye = vec3(0.0f, 0.0f, 0.0f);

// Global values for the planets
GLuint sunVAO = 0, mercuryVAO = 0, venusVAO = 0, earthVAO = 0, moonVAO = 0, marsVAO = 0, saturnVAO = 0, jupiterVAO = 0, uranusVAO = 0, neptuneVAO = 0;
GLuint cloudsVAO = 0;
GLuint linesVAO = 0;

// VAOs for the line of each planet direction
GLuint mercuryLineVAO = 0, venusLineVAO = 0, earthLineVAO = 0, moonLineVAO = 0, marsLineVAO = 0, saturnLineVAO = 0, jupiterLineVAO = 0, uranusLineVAO = 0, neptuneLineVAO = 0;

// VAO for the rings of jupiter
GLuint ringVAO = 0;

GLuint rockVAO = 0;

//STEP 6: add a global variable for the texture ID
GLuint texture_sun, texture_mercury, texture_venus, texture_earth, texture_moon, texture_mars, texture_jupiter, texture_saturn, texture_uranus, texture_neptune;
GLuint texture_earth_normal;
GLuint texture_earth_clouds;
GLuint texture_earth_night;
GLuint texture_earth_spec;
GLuint texture_skybox;
GLuint texture_line;
GLuint texture_ring;

// Because all planets use the same obj their are going to have the same number of triangles. 
GLuint numberOfTriangles = 0; //  Numbre of triangles we are painting.
GLuint numberOfLines = 0;

GLuint numberOfTriangles2 = 0;
GLuint numberOfTriangles3 = 0;

// View Matrix
vec3 cameraPos = vec3(0.0f, 4.0f, 15.0f);
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
    GLuint texture_night;
    GLuint normals;
    GLuint specular;
    vec3 position;
    vec3 scale;
    GLfloat orbit;
} Planet;


// Create planets
Planet sun;
Planet mercury;
Planet venus;
Planet moon;
Planet earth;
Planet mars;
Planet jupiter;
Planet saturn;
Planet uranus;
Planet neptune;
Planet clouds;
Planet rock;


void createVAO(GLuint *VAO, GLuint shader) {
    *VAO = gl_createAndBindVAO();
    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes[0].mesh.texcoords[0]), shapes[0].mesh.texcoords.size() * sizeof(GLfloat), shader, "a_uv", 2);
    gl_createAndBindAttribute(&(shapes[0].mesh.positions[0]), shapes[0].mesh.positions.size() * sizeof(float), shader, "a_vertex", 3);
    gl_createAndBindAttribute(&(shapes[0].mesh.normals[0]), shapes[0].mesh.normals.size() * sizeof(float), shader, "a_normal", 3);
    gl_createIndexBuffer(&(shapes[0].mesh.indices[0]), shapes[0].mesh.indices.size() * sizeof(unsigned int));
    gl_unbindVAO();
}

void createVAO2(GLuint* VAO, GLuint shader) {
    *VAO = gl_createAndBindVAO();
    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes2[0].mesh.texcoords[0]), shapes2[0].mesh.texcoords.size() * sizeof(GLfloat), shader, "a_uv", 2);
    gl_createAndBindAttribute(&(shapes2[0].mesh.positions[0]), shapes2[0].mesh.positions.size() * sizeof(float), shader, "a_vertex", 3);
    gl_createAndBindAttribute(&(shapes2[0].mesh.normals[0]), shapes2[0].mesh.normals.size() * sizeof(float), shader, "a_normal", 3);
    gl_createIndexBuffer(&(shapes2[0].mesh.indices[0]), shapes2[0].mesh.indices.size() * sizeof(unsigned int));
    gl_unbindVAO();
}

void createVAO3(GLuint* VAO, GLuint shader) {
    *VAO = gl_createAndBindVAO();
    // Bind the vectors/arrays to the attributes in the shaders
    gl_createAndBindAttribute(&(shapes3[0].mesh.texcoords[0]), shapes3[0].mesh.texcoords.size() * sizeof(GLfloat), shader, "a_uv", 2);
    gl_createAndBindAttribute(&(shapes3[0].mesh.positions[0]), shapes3[0].mesh.positions.size() * sizeof(float), shader, "a_vertex", 3);
    gl_createAndBindAttribute(&(shapes3[0].mesh.normals[0]), shapes3[0].mesh.normals.size() * sizeof(float), shader, "a_normal", 3);
    gl_createIndexBuffer(&(shapes3[0].mesh.indices[0]), shapes3[0].mesh.indices.size() * sizeof(unsigned int));
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
    bool ret2 = tinyobj::LoadObj(shapes2,"assets/plane.obj");
    bool ret3 = tinyobj::LoadObj(shapes3, "assets/rock2.obj");


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

    // Shader for the clouds (because of transparency) 
    Shader simpleCloudsShader("src/shader_clouds.vert", "src/shader_clouds.frag");
    g_simpleShader_clouds = simpleCloudsShader.program;

    Shader simpleLinesShader("src/shader_line.vert", "src/shader_line.frag");
    g_simpleShader_lines = simpleLinesShader.program;
    
    Shader simpleCircleShader("src/shader_circle.vert", "src/shader_circle.frag");
    g_simpleShader_circle = simpleCircleShader.program;

    Shader simpleSaturnRingShader("src/shader_ring.vert", "src/shader_ring.frag");
    g_simpleShader_ring = simpleSaturnRingShader.program;

    Shader simpleRockShader("src/shader_rock.vert", "src/shader_rock.frag");
    g_simpleShader_rock = simpleRockShader.program;
    

    // Create the VAOs where we store all geometry realted to the planets
    createVAO(&sunVAO, g_simpleShader_sun);
    createVAO(&mercuryVAO, g_simpleShader);
    createVAO(&venusVAO, g_simpleShader);
    createVAO(&earthVAO, g_simpleShader_earth);
    createVAO(&moonVAO, g_simpleShader);
    createVAO(&marsVAO, g_simpleShader);
    createVAO(&jupiterVAO, g_simpleShader);
    createVAO(&saturnVAO, g_simpleShader);
    createVAO(&uranusVAO, g_simpleShader);
    createVAO(&neptuneVAO, g_simpleShader);

    // Create the VA0s where we store all geometry related to the lines
    createVAO(&mercuryLineVAO, g_simpleShader_circle);
    createVAO(&venusLineVAO, g_simpleShader_circle);
    createVAO(&earthLineVAO, g_simpleShader_circle);
    createVAO(&moonLineVAO, g_simpleShader_circle);
    createVAO(&marsLineVAO, g_simpleShader_circle);
    createVAO(&jupiterLineVAO, g_simpleShader_circle);
    createVAO(&saturnLineVAO, g_simpleShader_circle);
    createVAO(&uranusLineVAO, g_simpleShader_circle);
    createVAO(&neptuneLineVAO, g_simpleShader_circle);

    // Create the VAO where we store all geometry realted to the skybox
    createVAO(&skyboxVAO, g_simpleShader_sky);

    // Create the VAO where we store all geometry realted to the clouds that we are going to use in the earth
    createVAO(&cloudsVAO, g_simpleShader_earth);

    // All planets use the same shape so there is no need for more variables to store triangles
    numberOfTriangles = shapes[0].mesh.indices.size() / 3;
    
    createVAO2(&ringVAO, g_simpleShader_ring);
    numberOfTriangles2 = shapes2[0].mesh.indices.size() / 3;

    createVAO3(&rockVAO, g_simpleShader);
    numberOfTriangles3 = shapes3[0].mesh.indices.size() / 3;

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
    loadTexture(jupiterImage, &texture_jupiter);

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

    Image* earth_clouds = loadBMP("assets/2k_earth_clouds.bmp");
    loadTexture(earth_clouds, &texture_earth_clouds);
      
    Image* earth_night = loadBMP("assets/earthnight.bmp");
    loadTexture(earth_night, &texture_earth_night);

    Image* line = loadBMP("assets/circle.bmp");
    loadTexture(line, &texture_line);

    Image* ring = loadBMP("assets/saturn_rings.bmp");
    loadTexture(ring, &texture_ring);

    Image* earth_specular = loadBMP("assets/earthspec.bmp");
    loadTexture(earth_specular, &texture_earth_spec);

    // Earth
    earth.position = vec3(16.0f, 16.0f, 0.0f);
    earth.scale = vec3(0.5f, 0.5f, 0.5f);
    earth.texture = texture_earth;
    earth.texture_night = texture_earth_night;
    earth.normals = texture_earth_normal;
    earth.specular = texture_earth_spec;
    earth.orbit = 169.0f;

    // Sun
    sun.position = vec3(0.0f, 0.0f, 0.0f);
    sun.scale =  earth.scale * 15.0f;
    sun.texture = texture_sun;
    sun.orbit = 0.0f;

    // Mercury
    mercury.position = vec3(10.0f, 10.0f, 0.0f);
    mercury.texture = texture_mercury;
    mercury.scale = earth.scale * 0.40f;
    mercury.orbit = 310.0f;
    
    // Venus
    venus.position = vec3(13.0f, 13.0f, 0.0f);
    venus.texture = texture_venus;
    venus.scale = earth.scale * 0.95f;
    venus.orbit = 297.0f;

    // Moon
    moon.position = vec3(1.0f, 1.0f, 0.0f);
    moon.texture = texture_moon;
    moon.scale = earth.scale * 0.30f;
    moon.orbit = 62.0f;

    // Mars
    mars.position = vec3(17.6f, 17.6f, 0.0f);
    mars.texture = texture_mars;
    mars.scale = earth.scale * 0.53f;
    mars.orbit = 114.0f;

    // Jupiter
    jupiter.position = vec3(25.0f, 25.0f, 0.0f);
    jupiter.texture = texture_jupiter;
    jupiter.scale = earth.scale * 5.40f;
    jupiter.orbit = 181.0f;

    // Saturn
    saturn.position = vec3(35.0f, 35.0f, 0.0f);
    saturn.texture = texture_saturn;
    saturn.scale = earth.scale * 4.19f;
    saturn.orbit = 17.0f;

    // Uranus
    uranus.position = vec3(45.0f, 45.0f, 0.0f);
    uranus.texture = texture_uranus;
    uranus.scale = earth.scale * 3.04f;
    uranus.orbit = 45.0f;

    // Neptune
    neptune.position = vec3(55.0f, 55.0f, 0.0f);
    neptune.texture = texture_neptune;
    neptune.scale = earth.scale * 2.88f;
    neptune.orbit = 321.0f;

    // Clouds
    clouds.position = earth.position;
    clouds.texture = texture_earth_clouds;
    clouds.scale = earth.scale * 1.05f;
    clouds.orbit = earth.orbit;

    // Rock
    rock.position = vec3(2.0, 2.0f, 2.0f);
    rock.texture = texture_moon;
    rock.scale = vec3(0.0007f, 0.0007f, 0.0007f);
    rock.orbit = 5.0f;

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
        500.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

   
    gl_bindVAO(sunVAO);

    mat4 model = translate(mat4(1.0f), sun.position) * scale(mat4(1.0f), sun.scale);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    GLuint u_texture = glGetUniformLocation(g_simpleShader_sun, "u_texture");
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sun.texture);

    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);
    
}

void drawPlanetWithNormal(Planet &planet, Planet &around, GLuint VAO, GLuint model_loc, GLuint u_texture, GLuint u_texture_normal) {

    gl_bindVAO(VAO);

    GLfloat around_x = around.position.x * cos(around.orbit / 180 * glm::pi<float>());
    GLfloat around_y = around.position.y * cos(around.orbit / 180 * glm::pi<float>());

    GLfloat planet_x = around_x + planet.position.x * cos(planet.orbit / 180 * glm::pi<float>());
    GLfloat planet_y = around_y + planet.position.y * sin(planet.orbit / 180 * glm::pi<float>());

    planet.orbit += .003;
    if (planet.orbit > 360)
        planet.orbit = 0;


    mat4 model = rotate(mat4(1.0), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * translate(mat4(1.0f), vec3(planet_x, planet_y, 0.0f)) * rotate(mat4(1.0), ((float)glfwGetTime() * 3.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0f), planet.scale);;

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

    glUniform1i(u_texture_normal, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, planet.normals);


    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}

void drawEarth() {
    
    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // activate shader
    glUseProgram(g_simpleShader_earth);

    // Set alpha
    GLuint u_clouds = glGetUniformLocation(g_simpleShader_earth, "u_clouds");
    glUniform1f(u_clouds, 0.0);
    GLuint u_alpha = glGetUniformLocation(g_simpleShader_earth, "u_alpha");
    glUniform1f(u_alpha, 1.0);

    // SET PROJECTION MATRIX
    GLuint projection_loc = glGetUniformLocation(g_simpleShader_earth, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        500.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    GLuint model_loc = glGetUniformLocation(g_simpleShader_earth, "u_model");
    // GLuint colorLoc = glGetUniformLocation(g_simpleShader, "u_color"); // Al añadir una textura al objeto no me hace falta añadirle color
    GLuint u_texture = glGetUniformLocation(g_simpleShader_earth, "u_texture");
    GLuint u_texture_normal = glGetUniformLocation(g_simpleShader_earth, "u_texture_normal");
    // Set texture night.
    GLuint u_texture_night = glGetUniformLocation(g_simpleShader_earth, "u_texture_night");
    // Set specular 
    GLuint u_texture_specular = glGetUniformLocation(g_simpleShader_earth, "u_texture_specular");


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
    glUniform3f(specular_loc, 0.5f, 0.5f, 0.5f);
    GLuint shininess_loc = glGetUniformLocation(g_simpleShader_earth, "u_shininess");
    glUniform1f(shininess_loc, 120.0f);

    // Draw planets
    glUniform1i(u_texture_night, 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, earth.texture_night);

    glUniform1i(u_texture_specular, 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, earth.specular);
    
    drawPlanetWithNormal(earth, sun, earthVAO, model_loc, u_texture, u_texture_normal);

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_earth, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

}

void drawPlanet(Planet &planet, Planet &around, GLuint VAO, GLuint model_loc, GLuint u_texture) {

    gl_bindVAO(VAO);

    GLfloat around_x = around.position.x * cos(around.orbit / 180 * glm::pi<float>());
    GLfloat around_y = around.position.y * sin(around.orbit / 180 * glm::pi<float>());

    GLfloat planet_x = around_x + planet.position.x * cos(planet.orbit / 180 * glm::pi<float>());
    GLfloat planet_y = around_y + planet.position.y * sin(planet.orbit / 180 * glm::pi<float>());

    if (around_x != 0.0f) planet.orbit += .03;
    else planet.orbit += .003;

    if (planet.orbit > 360)
        planet.orbit = 0;

    mat4 model = rotate(mat4(1.0), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * translate(mat4(1.0f), vec3(planet_x, planet_y, 0.0f)) * rotate(mat4(1.0), ((float)glfwGetTime() * 10.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0), planet.scale);
    
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}

void drawR(Planet planet, GLuint VAO, GLuint model_loc, GLuint u_texture) {

    gl_bindVAO(VAO);
    mat4 model = translate(mat4(1.0f), planet.position) * rotate(mat4(1.0), ((float)glfwGetTime() * 3.0f), vec3(0.0f, 1.0f, 0.0f)) * scale(mat4(1.0), planet.scale);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, planet.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles3, GL_UNSIGNED_INT, 0);

}

void drawClouds() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate shader
    glUseProgram(g_simpleShader_earth);
    
    // Set alpha
    GLuint u_clouds = glGetUniformLocation(g_simpleShader_earth, "u_clouds");
    glUniform1f(u_clouds, 1.0);
    GLuint u_alpha = glGetUniformLocation(g_simpleShader_earth, "u_alpha");
    glUniform1f(u_alpha, 0.8);

    // SET MVP
    GLuint model_loc = glGetUniformLocation(g_simpleShader_earth, "u_model");

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_earth, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLuint projection_loc = glGetUniformLocation(g_simpleShader_earth, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        500.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    
    // Draw planets
    GLuint u_texture = glGetUniformLocation(g_simpleShader_earth, "u_texture");

    drawPlanet(clouds, sun, cloudsVAO, model_loc, u_texture);

}

void drawPlanets() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
   
    // activate shader
    glUseProgram(g_simpleShader);

    // Set alpha
    GLuint u_clouds = glGetUniformLocation(g_simpleShader, "u_clouds");
    glUniform1f(u_clouds, 0.0);
    GLuint u_alpha = glGetUniformLocation(g_simpleShader, "u_alpha");
    glUniform1f(u_alpha, 1.0);

    // SET PROJECTION MATRIX
    GLuint projection_loc = glGetUniformLocation(g_simpleShader, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float) g_ViewportWidth / (float) g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        500.0f       // Far plane (distance from camera)
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
    glUniform3f(diffuse_loc, 0.7f, 0.7f, 0.7f);
    GLuint specular_loc = glGetUniformLocation(g_simpleShader, "u_specular");
    glUniform3f(specular_loc, 1.0f, 1.0f, 1.0f);
    GLuint shininess_loc = glGetUniformLocation(g_simpleShader, "u_shininess");
    glUniform1f(shininess_loc, 120.0f);
     
    // Draw plaents without normals (bumps)
    drawPlanet(mercury, sun, mercuryVAO, model_loc, u_texture);
    drawPlanet(venus, sun, venusVAO, model_loc, u_texture);
    drawPlanet(moon, earth, moonVAO, model_loc, u_texture);
    drawPlanet(mars, sun, marsVAO, model_loc, u_texture);
    drawPlanet(jupiter, sun, jupiterVAO, model_loc, u_texture);
    drawPlanet(saturn, sun, saturnVAO, model_loc, u_texture);
    drawPlanet(uranus, sun, uranusVAO, model_loc, u_texture);
    drawPlanet(neptune, sun, neptuneVAO, model_loc, u_texture);


    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

}

void drawLine(Planet planet, Planet around, GLuint VAO, GLuint model_loc, GLuint u_texture) {

    gl_bindVAO(VAO);

    GLfloat planet_x = around.position.x * cos(around.orbit / 180 * glm::pi<float>());
    GLfloat planet_y = around.position.y * sin(around.orbit / 180 * glm::pi<float>());

    mat4 model;
    if (around.position.x != 0) 
        model = rotate(mat4(1.0), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * translate(mat4(1.0f), vec3(planet_x, planet_y, 0.0f)) * rotate(mat4(1.0), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0), vec3(planet.position.x, 2.0f, planet.position.y));
    else 
        model = translate(mat4(1.0f), vec3(planet_x, planet_y, 0.0f)) * scale(mat4(1.0), vec3(planet.position.x, 2.0f, planet.position.y));

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_line);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

}

void drawLines() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);

    // As we want to see both faces of texture disable cull face
    glDisable(GL_CULL_FACE);

    // activate shader
    glUseProgram(g_simpleShader_circle);

    // SET MVP
    GLuint model_loc = glGetUniformLocation(g_simpleShader_circle, "u_model");

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_circle, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLuint projection_loc = glGetUniformLocation(g_simpleShader_circle, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        500.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    // Draw planets
    GLuint u_texture = glGetUniformLocation(g_simpleShader_circle, "u_texture");

    drawLine(mercury, sun, mercuryLineVAO, model_loc, u_texture);
    drawLine(venus, sun, venusLineVAO, model_loc, u_texture);
    drawLine(earth, sun, earthLineVAO, model_loc, u_texture);
    drawLine(moon, earth, moonLineVAO, model_loc, u_texture);
    drawLine(mars, sun, marsLineVAO, model_loc, u_texture);
    drawLine(jupiter, sun, jupiterLineVAO, model_loc, u_texture);
    drawLine(saturn, sun, saturnLineVAO, model_loc, u_texture);
    drawLine(uranus, sun, uranusLineVAO, model_loc, u_texture);
    drawLine(neptune, sun, neptuneLineVAO, model_loc, u_texture);

}

void drawRings() {

    // STEP 4: add depth test
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
  
    // activate shader
    glUseProgram(g_simpleShader_ring);

    // SET MVP
    GLuint model_loc = glGetUniformLocation(g_simpleShader_ring, "u_model");

    mat4 view_matrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    GLuint view_loc = glGetUniformLocation(g_simpleShader_ring, "u_view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view_matrix));

    GLuint projection_loc = glGetUniformLocation(g_simpleShader_ring, "u_projection");
    mat4 projection_matrix = perspective(
        90.0f,      // Field of view
        ((float)g_ViewportWidth / (float)g_ViewportHeight),       // Aspect ratio
        0.1f,       // Near plane (distance from camera)
        500.0f       // Far plane (distance from camera)
    );
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, value_ptr(projection_matrix));

    // Create planets
    Planet ring;
    ring.position = vec3(30.0f, 0.0f, 0.0f);
    ring.texture = texture_ring;
    ring.scale = vec3(7.5f, 7.5f, 7.5f);

    // Draw planets
    GLuint u_texture = glGetUniformLocation(g_simpleShader_ring, "u_texture");

    gl_bindVAO(ringVAO);

    GLfloat saturn_x = saturn.position.x * cos(saturn.orbit / 180 * glm::pi<float>());
    GLfloat saturn_y = saturn.position.y * sin(saturn.orbit / 180 * glm::pi<float>());

    mat4 model = rotate(mat4(1.0), 90.0f, vec3(1.0f, 0.0f, 0.0f)) * translate(mat4(1.0f), vec3(saturn_x, saturn_y, 0.0f)) * rotate(mat4(1.0), 45.0f, vec3(1.0f, 0.0f, 0.0f)) * scale(mat4(1.0), ring.scale);

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
   
    glUniform1i(u_texture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ring.texture);
    glDrawElements(GL_TRIANGLES, 3 * numberOfTriangles, GL_UNSIGNED_INT, 0);

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
    drawEarth();
    drawClouds();
    drawPlanets();
    drawLines();
    drawRings();
    

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