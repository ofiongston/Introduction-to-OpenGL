#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>
#include <GLES2/gl2.h>
#include <SDL2/SDL_image.h>
//#include <GLES2/gl2ext.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Exception.cpp"
#include "cube.cpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace glm;

SDL_Surface* surface = NULL;
SDL_Window* window = NULL; ;
int screenWidth = 1000.0;
int screenHeight = 1500.0;
fstream file("log.txt", ios::out|ios::ate);
unsigned int cubeProg;
/*mat4 projnMatrix(1.0f);
mat4 modelMatrix(1.0f);
mat4 viewMatrix(1.0f);
mat4 idMatrix(1.0f);*/
bool running = true;



void initSDL( ){
    SDL_DisplayMode dm;
    
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
        file << SDL_GetError();
        
    if(SDL_GetCurrentDisplayMode(0, &dm) != 0)
        file << SDL_GetError();
        
    screenWidth = dm.w;
    screenHeight = dm.h;
    
        
    window = SDL_CreateWindow("sdl", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if(window == nullptr)
	    file << SDL_GetError();
	    
	SDL_GLContext context = SDL_GL_CreateContext(window);
	if(context == NULL)
	    file << SDL_GetError();
	    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}



unsigned int getCubeProgram(){
    const char* vs = vertexSrc.c_str(); 
    const char* fs = fragmentSrc.c_str();
        
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(    
                       vertexShader, //GLuint shader
                       1,            //GLsizei count
                       &vs,          //const GLchar **string
                       NULL          //const GLint *length
                   );
    glCompileShader(vertexShader);

    unsigned fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fs, NULL);
    glCompileShader(fragmentShader);

    unsigned int tProg = glCreateProgram();
    glAttachShader(tProg, vertexShader);
    glAttachShader(tProg, fragmentShader);
    glLinkProgram(tProg);

    int linked;
    char info[512];
    glGetProgramiv(tProg, GL_LINK_STATUS, &linked);
    if(!linked){
        glGetProgramInfoLog(tProg, 512, NULL, info);
        /*stringstream error;
        error << "\nProgram error: ";
        error << info;
        error << "\n";
        Exception e( error.str() );
        throw e;*/
        file << "\nPROGRAM LINK EROR:\n" << info << "\n" ;  
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return tProg;
} 



void createPositionBuffer(unsigned int prog){
    unsigned int positionBuffer;
    glGenBuffers(1, &positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, // GLenum target
                 posCoords.size()*sizeof(float), //GLsizeiptr size
                 &posCoords[0], //const void * data
                 GL_STATIC_DRAW //GLenum usage
                 );
    glVertexAttribPointer(glGetAttribLocation(prog, "aPosCoord"), //GLuint index,
                          3, //GLint size,
                          GL_FLOAT, //GLenum type,
                          GL_FALSE, //GLboolean normalized
                          3 * sizeof(float), //GLsizei stride
                          (void*)0 //const void * pointer
                          );
    glEnableVertexAttribArray(glGetAttribLocation(prog, "aPosCoord"));
}



void createTextureBuffer(unsigned int prog){
    unsigned int texBuffer;
    glGenBuffers(1, &texBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texBuffer);
    glBufferData(GL_ARRAY_BUFFER, // GLenum target
                 textureCoordinates.size()*sizeof(float), //GLsizeiptr size
                 &textureCoordinates[0], //const void * data
                 GL_STATIC_DRAW //GLenum usage
                 );
    string err = error(glGetError(), __LINE__);
    
    glVertexAttribPointer(glGetAttribLocation(prog, "aTexCoord"), //GLuint index,
                          2, //GLint size,//components
                          GL_FLOAT, //GLenum type,
                          GL_FALSE, //GLboolean normalized
                          2 * sizeof(float), //GLsizei stride
                          (void*)0 //const void * pointer
                          );
    glEnableVertexAttribArray(glGetAttribLocation(prog, "aTexCoord"));
}



void createIndexBuffer(){
    unsigned int elementBuffer;
    glGenBuffers(1, &elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, // GLenum target
                 indices.size()*sizeof(float), //GLsizeiptr size
                 &indices[0], //const void * data
                 GL_STATIC_DRAW //GLenum usage
                 ); 
}



unsigned int createTextureImage(unsigned int prog){
    SDL_Surface* surface = nullptr;
    unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    //load 2d texture
    string path = "1.png";
	surface = IMG_Load(path.c_str());
    if(surface == NULL){
        const char* er1 = IMG_GetError() ;
        file << er1;
    }
    GLint mode = GL_RGB;
    if(surface->format->BytesPerPixel == 4)
        mode = GL_RGBA;
        
        
    glTexImage2D(GL_TEXTURE_2D,    //GLenum target
                 0,                //GLint mipmap level
                 mode,             //GLint internalFormat
                 surface->w,       //GLsizei width
                 surface->h,       // GLsizei height
                 0,                //GLint border
                 mode,             // GLenum format
                 GL_UNSIGNED_BYTE, //GLenum type
                 surface->pixels   //const void * data
                 );
    glGenerateMipmap(GL_TEXTURE_2D);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    SDL_FreeSurface(surface);
    
    //upload unit to texture
    glUniform1i( glGetUniformLocation(prog, "sampler1"), 0);
    return texture; 
}



int main(){
    initSDL();
    cubeProg = getCubeProgram();
    glUseProgram(cubeProg);
    createPositionBuffer(cubeProg);
    createTextureBuffer(cubeProg);
    unsigned int texture1 = createTextureImage(cubeProg);
  
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, screenWidth, screenHeight);
    glClearColor(1, 1, 1, 1);
    
    //game loop
    
    mat4 projnMatrix(1.0f);
    mat4 modelMatrix(1.0f);
    mat4 viewMatrix(1.0f);
    mat4 idMatrix(1.0f);

	while( running ){
	    SDL_Event e;
	    while( SDL_PollEvent(&e) != 0 ){
	        if(e.type == SDL_FINGERDOWN)
	            running = false;
	    } 
	    
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	   
	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texture1);
	    
	    //transform mesh here
	    float speed = 10;
	    float x = SDL_GetTicks()/speed;
	    
	    projnMatrix = perspective( radians(45.0f), (float)screenWidth/(float)screenHeight, 0.1f, 100.0f ); 
	    modelMatrix = rotate(idMatrix, radians((float) x ), vec3(1.f, 1.0f, 0.0f));
	    modelMatrix = translate(modelMatrix, vec3(0.0f, 0.0f, 0.0f));
	    viewMatrix = translate(idMatrix, vec3(0.0f, 0.0f, -12.0f));
	    
	    //upload uniforms to shader
	    glUniformMatrix4fv(glGetUniformLocation(cubeProg, "projnMatrix"),  //GLint location,
	                      1, //GLsizei count
	                      GL_FALSE, //GLboolean transpose
	                      value_ptr(projnMatrix)  //const GLfloat *value
	                     );
	    glUniformMatrix4fv(glGetUniformLocation(cubeProg, "modelMatrix"),  //GLint location,
	                      1, //GLsizei count
	                      GL_FALSE, //GLboolean transpose
	                      value_ptr(modelMatrix)  //const GLfloat *value
	                     );
	    glUniformMatrix4fv(glGetUniformLocation(cubeProg, "viewMatrix"),  //GLint location,
	                      1, //GLsizei count
	                      GL_FALSE, //GLboolean transpose
	                      value_ptr(viewMatrix)  //const GLfloat *value
	                     );
	    
	   
	    glDrawElements( GL_TRIANGLES ,         //GLenum mode,
                 	   posCoords.size()/2,         //GLsizei count,
 	                   GL_UNSIGNED_INT,       //GLenum type,
 	                   &indices[0]         //const void * indices
 	                      );
	
    
    	SDL_GL_SwapWindow(window); 
          
	    /*int t = SDL_GetTicks();
	    if(t > 5000)
	        running = false;*/
	    //this_thread::sleep_for(milliseconds(300));
	    
	}
	
	SDL_DestroyWindow(window);
	SDL_Quit();   
	IMG_Quit(); 
	
    file.close();
    
}




