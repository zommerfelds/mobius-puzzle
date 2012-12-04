// Name: Christian Zommerfelds
// Student Number: 20493973
// User-id: cgzommer

#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <string>
#include <list>
#include <map>

typedef std::string ShaderId;

class ShaderManager {
public:
    ShaderManager();

    void loadShader(const ShaderId& id, const std::string& vertfile, const std::string& fragfile,
                    const std::list<std::string>& defines = std::list<std::string>());

    void useShader(const ShaderId& shader);
    void setParam(const ShaderId& param, int value);

    static const ShaderId defaultShader;

private:
    std::map<ShaderId, size_t> programs;
    ShaderId curProg;
};

#endif
