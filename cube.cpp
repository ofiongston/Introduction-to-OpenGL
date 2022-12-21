#include <fstream>
#include <string>
#include <sstream>
#include <vector>



using coordinates = float;
using namespace std; 



const vector<coordinates> posCoords {
  // Front face
    -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,                     
  // Back face
    -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
  // Top face
    -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
  // Bottom face
    -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
  // Right face
    1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f,
  // Left face
    -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f
};



const vector<coordinates> textureCoordinates {
    // Front
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    // Back
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    // Top
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    // Bottom
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    // Right
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    // Left
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
};



const vector<coordinates> vertexNormals {
    // Front
    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    // Back
    0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
    // Top
    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    // Bottom
    0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    // Right
    1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    // Left
    -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
};



const vector<int>  indices {
    0, 1, 2, 0, 2, 3, // front
    4, 5, 6, 4, 6, 7, // back
    8, 9, 10, 8, 10, 11, // top
    12, 13, 14, 12, 14, 15, // bottom
    16, 17, 18, 16, 18, 19, // right
    20, 21, 22, 20, 22, 23, // left
};



const string vertexSrc = R"(#version 300 es

    in vec3 aPosCoord;
    in vec3 aNormalCoord;
    in vec2 aTexCoord;

    uniform mat4 projnMatrix;
    uniform mat4 viewMatrix;
    uniform mat4 modelMatrix;
    uniform mat4 normalMatrix;

    out vec3 Normal;
    out vec2 TexCoord;
                                  
    void main(){
        gl_Position = projnMatrix * viewMatrix * modelMatrix * vec4(aPosCoord, 1.0);                 
        Normal =  aNormalCoord;   
        TexCoord = aTexCoord;               
    }
)";
    
    
    
    
const string fragmentSrc = R"(#version 300 es
    precision mediump float;

    in vec3 Normal;
    in vec2 TexCoord;
                              
    out vec4 fragColour;

    uniform sampler2D sampler1;
                              
    void main(){
        fragColour = texture(sampler1, TexCoord); 
        //fragColour = vec4(1.0, 0.0, 0.0, 1.0);        
    } 
)";



string loadShader(string path){
    string shader = "";  
    stringstream ss;
    exception e;
    
    fstream f(path, ios::in);
    if(!f.is_open())
        throw  e;
    
    while( !f.eof() ){
         getline(f, shader);
         ss << shader << endl;
    }
    f.close();
        
    return ss.str();
}



string error(int code, int l){
    stringstream ss;
    string errMsg = "";
    stringstream line;
    line<<l;
    
    if(code == 1280){
        ss << code;
        errMsg = "Error at line " + line.str() + "\nGL_INVALID_ENUM(" + ss.str() + "):  An unacceptable value is specified for an enumerated argument.          \n\n";
    }
    else if(code == 1281){
        ss << code;
        errMsg = "Error at line " + line.str() +  "\nGL_INVALID_VALUE(" + ss.str() + "):  A numeric argument is out of range.          \n\n";
    }
    else if(code == 1282){
        ss << code;
        errMsg = "Error at line " + line.str() +   "\nGL_INVALID_OPERATION(" + ss.str() + "):  The specified operation is not allowed in the current state.         \n\n";
    }
    else if(code == 1283){
        ss << code;
        errMsg = "Error at line " + line.str() +   "\nGL_INVALID_FRAMEBUFFER_OPERATION(" + ss.str() + "): The framebuffer object is not complete.            \n\n";
    }
    else if(code == 1284){
        ss << code;
        errMsg = "Error at line " + line.str() +   "GL_OUT_OF_MEMORY(" + ss.str() + "):  There is not enough memory left to execute the command.         \n\n";
    }
    else if(code == 1285){
        ss << code;
        errMsg = "Error at line " + line.str() +   "\nGL_STACK_UNDERFLOW(" + ss.str() + "): An attempt has been made to perform an operation that would cause an internal stack to underflow.         \n\n";
    }
    else if(code == 1286){
        ss << code;
        errMsg = "Error at line " + line.str() +   "\nGL_STACK_OVERFLOW(" + ss.str() + "): An attempt has been made to perform an operation that would cause an internal stack to overflow.           \n\n";
    }
    else{
        ss << 0;
        return "Error at line " + line.str() +   "\nGL_NO_ERROR(" + ss.str() + "): No error has been recorded.          \n\n";
    }
    
    return errMsg;          
}