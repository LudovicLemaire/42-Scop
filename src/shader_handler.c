#include <scop.h>

char *shaderParser(char *path) {
    if (access(path, F_OK ) != 0) {
        printf("\x1b[91mShader Path\x1b[0m: %s\n", path);
        exit(-1);
    } else {
        printf("\x1b[92mShader Path\x1b[0m: %s\n", path);
    }

    FILE *inputfile = fopen(path, "r");
    size_t linesize = 0;
    char* linebuf = 0;
    ssize_t linelen = 0;
    struct stat file_info;
	stat(path, &file_info);
    int mallocText = file_info.st_size;
    char *completeText = calloc(mallocText + 1, sizeof(char));

    while ((linelen = getline(&linebuf, &linesize, inputfile)) > 0) {
        strcat(completeText, linebuf);
        free(linebuf);
        linebuf = NULL;
    }
    free(linebuf);
    linebuf = NULL;
    fclose(inputfile);
	return completeText;
}

int shaderCreateProgram() {
    int     program = 0;
    int     vertex;
    int     fragment;
    char    log[512];
    int     success;

    char *vertexShaderSource = shaderParser("shaders/vertexShader.vert");
    char *fragmentShaderSource = shaderParser("shaders/fragmentShader.frag");

    vertex      = glCreateShader(GL_VERTEX_SHADER);
    fragment    = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex, 1, (const char * const *)&vertexShaderSource, NULL);
    free(vertexShaderSource);
    vertexShaderSource = NULL;
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, log);
		printf("\x1b[91m[Vertex shader] Linking failed\x1b[0m: %s\n", log);
        exit(0);
    }
    glShaderSource(fragment, 1, (const char * const *)&fragmentShaderSource, NULL);
    free(fragmentShaderSource);
    fragmentShaderSource = NULL;
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, log);
		printf("\x1b[91m[Fragment shader] Linking failed\x1b[0m: %s\n", log);
        exit(0);
    }

    program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, log);
		printf("\x1b[91m[Program] Linking failed\x1b[0m: %s\n", log);
        exit(0);
    }
    return (program);

}