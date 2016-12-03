#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"


class ofApp : public ofBaseApp {
public:
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void atualizaContraste(ofxCvGrayscaleImage &imgGray, int contrasteDistancia);
	void atualizaFantasmaDepth(ofxCvGrayscaleImage imgAtual, float iRastro);
	void atualizaBlurFantasmaDepth(ofxCvGrayscaleImage imgAtual, float iRastro);
	void atualizaSombra(ofxCvGrayscaleImage imgAtual, float iRastro);
	void atualizaSombraMirror(ofxCvGrayscaleImage imgAtual, float iRastro);

	void drawPointCloud();
	void desenhaCameras(bool desenhaInstrucoes);
	
	void trocaBrisa();
	void desenhaFade();
	void autoChange();
	void desenhaSombra();
	void desenhaSombraMirror();
	void desenhaBGcolorido();
	void desenhaBGSol();
	void desenhaGirassois(int qtdColW, int qtdColH);
	void desenhaGirassol(int x, int y, float bright) ;
	void desenhaRaiosois(int qtdColW, int qtdColH);
	void desenhaRaiosol(int x, int y, float bright) ;

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	
	ofxKinect kinect;
	
	ofImage raiosol, girassol;
	
	ofxCvColorImage colorImg;

	ofxCvGrayscaleImage fantasmaDepth;
	ofxCvGrayscaleImage blurDepth;
	ofxCvGrayscaleImage blurFantasmaDepth;
	ofxCvGrayscaleImage sombra;
	ofxCvGrayscaleImage sombraMirror;
	ofxCvGrayscaleImage contrasteDepth; // grayscale depth exagerado
	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image
	
	ofxCvContourFinder contourFinder;
	

	float indiceContraste;

	bool bDrawPointCloud;
	bool bDesenhaCameras;
	
	int nearThreshold;
	int farThreshold;
	
	int angle;
	
	// used for viewing the point cloud
	ofEasyCam easyCam;
};
