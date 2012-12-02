#include "shader.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <GL/glew.h>
#include <boost/foreach.hpp>
#include <stdexcept>
using namespace std;

namespace { // anonymous

char* textFileRead(const char *fn) {
    FILE *fp;
    char *content = NULL;

    int count = 0;

    assert (fn != NULL);
    fp = fopen(fn, "rt");

    if (fp == NULL)
        throw runtime_error(string("file '")+fn+"' not found!");

    fseek(fp, 0, SEEK_END);
    count = ftell(fp);
    rewind(fp);

    if (count > 0) {
        content = (char *) malloc(sizeof(char) * (count + 1));
        count = fread(content, sizeof(char), count, fp);
        content[count] = '\0';
    }
    fclose(fp);

    return content;
}

void printShaderInfoLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char *) malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj) {
    int infologLength = 0;
    int charsWritten = 0;
    char *infoLog;

    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

    if (infologLength > 0) {
        infoLog = (char *) malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
        printf("%s\n", infoLog);
        free(infoLog);
    }
}

}

const ShaderId ShaderManager::defaultShader = "";

ShaderManager::ShaderManager()
: curProg (defaultShader) {
    programs[defaultShader] = 0;
}


void ShaderManager::loadShader(const ShaderId& id, const string& vertfile, const string& fragfile, const list<string>& defines) {
    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

    char* vs = textFileRead(vertfile.c_str());
    char* fs = textFileRead(fragfile.c_str());

    string define_code;
    BOOST_FOREACH(const string& define, defines) {
        define_code += "#define " + define + "\n";
    }

    const char* sources[2] = {define_code.c_str(), vs};
    glShaderSource(v, 2, sources, NULL);
    sources[1] = fs;
    glShaderSource(f, 2, sources, NULL);

    free(vs);
    free(fs);

    glCompileShader(v);
    glCompileShader(f);

    printShaderInfoLog(v);
    printShaderInfoLog(f);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);

    glLinkProgram(p);
    printProgramInfoLog(p);

    programs[id] = p;
}

void ShaderManager::useShader(const ShaderId& shader) {
    assert(programs.count(shader) > 0);
    glUseProgram(programs[shader]);
    curProg = shader;
}

void ShaderManager::setParam(const std::string& param, int value) {
    int loc = glGetUniformLocation(programs[curProg], param.c_str());
    if (loc == -1)
        throw runtime_error("param '" + param + "' not found in shader");
    glUniform1i(loc, value);
}
