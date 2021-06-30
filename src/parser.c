#include <stdio.h>
#include <GL/gl3w.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <scop.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <unistd.h>

void setFace(GLfloat **buffer, int *j, int *face_inc, GLfloat x, GLfloat y, GLfloat z, t_rgb rgbRand, t_rgb rgbMtl, t_vec3 normal, GLfloat xTexture, GLfloat yTexture, GLfloat xTextureMtl, GLfloat yTextureMtl) {
    (*buffer)[(*j)++] = x;
    (*buffer)[(*j)++] = y;
    (*buffer)[(*j)++] = z;
    (*buffer)[(*j)++] = rgbRand.r;
    (*buffer)[(*j)++] = rgbRand.g;
    (*buffer)[(*j)++] = rgbRand.b;
	(*buffer)[(*j)++] = rgbMtl.r;
	(*buffer)[(*j)++] = rgbMtl.g;
	(*buffer)[(*j)++] = rgbMtl.b;
    (*buffer)[(*j)++] = normal.x;
    (*buffer)[(*j)++] = normal.y;
    (*buffer)[(*j)++] = normal.z;
	(*buffer)[(*j)++] = xTexture;
	(*buffer)[(*j)++] = yTexture;
	(*buffer)[(*j)++] = xTextureMtl;
	(*buffer)[(*j)++] = yTextureMtl;
    ++(*face_inc);
}

void checkObjValues(int *arr, int size, int vMax) {
	while(--size) {
		if (arr[size] > vMax || arr[size] < 1) {
			printf("\x1b[91mError obj file\x1b[0m\n");
			exit(0);
		}
	}
}

void parser(GLfloat **buffer, int *sizeMallocFaces, char *filename, t_obj_spec *mm, int *hasVT) {
    srand(time(NULL));
    if (access(filename, F_OK ) != 0 || !EndsWith(filename, ".obj")) {
        printf("\x1b[91mObj Path\x1b[0m: %s\n", filename);
        exit(-1);
    } else {
        printf("\x1b[92mObj Path\x1b[0m: %s\n", filename);
    }

    FILE *inputfile = fopen(filename, "r");

    struct stat file_info;
    stat(filename, &file_info);
	// Initialize MallocSize value from file size so it's neither too big, nor will do too much realloc
    int sizeMallocVertex = file_info.st_size / 50;
	int sizeMallocVN = file_info.st_size / 50;
	int sizeMallocVT = file_info.st_size / 50;
	int sizeMallocMaterials = 10;
    *sizeMallocFaces = sizeMallocVertex * BUFFER_LENGTH;

	// Calloc buffers
    t_vec3 *vertex = calloc(sizeMallocVertex, sizeof(t_vec3));
	if (!vertex)
		mallocFailed("Vertex calloc");
    *buffer = calloc(*sizeMallocFaces, sizeof(GLfloat));
	if (!*buffer)
		mallocFailed("Complete buffer calloc");
	t_vec3 *normalBuff = calloc(sizeMallocVertex, sizeof(t_vec3));
	if (!normalBuff)
		mallocFailed("Normal calloc");
	t_vec2 *vertexTextureBuff = calloc(sizeMallocVertex, sizeof(t_vec2));
	if (!vertexTextureBuff)
		mallocFailed("Texture calloc");
	t_materials *materials = calloc(sizeMallocMaterials, sizeof(t_materials));
	if (!materials)
		mallocFailed("Material calloc");
	
    size_t linesize = 0;
    char* linebuf = 0;
    ssize_t linelen = 0;
    float x, y, z;
    int a_point, a_texture, a_normal, b_point, b_texture, b_normal, c_point, c_texture, c_normal, d_point, d_normal, d_texture, e_point;
    int vertex_inc = 0;
	int vn_inc = 0;
	int vt_inc = 0;
    int face_inc = 0;
	char mtl_reduced_path[256];
	char *mtl_path;
	float r, g, b;
	char materialName[256];
	int totalMaterials = 0;
	t_rgb rgbMtl = fill_rgb(0.0, 1.0, 0.0);
	int haveMtl = 0;

    while ((linelen = getline(&linebuf, &linesize, inputfile)) > 0) {
        // Realloc buffer Vertex if necessary
		if (vertex_inc + 1 >= sizeMallocVertex) {
            sizeMallocVertex *= 1.5;
            printf("sizeMallocVertex: %d\n", sizeMallocVertex);
            vertex = realloc(vertex, (sizeMallocVertex) * sizeof(t_vec3));
			if (!vertex)
				mallocFailed("Vertex realloc");
        }
		// Realloc buffer Normal if necessary
        if (vn_inc + 1 >= sizeMallocVN) {
            sizeMallocVN *= 1.5;
            printf("sizeMallocVN: %d\n", sizeMallocVN);
			normalBuff = realloc(normalBuff, (sizeMallocVN) * sizeof(t_vec3));
			if (!normalBuff)
				mallocFailed("Normal realloc");
        }
		// Realloc buffer Texture if necessary
		if (vt_inc + 1 >= sizeMallocVT) {
            sizeMallocVT *= 1.5;
            printf("sizeMallocVT: %d\n", sizeMallocVT);
			vertexTextureBuff = realloc(vertexTextureBuff, (sizeMallocVT) * sizeof(t_vec2));
			if (!vertexTextureBuff)
				mallocFailed("Texture realloc");
        }
		// Realloc buffer Face if necessary
        if ((face_inc + 1) * 18 >= *sizeMallocFaces) {
            *sizeMallocFaces *= 1.5;
            printf("\x1b[93mRealloc bufferFace:\x1b[0m %d\n", *sizeMallocFaces);
            *buffer = realloc(*buffer, *sizeMallocFaces * sizeof(GLfloat));
			if (!*buffer)
				mallocFailed("Face realloc");
        }
		// Get RGB associated to the Material and store it to add them to buffer (which have this Material)
		if (sscanf(linebuf, "usemtl %s", materialName) == 1) {
			if (haveMtl) {
				int lenMaterials = totalMaterials;
				while (lenMaterials--) {
					if (strcmp(materialName, materials[lenMaterials].name) == 0) { 
						rgbMtl = fill_rgb(materials[lenMaterials].rgb.r, materials[lenMaterials].rgb.g, materials[lenMaterials].rgb.b);
					}
				}
			}
		}

		// Parse MTL file to get colors (texture aren't handled)
        else if (sscanf(linebuf, "mtllib %s", mtl_reduced_path) == 1) {
			// Find path for .mtl and check if the file exist
			char prePath[256];
			getPrePath(filename, prePath);
			mtl_path = concat(prePath, mtl_reduced_path);
			if (access(mtl_path, F_OK ) != 0 || !EndsWith(mtl_path, ".mtl")) {
				printf("\x1b[91mMtl Path\x1b[0m: %s\n", mtl_path);
				exit(-1);
			} else {
				printf("\x1b[92mMtl Path\x1b[0m: %s\n", mtl_path);
    		}

			FILE *mtlFile = fopen(mtl_path, "r");
			size_t linesizeMtl = 0;
    		char* linebufMtl = 0;
			ssize_t linelenMtl = 0;
			char nameMaterial[256];
			haveMtl = 1;

			while ((linelenMtl = getline(&linebufMtl, &linesizeMtl, mtlFile)) > 0) {
				// Store every material names
				if (sscanf(linebufMtl, "newmtl %s", nameMaterial) == 1) {
					if (totalMaterials+1 > sizeMallocMaterials) {
						sizeMallocMaterials += 10;
						materials = realloc(materials, (sizeMallocMaterials) * sizeof(t_materials));
						if (!materials) {
							mallocFailed("Material realloc");
						}
					}
					materials[totalMaterials].name = strdup(nameMaterial);
					++totalMaterials;
				}
				// Store Kd (colors) associated for each materials
				if (sscanf(linebufMtl, "Kd %f %f %f", &r, &g, &b) == 3) {
					t_rgb rgb = fill_rgb(r, g, b);
					materials[totalMaterials-1].rgb = rgb;
				}
				free(linebufMtl);
        		linebufMtl = NULL;
			}
			free(linebufMtl);
        	linebufMtl = NULL;
			fclose(mtlFile);
        }

        // Handle and store vertex
        else if (sscanf(linebuf, "v %f %f %f", &x, &y, &z) == 3) {
            fill_vertex3d(&vertex[vertex_inc], x, y, z);
            ++vertex_inc;
        }
		// Handle and store normal vn
        else if (sscanf(linebuf, "vn %f %f %f", &x, &y, &z) == 3) {
            fill_vertex3d(&normalBuff[vn_inc], x, y, z);
            ++vn_inc;
        }
		// Handle and store texture vt
        else if (sscanf(linebuf, "vt %f %f", &x, &y) == 2) {
            fill_vertex2d(&vertexTextureBuff[vt_inc], x, y);
            ++vt_inc;
			*hasVT = 1;
        }

        // Generate random color for a face
        t_rgb rgbRand = fill_rgb(getRC(), getRC(), getRC());
        int j = face_inc * BUFFER_LENGTH;
		// Contain 5 points
		if (sscanf(linebuf, "f %d %d %d %d %d", &a_point, &b_point, &c_point, &d_point, &e_point) == 5) {
			checkObjValues((int[5]){a_point, b_point, c_point, d_point, e_point}, 5, vertex_inc);
            t_vec3 normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);

            normal = getNormal(vertex[a_point-1], vertex[c_point-1], vertex[d_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);

            normal = getNormal(vertex[a_point-1], vertex[d_point-1], vertex[e_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[e_point-1].x, vertex[e_point-1].y, vertex[e_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
		// Contain 4 points/textures/normals
        else if (sscanf(linebuf, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &a_point, &a_texture, &a_normal, &b_point, &b_texture, &b_normal, &c_point, &c_texture, &c_normal, &d_point, &d_texture, &d_normal) == 12) {
			checkObjValues((int[4]){a_point, b_point, c_point, d_point}, 4, vertex_inc);
			checkObjValues((int[4]){a_texture, b_texture, c_texture, d_texture}, 4, vt_inc);
			checkObjValues((int[4]){a_normal, b_normal, c_normal, d_normal}, 4, vn_inc);
			t_vec3 normal = fill_vec(-normalBuff[a_normal-1].x, -normalBuff[a_normal-1].y, -normalBuff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, vertexTextureBuff[a_texture-1].x, vertexTextureBuff[a_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, vertexTextureBuff[b_texture-1].x, vertexTextureBuff[b_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, vertexTextureBuff[c_texture-1].x, vertexTextureBuff[c_texture-1].y);

			normal = fill_vec(-normalBuff[a_normal-1].x, -normalBuff[a_normal-1].y, -normalBuff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, vertexTextureBuff[a_texture-1].x, vertexTextureBuff[a_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, vertexTextureBuff[c_texture-1].x, vertexTextureBuff[c_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, vertexTextureBuff[d_texture-1].x, vertexTextureBuff[d_texture-1].y);
        }
		// Contain 4 points
		else if (sscanf(linebuf, "f %d %d %d %d", &a_point, &b_point, &c_point, &d_point) == 4) {
			checkObjValues((int[4]){a_point, b_point, c_point, d_point}, 4, vertex_inc);
            t_vec3 normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);

            normal = getNormal(vertex[a_point-1], vertex[c_point-1], vertex[d_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
		// Contain 4 points/textures
		else if (sscanf(linebuf, "f %d/%d %d/%d %d/%d %d/%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal, &d_point, &d_normal) == 8) {
			checkObjValues((int[4]){a_point, b_point, c_point, d_point}, 4, vertex_inc);
			checkObjValues((int[4]){a_normal, b_normal, c_normal, d_normal}, 4, vn_inc);
			t_vec3 normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, vertexTextureBuff[a_texture-1].x, vertexTextureBuff[a_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, vertexTextureBuff[b_texture-1].x, vertexTextureBuff[b_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, vertexTextureBuff[c_texture-1].x, vertexTextureBuff[c_texture-1].y);

            normal = getNormal(vertex[a_point-1], vertex[c_point-1], vertex[d_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, vertexTextureBuff[a_texture-1].x, vertexTextureBuff[a_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, vertexTextureBuff[c_texture-1].x, vertexTextureBuff[c_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, vertexTextureBuff[d_texture-1].x, vertexTextureBuff[d_texture-1].y);
        }
		// Contain 4 points/normals
		else if (sscanf(linebuf, "f %d//%d %d//%d %d//%d %d//%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal, &d_point, &d_normal) == 8) {
			checkObjValues((int[4]){a_point, b_point, c_point, d_point}, 4, vertex_inc);
			checkObjValues((int[4]){a_normal, b_normal, c_normal, d_normal}, 4, vn_inc);
			t_vec3 normal = fill_vec(-normalBuff[b_normal-1].x, -normalBuff[b_normal-1].y, -normalBuff[b_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);

            normal = fill_vec(-normalBuff[c_normal-1].x, -normalBuff[c_normal-1].y, -normalBuff[c_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[d_point-1].x, vertex[d_point-1].y, vertex[d_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
		// Contain 3 points/textures/normals
		else if (sscanf(linebuf, "f %d/%d/%d %d/%d/%d %d/%d/%d", &a_point, &a_texture, &a_normal, &b_point, &b_texture, &b_normal, &c_point, &c_texture, &c_normal) == 9) {
			checkObjValues((int[3]){a_point, b_point, c_point}, 3, vertex_inc);
			checkObjValues((int[3]){a_texture, b_texture, c_texture}, 3, vt_inc);
            t_vec3 normal = fill_vec(-normalBuff[a_normal-1].x, -normalBuff[a_normal-1].y, -normalBuff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, vertexTextureBuff[a_texture-1].x, vertexTextureBuff[a_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, vertexTextureBuff[b_texture-1].x, vertexTextureBuff[b_texture-1].y);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, vertexTextureBuff[c_texture-1].x, vertexTextureBuff[c_texture-1].y);
        }
		// Contain 3 points
        else if (sscanf(linebuf, "f %d %d %d", &a_point, &b_point, &c_point) == 3) {
			checkObjValues((int[3]){a_point, b_point, c_point}, 3, vertex_inc);
            t_vec3 normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
		// Contain 3 points/textures
        else if (sscanf(linebuf, "f %d/%d %d/%d %d/%d", &a_point, &a_texture, &b_point, &b_texture, &c_point, &c_texture) == 6) {
			checkObjValues((int[3]){a_point, b_point, c_point}, 3, vertex_inc);
			checkObjValues((int[3]){a_texture, b_texture, c_texture}, 3, vt_inc);
            t_vec3 normal = getNormal(vertex[a_point-1], vertex[b_point-1], vertex[c_point-1]);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
		// Contain 3 points/normals
        else if (sscanf(linebuf, "f %d//%d %d//%d %d//%d", &a_point, &a_normal, &b_point, &b_normal, &c_point, &c_normal) == 6) {
			checkObjValues((int[3]){a_point, b_point, c_point}, 3, vertex_inc);
			checkObjValues((int[3]){a_normal, b_normal, c_normal}, 3, vn_inc);
			t_vec3 normal = fill_vec(-normalBuff[a_normal-1].x, -normalBuff[a_normal-1].y, -normalBuff[a_normal-1].z);
            setFace(buffer, &j, &face_inc, vertex[a_point-1].x, vertex[a_point-1].y, vertex[a_point-1].z, rgbRand, rgbMtl, normal, 0.0, 0.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[b_point-1].x, vertex[b_point-1].y, vertex[b_point-1].z, rgbRand, rgbMtl, normal, 0.0, 1.0, 0.0, 0.0);
            setFace(buffer, &j, &face_inc, vertex[c_point-1].x, vertex[c_point-1].y, vertex[c_point-1].z, rgbRand, rgbMtl, normal, 1.0, 1.0, 0.0, 0.0);
        }
        free(linebuf);
        linebuf = NULL;
    }
	// Free all buffers
    free(linebuf);
    linebuf = NULL;
    free(vertex);
    vertex = NULL;
	free(normalBuff);
    normalBuff = NULL;
	free(vertexTextureBuff);
    vertexTextureBuff = NULL;
	// Free mtl path if it exists
	if (haveMtl) {
		free(mtl_path);
		mtl_path = NULL;
	}
	// Free every materials
	int lenMaterials = totalMaterials;
	while (lenMaterials--) {
		free(materials[lenMaterials].name);
		materials[lenMaterials].name = NULL;
	}
	free(materials);
	materials = NULL;
	fclose(inputfile);

	// Realloc buffer to correct size
    *buffer = realloc(*buffer, ((face_inc * 3 * BUFFER_LENGTH) + 1) * sizeof(GLfloat));
	if (!*buffer)
		mallocFailed("Complete buffer realloc");

    *sizeMallocFaces = (face_inc * BUFFER_LENGTH);

    printf("\x1b[94mNombre de triangles:\x1b[0m %d\n", face_inc/3);

	// initialize min/max
    mm->x_min = FLT_MAX;
    mm->y_min = FLT_MAX;
    mm->z_min = FLT_MAX;
    mm->x_max = -FLT_MAX;
    mm->y_max = -FLT_MAX;
    mm->z_max = -FLT_MAX;

    getMinMax(*buffer, face_inc, mm);
    getLengthMiddle(mm);
    //print_vertex(vertex, vertex_inc);
    // print_buffer(*buffer, face_inc);
}
