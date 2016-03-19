#include "shader.h"
#include "memory.h"
#include "log.h"

#include <stdlib.h>
#include <stdio.h>

#include <GLXW/glxw.h>

static unsigned int _compile_shader(const char* filename, GLenum type);

struct tds_shader* tds_shader_create(const char* vs, const char* gs, const char* fs) {
	int status = 0;
	struct tds_shader* output = tds_malloc(sizeof *output);

	output->prg = glCreateProgram();

	if (vs) {
		output->vs = _compile_shader(vs, GL_VERTEX_SHADER);
		glAttachShader(output->prg, output->vs);
	}

	if (gs) {
		output->gs = _compile_shader(gs, GL_GEOMETRY_SHADER);
		glAttachShader(output->prg, output->gs);
	}

	if (fs) {
		output->fs = _compile_shader(fs, GL_FRAGMENT_SHADER);
		glAttachShader(output->prg, output->fs);
	}

	glLinkProgram(output->prg);
	glGetProgramiv(output->prg, GL_LINK_STATUS, &status);

	if (!status) {
		char buf[1024] = {0};
		glGetProgramInfoLog(output->prg, sizeof buf / sizeof *buf , NULL, buf);
		tds_logf(TDS_LOG_CRITICAL, "Failed to link shader program with error: %s\n", buf);
		return NULL;
	}

	output->f_vs = vs;
	output->f_gs = gs;
	output->f_fs = fs;

	tds_shader_bind(output);
	output->u_transform = glGetUniformLocation(output->prg, "tds_transform");
	output->u_color = glGetUniformLocation(output->prg, "tds_color");

	unsigned int u_texture = glGetUniformLocation(output->prg, "tds_texture");
	glUniform1i(u_texture, 0);

	return output;
}

void tds_shader_free(struct tds_shader* ptr) {
	glUseProgram(0);

	if (ptr->f_vs) {
		glDetachShader(ptr->prg, ptr->vs);
		glDeleteShader(ptr->vs);
	}

	if (ptr->f_gs) {
		glDetachShader(ptr->prg, ptr->gs);
		glDeleteShader(ptr->gs);
	}

	if (ptr->f_fs) {
		glDetachShader(ptr->prg, ptr->fs);
		glDeleteShader(ptr->fs);
	}

	glDeleteProgram(ptr->prg);
	tds_free(ptr);
}

unsigned int _compile_shader(const char* filename, GLenum type) {
	FILE* f = fopen(filename, "r");

	if (!f) {
		tds_logf(TDS_LOG_CRITICAL, "Failed to open shader file [%s] for reading.\n", filename);
		return 0;
	}

	fseek(f, 0, SEEK_END);
	int filesize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char* buf = tds_malloc(filesize);
	fread(buf, 1, filesize, f);

	unsigned int output = glCreateShader(type);
	glShaderSource(output, 1, (const char**) &buf, &filesize);

	tds_free(buf);
	glCompileShader(output);

	int status = 0;
	glGetShaderiv(output, GL_COMPILE_STATUS, &status);

	if (!status) {
		char buf[1024] = {0};
		glGetShaderInfoLog(output, sizeof buf / sizeof *buf , NULL, buf);
		tds_logf(TDS_LOG_CRITICAL, "%s failed to compile with error: %s\n", filename, buf);
		return 0;
	}

	fclose(f);
	return output;
}

void tds_shader_bind(struct tds_shader* ptr) {
	glUseProgram(ptr->prg);
}

void tds_shader_set_transform(struct tds_shader* ptr, float* transform) {
	glUniformMatrix4fv(ptr->u_transform, 1, GL_FALSE, transform);
}

void tds_shader_set_color(struct tds_shader* ptr, float r, float g, float b, float a) {
	glUniform4f(ptr->u_color, r, g, b, a);
}
