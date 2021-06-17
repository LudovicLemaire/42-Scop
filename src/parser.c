// gcc -I./include parser.c -o parser && ./parser
#include <stdio.h>
#include <GL/gl3w.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <wow.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void print_vertex(t_vec *vertex, int size) {
    int i = 0;
    while (i < size) {
        printf("vertex[%d]: %f %f %f\n", i, vertex[i].x, vertex[i].y, vertex[i].z);
        ++i;
    }
}

void print_buffer(GLfloat *buffer, int size) {
    int i = 0;
    while (size--) {
        printf("%f, %f, %f, %f, %f, %f\n", buffer[i], buffer[i+1], buffer[i+2], buffer[i+3], buffer[i+4], buffer[i+5]);
        i += 6;
    }
}

void fill_vertex(t_vec *vertex, float x, float y, float z) {
    vertex->x = x;
    vertex->y = y;
    vertex->z = z;
}

float copyGetRC() {
    return 1.0;
    float color = (float)rand() / (float)RAND_MAX;
    return color;
}

char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

void setFace(GLfloat **buffer, int *j, int *face_inc, GLfloat x, GLfloat y, GLfloat z) {
    (*buffer)[(*j)++] = x;
    (*buffer)[(*j)++] = y;
    (*buffer)[(*j)++] = z;
    (*buffer)[(*j)++] = copyGetRC();
    (*buffer)[(*j)++] = copyGetRC();
    (*buffer)[(*j)++] = copyGetRC();
    ++(*face_inc);
}

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename) {
    srand(time(NULL));
    printf("path: %s\n", filename);
    FILE *inputfile = fopen(filename, "r");

    struct stat file_info;
    lstat(filename, &file_info);
    int sizeMallocVertex = file_info.st_size / 50;
    *sizeMallocFaces = sizeMallocVertex * 6;

    t_global_params *g_param;
    g_param = calloc(1, sizeof(t_global_params));
    g_param->vertex = calloc(sizeMallocVertex, sizeof(t_vec));
    *buffer = calloc(*sizeMallocFaces, sizeof(GLfloat));

    size_t linesize = 0;
    char* linebuf = 0;
    ssize_t linelen = 0;
    float x, y, z;
    int a_point, a_texture, a_normal, b_point, b_texture, b_normal, c_point, c_texture, c_normal;
    int vertex_inc = 0;
    int face_inc = 0;

    while ((linelen = getline(&linebuf, &linesize, inputfile)) > 0) {
        // realloc
        if (vertex_inc + 1 >= sizeMallocVertex) {
            sizeMallocVertex *= 1.5;
            printf("sizeMallocVertex: %d\n", sizeMallocVertex);
            g_param->vertex = realloc(g_param->vertex, (sizeMallocVertex) * sizeof(t_vec));
        }
        if ((face_inc + 1) * 18 >= *sizeMallocFaces) {
            *sizeMallocFaces *= 1.5;
            printf("\x1b[93msizeMallocFaces:\x1b[0m %d\n", *sizeMallocFaces);
            *buffer = realloc(*buffer, *sizeMallocFaces * sizeof(GLfloat));
        }
        // vertex
        if (sscanf(linebuf, "v %f %f %f", &x, &y, &z) == 3) {
            fill_vertex(&g_param->vertex[vertex_inc], x, y, z);
            ++vertex_inc;
        }

        // faces
        int j = face_inc * 6;
        if (sscanf(linebuf, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a_point, &a_texture, &a_normal, &b_point, &b_texture, &b_normal, &c_point, &c_texture, &c_normal) == 9) {
            setFace(&*buffer, &j, &face_inc, g_param->vertex[a_point-1].x, g_param->vertex[a_point-1].y, g_param->vertex[a_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[b_point-1].x, g_param->vertex[b_point-1].y, g_param->vertex[b_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[c_point-1].x, g_param->vertex[c_point-1].y, g_param->vertex[c_point-1].z);
        }
        if (sscanf(linebuf, "f %d %d %d", &a_point, &b_point, &c_point) == 3) {
            setFace(&*buffer, &j, &face_inc, g_param->vertex[a_point-1].x, g_param->vertex[a_point-1].y, g_param->vertex[a_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[b_point-1].x, g_param->vertex[b_point-1].y, g_param->vertex[b_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[c_point-1].x, g_param->vertex[c_point-1].y, g_param->vertex[c_point-1].z);
        }
        if (sscanf(linebuf, "f %d/%d %d/%d %d/%d", &a_point, &a_texture, &b_point, &b_texture, &c_point, &c_texture) == 6) {
            setFace(&*buffer, &j, &face_inc, g_param->vertex[a_point-1].x, g_param->vertex[a_point-1].y, g_param->vertex[a_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[b_point-1].x, g_param->vertex[b_point-1].y, g_param->vertex[b_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[c_point-1].x, g_param->vertex[c_point-1].y, g_param->vertex[c_point-1].z);
        }
        if (sscanf(linebuf, "f %d//%d %d//%d %d//%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal) == 6) {
            setFace(&*buffer, &j, &face_inc, g_param->vertex[a_point-1].x, g_param->vertex[a_point-1].y, g_param->vertex[a_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[b_point-1].x, g_param->vertex[b_point-1].y, g_param->vertex[b_point-1].z);
            setFace(&*buffer, &j, &face_inc, g_param->vertex[c_point-1].x, g_param->vertex[c_point-1].y, g_param->vertex[c_point-1].z);
        }

        //------------------------ strtok -> split ligne sur un char -------------------- 
        free(linebuf);
        linebuf = NULL;
    }
    g_param->vertex = realloc(g_param->vertex, (vertex_inc + 1) * sizeof(t_vec));
    //print_vertex(g_param->vertex, vertex_inc);

    printf("\x1b[92mNombre de triangles:\x1b[0m %d\n", face_inc/3);

    //print_buffer(*buffer, face_inc);
}
