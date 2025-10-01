#include <shader.h>
#include <Debug.h>
#include <filesystem>
#include <glm/gtc/type_ptr.hpp>

static std::string getFileName(const char* filepath) {
	std::string path = std::string(filepath);
	size_t pos = path.find_last_of("/\\");
	return (pos != std::string::npos)
		? path.substr(pos + 1)
		: path;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	constexpr size_t infoLogSize = 512;
	const std::string pathPrefix = "res\\shaders\\";

	std::string fullVertexPath = pathPrefix + std::string(vertexPath);
	std::string fullFragmentPath = pathPrefix + std::string(fragmentPath);

	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	std::string vShaderFileName = getFileName(fullVertexPath.c_str());
	std::string fShaderFileName = getFileName(fullFragmentPath.c_str());

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vShaderFile.open(fullVertexPath.c_str());
		fShaderFile.open(fullFragmentPath.c_str());
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		debugLog("Failure: %s\n", e.what());
		debugError("Failed to read shader file\n");
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[infoLogSize];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, infoLogSize, NULL, infoLog);
		debugError("Vertex shader compilation for %s failed\n%s\n", vShaderFileName.c_str(), infoLog);
	} else {
		debugLog("Vertex shader compiled for %s\n", vShaderFileName.c_str());
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, infoLogSize, NULL, infoLog);
		debugError("Fragment shader compilation for %s failed\n%s\n", fShaderFileName.c_str(), infoLog);
	} else {
		debugLog("Fragment shader compiled for %s\n", fShaderFileName.c_str());
	}

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, infoLogSize, NULL, infoLog);
		debugError("Shader program linking failed\n%s\n", infoLog);
	} else {
		debugLog("Shader program linked\n");
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(ID);
}

int Shader::tryGetLoc(const std::string& name) const {
	int loc = glGetUniformLocation(ID, name.c_str());
	if (loc == -1) debugError("Failed to get uniform location for \"%s\"\n", name.c_str());
	return loc;
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(tryGetLoc(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(tryGetLoc(name), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(tryGetLoc(name), value);
}

void Shader::setVec2(const std::string& name, glm::vec2 value) const {
	glUniform2f(tryGetLoc(name), value.x, value.y);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const {
	glUniform3f(tryGetLoc(name), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value) const {
	glUniform4f(tryGetLoc(name), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const {
	glUniformMatrix4fv(tryGetLoc(name), 1, GL_FALSE, glm::value_ptr(value));
}
