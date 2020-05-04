/*
 * Transforms.c
 *
 *  Created on: 17 feb. 2020
 *      Author: Cursos
 */
#include "Transforms.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct strNode {
	struct strNode* below;
	Mat4* data;
};

typedef struct strNode* Node;

Node top = NULL;

void emptyStack() {
	Node current = top;
	while(current != NULL) {
		Node tmp = current ->below;
		free(current->data);
		free(current);
		current = tmp;
	}
	top = NULL;
}

void loadIdentity(Mat4* csMatrix) {
	mIdentity(csMatrix);
	emptyStack();
}

void pushMatrix(Mat4* csMatrix) {
	Node node = (Node) malloc(sizeof(struct strNode));
	Mat4* mat = (Mat4*) malloc(sizeof(Mat4));
	int i;
	for(i = 0; i < 16; i ++) mat->values[i] = csMatrix->values[i];
	node->data  = mat;
	node->below = top;
	top = node;
}

void popMatrix(Mat4* csMatrix) {
	if(top == NULL) return;
	int i;
	for(i = 0; i < 16; i ++) csMatrix->values[i] = top->data->values[i];
	Node toPop = top;
	top = top->below;
	free(toPop->data);
	free(toPop);
}

void translate(Mat4* csMatrix, float tx, float ty, float tz) {
	Mat4 trMatrix;
	mIdentity(&trMatrix);
	trMatrix.at[0][3] = tx;
	trMatrix.at[1][3] = ty;
	trMatrix.at[2][3] = tz;
	mMult(csMatrix, trMatrix);
//	mPrint(*csMatrix);
}

void rotateX(Mat4* m, float degrees) {
	Mat4 rotMatrix;
    mIdentity(&rotMatrix);
    float radians = degrees * M_PI / 180;
    rotMatrix.at[1][1] = cos(radians);
    rotMatrix.at[1][2] = -sin(radians);
    rotMatrix.at[2][1] = sin(radians);
    rotMatrix.at[2][2] = cos(radians);

    mMult(m, rotMatrix);
}

void rotateY(Mat4* csMatrix, float degrees) {
	Mat4 rotMatrix;
	mIdentity(&rotMatrix);
	float radians = degrees * M_PI / 180;
	rotMatrix.at[2][2] =  cos(radians);
	rotMatrix.at[2][0] = -sin(radians);
	rotMatrix.at[0][2] =  sin(radians);
	rotMatrix.at[0][0] =  cos(radians);
	mMult(csMatrix, rotMatrix);
}

void rotateZ(Mat4* csMatrix, float degrees) {
	Mat4 rotMatrix;
	mIdentity(&rotMatrix);
	float radians = degrees * M_PI / 180;
	rotMatrix.at[0][0] =  cos(radians);
	rotMatrix.at[0][1] = -sin(radians);
	rotMatrix.at[1][0] =  sin(radians);
	rotMatrix.at[1][1] =  cos(radians);
	mMult(csMatrix, rotMatrix);
//	mPrint(*csMatrix);
}

void scale (Mat4* csMatrix, float sx, float sy, float sz) {
	Mat4 scMatrix;
	mIdentity(&scMatrix);
	scMatrix.at[0][0] = sx;
	scMatrix.at[1][1] = sy;
	scMatrix.at[2][2] = sz;
	mMult(csMatrix, scMatrix);
//	mPrint(*csMatrix);
}

void setOrtho(Mat4* m, float L, float R, float B, float T, float F, float N) {
	mIdentity(m);
	m->at[0][0] = 2 / (R - L);
	m->at[1][1] = 2 / (T - B);
	m->at[2][2] = 2 / (N - F);
	m->at[0][3] = -(R + L) / (R - L);
	m->at[1][3] = -(T + B) / (T - B);
	m->at[2][3] = -(N + F) / (N - F);
	mPrint(*m);
}

void setPerspective(Mat4* m, float fovy, float aspect, float nearZ, float farZ) {
	mIdentity(m);
	float radians = fovy * M_PI / 180;
	float tanF    = tan(radians / 2);
	m->at[0][0] = 1.0 / (aspect * tanF);
	m->at[1][1] = 1.0 / tanF;
	m->at[2][2] = (farZ + nearZ) / (nearZ - farZ);
	m->at[2][3] = (-2 * nearZ * farZ) / (nearZ - farZ);
	m->at[3][2] = -1;
	m->at[3][3] = 0;
}
