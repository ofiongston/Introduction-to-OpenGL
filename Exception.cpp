#include <exception>
#include <string>

using namespace std;


#ifndef TEXTURE_EXCEPTION_CPP
#define TEXTURE_EXCEPTION_CPP

class Exception : public exception{
    
    private:
        const char* t = NULL;
        string t2;
        int txt;
        
        
    public:
        Exception(const char* txt): t(txt)
        {}
        
        Exception(string txt): t2(txt)
        {}
        
        Exception(int txt): txt(txt)
        {}
        
        
        const char* what() const noexcept{
            return t;
        }
          
};
#endif