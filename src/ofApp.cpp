#include "ofApp.h"

/*
    If you are struggling to get the device to connect ( especially Windows Users )
    please look at the ReadMe: in addons/ofxKinect/README.md
*/
int trocouBrisaFade = 0;
bool trocaBrisaControl = false;
int numControl = 0;
float inicioUltimaBrisa = 0;
int vw, vh;
float agora;
//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	// enable depth->video image calibration
	kinect.setRegistration(true);
    
	kinect.init();
	
	kinect.open();		// opens first available kinect
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}
	
	fantasmaDepth.allocate(kinect.width, kinect.height);
	blurDepth.allocate(kinect.width, kinect.height);
	blurFantasmaDepth.allocate(kinect.width, kinect.height);
	sombra.allocate(kinect.width, kinect.height);
	sombraMirror.allocate(kinect.width, kinect.height);
	contrasteDepth.allocate(kinect.width, kinect.height);

	colorImg.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	
	nearThreshold = 230;
	farThreshold = 70;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinect.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;

	indiceContraste =158;


	ofEnableBlendMode(OF_BLENDMODE_ALPHA);

	raiosol.load("../data/raiosol.png");
	girassol.load("../data/girassol.png");
	
	inicioUltimaBrisa = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofApp::update() {
	agora = ofGetElapsedTimef();
	ofBackground(100, 100, 100);
	
	kinect.update();
	
	// there is a new frame and we are connected
	if(kinect.isFrameNew()) {
		
		
		// Pega imagem de profundidade e aumenta o contraste das distancias
		contrasteDepth.setFromPixels(kinect.getDepthPixels());
		atualizaContraste(contrasteDepth, indiceContraste);

		atualizaFantasmaDepth(contrasteDepth, 0.19);
		atualizaBlurFantasmaDepth(contrasteDepth, 0.97);
		atualizaSombra(contrasteDepth, 0.99 );
		atualizaSombraMirror(contrasteDepth, 0.99 );
		blurDepth.setFromPixels(kinect.getDepthPixels());
		blurDepth.blur(51);

	}

	vw = ofGetWidth();
	vh = ofGetHeight();


	autoChange();
}

void ofApp::autoChange() {
	float tempoBrisa = 10000;
	float now = ofGetElapsedTimef();

	if(now - inicioUltimaBrisa > tempoBrisa) {
		trocaBrisa();
	}
}
void ofApp::atualizaContraste(ofxCvGrayscaleImage &imgGray, int contrasteDistancia) {

	imgGray.mirror(false,true);
	ofPixels & pixNoise = imgGray.getPixels();
	int numPixelsNoise = pixNoise.size();
	for (int i = 0; i < numPixelsNoise; i++) {
		pixNoise[i] = ofClamp(ofMap(pixNoise[i], 0, 255, -contrasteDistancia, 255), 0, 255); // Aumenta contraste de distancia
	}
	imgGray.flagImageChanged();
}

void ofApp::atualizaFantasmaDepth(ofxCvGrayscaleImage imgAtual, float iRastro) {

	ofPixels & pixF = fantasmaDepth.getPixels();
	ofPixels & pixA = imgAtual.getPixels();
	int numPixels = pixF.size();
	for (int i = 0; i < numPixels; i++) {
		pixF[i] = pixF[i] * iRastro + pixA[i] * (1 - iRastro);// Aumenta contraste de distancia
	}
	fantasmaDepth.flagImageChanged();
}
void ofApp::atualizaBlurFantasmaDepth(ofxCvGrayscaleImage imgAtual, float iRastro) {

	blurFantasmaDepth.blur(11);
	blurFantasmaDepth.blur(11);
	blurFantasmaDepth.erode();
	blurFantasmaDepth.dilate();
	blurFantasmaDepth.erode();
	ofPixels & pixF = blurFantasmaDepth.getPixels();
	ofPixels & pixA = imgAtual.getPixels();
	int numPixels = pixF.size();
	for (int i = 0; i < numPixels; i++) {
		pixF[i] =ofClamp(pixF[i] * iRastro + pixA[i] * (1.2 - iRastro),0,255); // Aumenta contraste de distancia
	}
	blurFantasmaDepth.flagImageChanged();
	blurFantasmaDepth.blur(111);
	blurFantasmaDepth.blur(121);

}
void ofApp::atualizaSombraMirror(ofxCvGrayscaleImage imgAtual, float iRastro) {
	sombraMirror = sombra;
	sombraMirror.mirror(false, true);
}
void ofApp::atualizaSombra(ofxCvGrayscaleImage imgAtual, float iRastro) {

	sombra.blur(11);
	sombra.blur(11);
	sombra.erode();
	sombra.dilate();
	sombra.erode();
	ofPixels & pixF = sombra.getPixels();
	ofPixels & pixA = imgAtual.getPixels();
	int numPixels = pixF.size();
	for (int i = 0; i < numPixels; i++) {
		pixF[i] =ofClamp(pixF[i] * iRastro + pixA[i] * (1.2 - iRastro),0,255); // Aumenta contraste de distancia
	}
	sombra.flagImageChanged();
	sombra.blur(11);

}

//--------------------------------------------------------------
void ofApp::draw() {
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		ofSetColor(255, 255, 255);
		//desenhaCameras(false);
	}
	int totalBrisas = 4;

	if(numControl%totalBrisas == 0) {
		desenhaBGcolorido();
		desenhaGirassois(5,4);
	}
	if(numControl%totalBrisas == 1) {
		desenhaBGSol();
		desenhaRaiosois(5,4);
	}
	if(numControl%totalBrisas == 2) {
		desenhaSombra();
	}
	if(numControl%totalBrisas == 3) {
		desenhaSombraMirror();
	}
	desenhaFade();
}

void ofApp::desenhaSombraMirror() {
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(255,0,0);
	sombra.draw(0,0,vw,vh);
	ofSetColor(0,0,255);
	sombraMirror.draw(0,0,vw,vh);
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
}
void ofApp::desenhaSombra() {
	ofSetColor(abs(cos(0.3*agora))*255,abs(sin(0.4*agora))*255,abs(sin(0.5*agora))*255);
	sombra.draw(0,0,vw,vh);
}
void ofApp::desenhaFade() {
	float now = ofGetElapsedTimef();
	float t = 2;
	float diffFade = now - trocouBrisaFade;
	if( diffFade < t) {
		ofSetColor(0,0,0, ofMap(diffFade,0,t,0,255));
		ofRect(0,0, vw, vh);			
	} 
	if( diffFade > t && diffFade < t*2) {
		ofSetColor(0,0,0, ofMap(diffFade-t,0,t,255,0));
		ofRect(0,0, vw, vh);
	}
	if( diffFade > t && trocaBrisaControl) {
		numControl++;
		numControl %= 10;
		trocaBrisaControl = false;
	}
}
void ofApp::desenhaBGcolorido() {
	
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);

	ofBackground(0,190,255);
	ofSetColor(0,255,255); //amarelo
	blurFantasmaDepth.draw(0, 0, ofGetWidth(), ofGetHeight());

	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofSetColor(2,255,52); //verdeloco
	blurFantasmaDepth.draw(0, 0, ofGetWidth(), ofGetHeight());
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
}

void ofApp::desenhaBGSol() {

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(0,180,200); //ciano
	ofBackground(0,180,200);
	ofSetColor(255,255,0); //amarelo
	blurFantasmaDepth.draw( 0, 0, vw, vh);
}

void ofApp::desenhaGirassois(int qtdColW, int qtdColH) {

	// aponta pra imagem base
	ofPixels & pixNoise = blurFantasmaDepth.getPixels();
	int numPixelsNoise = pixNoise.size();
	// calcula como percorrerá a imagem
	float intervaloWKinect = 640 / qtdColW;
	float intervaloHKinect = 480 / qtdColH;
	//calcula onde desenhará o resultado
	int margem = 100; // espaçamento das bordas
	float distanciaW = (ofGetWidth() - margem)/qtdColW;
	float distanciaH = (ofGetHeight() - margem)/qtdColH;
	int bright = 0;

	for (int iY = 0; iY < qtdColH; iY++) {

		for (int iX = 0; iX < qtdColW; iX++) {

			// Lê a posição do Kinect
			int xKinect = iX * intervaloWKinect;
			int yKinect = iY * intervaloHKinect;

			int i = xKinect + yKinect * 640; //Pega o indice da coluna na img corrida
			bright = pixNoise[i];

			// Desenha a posição lida
			desenhaGirassol(iX*distanciaW+margem, iY*distanciaH+margem, bright);
		}
	}

}

//--------------------------------------------------------------
void ofApp::desenhaGirassol(int x, int y, float bright) {
	glPushMatrix();

	glTranslatef(x, y, 0);

	ofSetColor(255,0,255);
	//ofDrawCircle(0,0, bright);

	girassol.setAnchorPercent(0.5, 0.5);

	float escala = ofMap(bright, 0, 255, 0.2, 1);
	ofScale(escala, escala);

	float variacao = sin(ofGetElapsedTimef());
	int rotacao = variacao*360*(ofClamp(escala-0.3,0,100));
	ofRotateZ(rotacao);
	ofSetColor(255,255,255);
	girassol.draw(0,0);

	glPopMatrix();
}

void ofApp::desenhaRaiosois(int qtdColW, int qtdColH) {

	// aponta pra imagem base
	ofPixels & pixNoise = blurFantasmaDepth.getPixels();
	int numPixelsNoise = pixNoise.size();
	int vw = ofGetWidth();
	int vh = ofGetHeight();
	// calcula como percorrerá a imagem
	float intervaloWKinect = 640 / qtdColW;
	float intervaloHKinect = 480 / qtdColH;
	//calcula onde desenhará o resultado
	int margem = 100; // espaçamento das bordas
	float distanciaW = (ofGetWidth() - margem)/qtdColW;
	float distanciaH = (ofGetHeight() - margem)/qtdColH;
	int bright = 0;

	for (int iY = 0; iY < qtdColH; iY++) {

		for (int iX = 0; iX < qtdColW; iX++) {

			// Lê a posição do Kinect
			int xKinect = iX * intervaloWKinect;
			int yKinect = iY * intervaloHKinect;

			int i = xKinect + yKinect * 640; //Pega o indice da coluna na img corrida
			bright = pixNoise[i];

			// Desenha a posição lida
			desenhaRaiosol(iX*distanciaW+margem, iY*distanciaH+margem, bright);
		}
	}

}
//--------------------------------------------------------------
void ofApp::desenhaRaiosol(int x, int y, float bright) {
	glPushMatrix();

	glTranslatef(x, y, 0);

	ofSetColor(255,0,255);
	//ofDrawCircle(0,0, bright);

	raiosol.setAnchorPercent(0.5, 0.5);

	float escala = ofMap(bright, 0, 255, 0.2, 1);
	ofScale(escala, escala);

	float variacao = sin(ofGetElapsedTimef());
	int rotacao = variacao*360*(ofClamp(escala-0.3,0,100));
	ofRotateZ(rotacao);
	ofSetColor(255,255,255);
	raiosol.draw(0,0);

	glPopMatrix();
}

void ofApp::desenhaCameras(bool desenhaInstrucoes) {
	// draw from the live kinect
	kinect.drawDepth(10, 10, 300, 225);
	kinect.draw(320, 10, 300, 225);
		contourFinder.draw(630, 10, 300, 225);

	contrasteDepth.draw(10, 245, 300, 225);
	fantasmaDepth.draw(320, 245, 300, 225);
	ofSetColor(0,200,200); //ciano
	blurDepth.draw(630, 245, 300, 225);


	ofSetColor(200,200,0); //amarelo
	blurFantasmaDepth.draw(320, 480, 300, 225);


	
	if (desenhaInstrucoes) {
		// draw instructions
		ofSetColor(255, 255, 255);
		stringstream reportStream;

		if (kinect.hasAccelControl()) {
			reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
				<< ofToString(kinect.getMksAccel().y, 2) << " / "
				<< ofToString(kinect.getMksAccel().z, 2) << endl;
		}
		else {
			reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
				<< "motor / led / accel controls are not currently supported" << endl << endl;
		}

		reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
			<< "set near threshold " << nearThreshold << " (press: + -)" << endl
			<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
			<< ", fps: " << ofGetFrameRate() << endl
			<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

		if (kinect.hasCamTiltControl()) {
			reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
				<< "press 1-5 & 0 to change the led mode" << endl;
		}

		ofDrawBitmapString(reportStream.str(), 20, 652);
	}

	
}
void ofApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinect.getColorAt(x,y));
				mesh.addVertex(kinect.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::exit() {
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void ofApp::trocaBrisa() {
	inicioUltimaBrisa = agora;
	trocouBrisaFade = agora;
	trocaBrisaControl = true;
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch (key) {
		case ' ':
			trocaBrisa();
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;
			
		case 'c':
			inicioUltimaBrisa = agora;
			break;
			
		case '1':
			kinect.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinect.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinect.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinect.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinect.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{

}