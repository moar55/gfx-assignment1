#include <GL/glut.h>
#include <cstdio>
#include <math.h>
#include <vector>
#include <cstring>
#include "random"
#include "soil-master/src/SOIL/SOIL.h"

struct point
{
    float x;
    float y;
};

struct path {
    point firstPoint;
    point secondPoint;
    point thirdPoint;
    point fourthPoint;
};

std::vector<path> paths;

struct movingObject {
    float currIteration;
    float currIteration;
    point currPoint;
    path objPath;
    bool isCrystal;
};

std::vector<movingObject> movingObjects;

void DrawArm(float x_start, float y_start, bool left, float length);
GLuint texID,
       spikes,
       spikes2,
       heisenberg,
       cashPile,
       meth;

int p0[2],
    p1[2],
    p2[2],
    p3[2];


std::random_device rd;
std::mt19937 mt(rd());

const int obstaclesGenTime = 1200;
int collectiblesGenTime;

int w1 = 0;
int h1 = 0;

int globalTime;

bool done = false;

int iw = 1280;
int ih = 720;
const float  PI = 3.1415926f;

int scrollX = 0;
int y_up = 0; // Offset for translating the character upon up and down buttons
int charXOffset = 0;

point charPosition[4];

void reshape(int w, int h)
{
    w1 = w;
    h1 = h;
    iw = w;
    ih = h;
    glViewport(0, 0, w, h);
}


void orthogonalStart()
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-w1/2, w1/2, -h1/2, h1/2);
    glMatrixMode(GL_MODELVIEW);
}

void orthogonalEnd()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


// simple linear interpolation between two points
void lerp(point& dest, const point& a, const point& b, const float t)
{
    dest.x = a.x + (b.x-a.x)*t;
    dest.y = a.y + (b.y-a.y)*t;
}

// evaluate a point on a bezier-curve. t goes from 0 to 1.0
void bezier(point &dest, const point& a, const point& b, const point& c, const point& d, const float t)
{
    point ab,bc,cd,abbc,bccd;
    lerp(ab, a,b,t);           // point between a and b (green)
    lerp(bc, b,c,t);           // point between b and c (green)
    lerp(cd, c,d,t);           // point between c and d (green)
    lerp(abbc, ab,bc,t);       // point between ab and bc (blue)
    lerp(bccd, bc,cd,t);       // point between bc and cd (blue)
    lerp(dest, abbc,bccd,t);   // point on the bezier-curve (black)
}


void genTexture(movingObject object) {

//    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, object.texture);

    glPushMatrix();

//    printf("the values are %f and %f and the i is %f\n", object.currPoint.x, object.currPoint.y, object.currIteration);

    glScalef(0.5, 0.5, 0);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);  glVertex2f(object.currPoint.x, object.currPoint.y + 50);
    glTexCoord2f(1,0);  glVertex2f(object.currPoint.x + 50 , object.currPoint.y + 50);
    glTexCoord2f(1,1);  glVertex2f(object.currPoint.x  +50, object.currPoint.y);
    glTexCoord2f(0,1);  glVertex2f(object.currPoint.x, object.currPoint.y);
    glEnd();

    glPopMatrix();
}

void genCash() {
    glBindTexture(GL_TEXTURE_2D, cashPile);
    glPushMatrix();
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);  glVertex2f(400,170);
    glTexCoord2f(1,0);  glVertex2f(600, 170);
    glTexCoord2f(1,1);  glVertex2f(600, 70);
    glTexCoord2f(0,1);  glVertex2f(400, 70);
    glEnd();
    glPopMatrix();

}

void background()
{
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture( GL_TEXTURE_2D, texID);

    orthogonalStart();

    // texture width/height

    glPushMatrix();
    glTranslatef( -iw/2 - scrollX, -ih/2, 0 );
    glBegin(GL_QUADS);


    glTexCoord2f(0,0); glVertex2i(0, 0);
    glTexCoord2f(1,0); glVertex2i(iw , 0);
    glTexCoord2f(1,1); glVertex2i(iw, ih);
    glTexCoord2f(0,1); glVertex2i(0, ih);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef( iw/2 - scrollX, -ih/2, 0 );
    glBegin(GL_QUADS);

    glTexCoord2f(0,0); glVertex2i(0, 0);
    glTexCoord2f(1,0); glVertex2i(iw , 0);
    glTexCoord2f(1,1); glVertex2i(iw, ih);
    glTexCoord2f(0,1); glVertex2i(0, ih);
    glEnd();
    glPopMatrix();

    orthogonalEnd();
}

GLvoid draw_circle(const GLfloat radius,const GLuint num_vertex, GLuint texture)
{
    GLfloat vertex[4];
    GLfloat texcoord[2];

    const GLfloat delta_angle = 2.0*M_PI/num_vertex;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,texture);
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
    glBegin(GL_TRIANGLE_FAN);

    //draw the vertex at the center of the circle
    texcoord[0] = 0.5;
    texcoord[1] = 0.5;
    glTexCoord2fv(texcoord);

    vertex[0] = vertex[1] = vertex[2] = 0.0;
    vertex[3] = 1.0;
    glVertex4fv(vertex);

    for(int i = 0; i < num_vertex ; i++)
    {
        texcoord[0] = (cosf(delta_angle*i) + 1.0)*0.5;
        texcoord[1] = (sinf(delta_angle*i) + 1.0)*0.5;
        glTexCoord2fv(texcoord);

        vertex[0] = cosf(delta_angle*i) * radius;
        vertex[1] = sinf(delta_angle*i) * radius;
        vertex[2] = 0.0;
        vertex[3] = 1.0;
        glVertex4fv(vertex);
    }

    texcoord[0] = (1.0 + 1.0)*0.5;
    texcoord[1] = (0.0 + 1.0)*0.5;
    glTexCoord2fv(texcoord);

    vertex[0] = 1.0 * radius;
    vertex[1] = 0.0 * radius;
    vertex[2] = 0.0;
    vertex[3] = 1.0;
    glVertex4fv(vertex);
    glEnd();

    glDisable(GL_TEXTURE_2D);

}

void Display()  {

    if(done)
        return;

    glClearColor (0.5,0.5,0.5,0.0);
//    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glClear (GL_COLOR_BUFFER_BIT);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glLoadIdentity();
    glEnable( GL_TEXTURE_2D);
    background();
    glDisable(GL_TEXTURE_2D);

    gluOrtho2D(0.0, 500, 0.0, 250);

    glPushMatrix();

    glTranslatef(200 + charXOffset, y_up, 0);


////    HEAD
    glPushMatrix();
    glTranslated(40, 150, 0);
    draw_circle(10, 20, heisenberg);
    glPopMatrix();

//    Torso
    glBegin(GL_QUADS);
    glVertex3f(32.0f, 141.0f, 0.0f);
    glVertex3f(45.0f, 141.0f, 0.0f);
    glVertex3f(45.0f, 121.0f, 0.0f);
    glVertex3f(32.0f, 121.0f, 0.0f);
    glEnd();

    /* First and Second Arm */
    DrawArm(44, 135, false, 20);
    DrawArm(32, 135, true, 20);

//    LEGS
//    First Leg
    glBegin(GL_LINES);
    glVertex3f(36, 121, 0);
    glVertex3f(30, 111, 0);
    glEnd();
//    Second Leg
    glBegin(GL_LINES);
    glVertex3f(41, 121, 0);
    glVertex3f(47,111, 0);
    glEnd();
    glPopMatrix();

//


    glEnable(GL_TEXTURE_2D);
//    Generate obstacles and collectibles
    for (int i = 0 ; i < movingObjects.size(); i++) {
        genTexture(movingObjects[i]);
    }
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);

    genCash();
    glDisable(GL_TEXTURE_2D);


    glFlush();
}

void loadTextures() {
    texID = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/bg.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     spikes = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/spikes.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    spikes2 = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/spikes2.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    heisenberg = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/heisenberg1.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    meth = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/meth.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );

    cashPile = SOIL_load_OGL_texture // load an image file directly as a new OpenGL texture
            (
                    "/home/moar/CLionProjects/Assignment1/cash-pile.png",
                    SOIL_LOAD_AUTO,
                    SOIL_CREATE_NEW_ID,
                    SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
            );


    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    //to the edge of our shape.
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

}

int* bezier(float t, float* p0,float* p1,float* p2,float* p3)
{
    int res[2];
    res[0]= pow((1-t),3)*p0[0]+3*t*pow((1-t),2)*p1[0]+3*pow(t,2)*(1-t)*p2[0]+pow(t,3)*p3[0];
    res[1]=pow((1-t),3)*p0[1]+3*t*pow((1-t),2)*p1[1]+3*pow(t,2)*(1-t)*p2[1]+pow(t,3)*p3[1];
    return res;
}

void DrawArm(float x_start, float y_start, bool left, float length) {

    float radius = (1.0f /6 ) * length;
    float upper_line_endX = (!left)?(x_start + (length * cosf(PI /4))) - (radius * cosf(PI / 4)) :
                            (x_start - (length * cosf(PI /4))) - (radius * cosf(PI / 4));

    float upper_line_endY = (y_start + (length * sinf(PI /4))) - (radius * sinf(PI / 4));

    glLineWidth(5);

    glBegin(GL_LINES);
    glVertex3f(x_start, y_start, 0);
    glVertex3f(upper_line_endX, upper_line_endY, 0);
    glEnd();

    float x2_start = x_start;
    float y2_start =  y_start - 10;

    glBegin(GL_LINES);
    glVertex3f(x2_start, y2_start, 0);
    length += 10;
    float lower_line_endX = (!left)?(x2_start + (length * cosf(PI /4))) - (radius * cosf(PI / 4)):
                            (x2_start - (length * cosf(PI /4))) - (radius * cosf(PI / 4));
    float lower_line_endY = (y2_start + (length * sinf(PI /4))) - (radius * sinf(PI / 4));
    glVertex3f(lower_line_endX, lower_line_endY, 0.00f);
    glEnd();

    glPointSize(5);

    float p0 [2] = {upper_line_endX, upper_line_endY};
    float p1 [2] = {(!left)?upper_line_endX + (radius * 2 * cosf(PI /4)) : upper_line_endX - (radius * 2 * cosf(PI /4)),
                    upper_line_endY + (radius * 1.1 * sin(PI /4))};
    float p2 [2] = {(!left)?lower_line_endX + (radius * 2 * cosf(PI /4)): lower_line_endX - (radius * 2 * cosf(PI /4)),
                    lower_line_endY+ (radius  * 1.1 * sin(PI /4))};
    float p3 [2] = {lower_line_endX, lower_line_endY};

    glBegin(GL_POINTS);
    for(float t1=0;t1<1;t1+=0.001)
    {
        int* p =bezier(t1,p0,p1,p2,p3);
        glVertex3f(p[0],p[1],0);
    }
    glEnd();

}

void SpecialFunction(int k, int x, int y)
{
    if (k == GLUT_KEY_UP && y_up < 90)
        y_up += 30;


    if (k == GLUT_KEY_DOWN && y_up >= -60)
        y_up -=30;

    const float lowerBound = 111  + y_up;
    const float upperBound = y_up + 160;

    charPosition[0].y = upperBound;
    charPosition[1].y = upperBound;
    charPosition[2].y = lowerBound;
    charPosition[3].y = lowerBound;

    glutPostRedisplay();//redisplay to update the screen with the changes
}
//

bool inRange(point vertex) {
    if(vertex.x  >= charPosition[0].x )
        printf("some left bound\n");
    if( vertex.x <= charPosition[1].x)
        printf("some right bound\n");

    if(vertex.y <= charPosition[0].y)
        printf("some upper bound\n");

    if(vertex.y >= charPosition[2].y)
        printf("some lower bound\n");

    return vertex.x  >= charPosition[0].x  && vertex.x <= charPosition[1].x
    && vertex.y <= charPosition[0].y && vertex.y >= charPosition[2].y;
}

void checkForCollision() {


    for (int i = 0 ; i < movingObjects.size(); i++) {
        const float movingObjectLeftBound = movingObjects[i].currPoint.x;
        const float movingObjectRightBound = movingObjects[i].currPoint.x + 50;
        const float movingObjectUpperBound = movingObjects[i].currPoint.y + 50;
        const float movingObjectLowerBound = movingObjects[i].currPoint.y;

        point leftUpper = {movingObjectLeftBound * 0.5, movingObjectUpperBound * 0.5};
        point rightUpper = {movingObjectRightBound * 0.5, movingObjectUpperBound * 0.5};
        point leftLower = {movingObjectLeftBound * 0.5, movingObjectLowerBound * 0.5};
        point rightLower = {movingObjectRightBound * 0.5, movingObjectLowerBound * 0.5};

        if(inRange(leftUpper) || inRange(rightUpper) || inRange(leftLower) || inRange(rightLower)) {
            charXOffset=(movingObjects[i].isCrystal)? charXOffset + 100 : charXOffset - 100;
            charPosition[0].x = charPosition[0].x + charXOffset;
            charPosition[1].x = charPosition[1].x + charXOffset;
            charPosition[2].x = charPosition[2].x + charXOffset;
            charPosition[3].x = charPosition[3].x + charXOffset;
            movingObjects.erase(movingObjects.begin() + i);
            if(charXOffset >= 300 || charXOffset <= -300)
                done = true;
            glutPostRedisplay();

        } else {
//            printf("not in rande\n");
        }


//        if(inRange(leftUpper, leftLower) || inRange(rightUpper, rightLower))
//            printf("hit!");
//        if(moving) {
//            printf("hit!");
//        }
//        printf("\t%f %f %f %f\n", movingObjectLeftBound, movingObjectRightBound, movingObjectUpperBound, movingObjectLowerBound);

    }

}


void timef(int val)//timer animation function, allows the user to pass an integer valu to the timer function.
{
    globalTime ++;
    checkForCollision();

//    printf("global time is %d", globalTime);
    scrollX += 1;
    if(scrollX > iw)
        scrollX = 0;

    for (int i = 0 ; i < movingObjects.size(); i++) {
        movingObjects[i].currIteration += 1;
        float t = static_cast<float>(movingObjects[i].currIteration)/999.0;
        bezier(movingObjects[i].currPoint,movingObjects[i].objPath.firstPoint,movingObjects[i].objPath.secondPoint,movingObjects[i].objPath.thirdPoint,movingObjects[i].objPath.fourthPoint,t);

        if(movingObjects[i].currIteration > 1500) // erase objects out of bounds to free memory
            movingObjects.erase(movingObjects.begin() + i);

    }

    if(globalTime % obstaclesGenTime == 0) {

        std::uniform_real_distribution<double> dist(0, 10);
        GLint texture = (dist(mt) < 5)? spikes : spikes2;
        point p;
        float t = static_cast<float>(0)/999.0;
        std::uniform_real_distribution<double> dist2(1, paths.size());
        int j = dist2(mt) - 1;
        bezier(p,paths[j].firstPoint,paths[j].secondPoint,paths[j].thirdPoint,paths[j].fourthPoint,t);
        movingObjects.push_back(movingObject{texture, 0, p, paths[j], false});
    }

    if(globalTime % collectiblesGenTime == 0) {
        std::uniform_real_distribution<double> dist(600, obstaclesGenTime);
        point p;
        float t = static_cast<float>(0)/999.0;
        std::uniform_real_distribution<double> dist2(1, paths.size());
        int j = dist2(mt) -1 ;
        bezier(p,paths[j].firstPoint,paths[j].secondPoint,paths[j].thirdPoint,paths[j].fourthPoint,t);
        movingObjects.push_back(movingObject{meth, 0, p, paths[j], true});
    }

    if(globalTime % 7200 == 0) {
        std::uniform_real_distribution<double> dist(5000, 7200);
        int random = dist(mt);
        collectiblesGenTime = random;
    }


    glutPostRedisplay();                        // redraw
    glutTimerFunc(1, timef, 0);                    //recall the time function after 1000
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);

    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Assignment1");

    glutDisplayFunc(Display);
    glutSpecialFunc(SpecialFunction);
    glutTimerFunc(0,timef,0);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


    p0[0]=100;
    p0[1]=100;

    p1[0]= 100;
    p1[1]=400;

    p2[0]=500;
    p2[1]=400;

    p3[0]=500;
    p3[1]=100;

    point a = { 800, 300};
    point b = { 500, 500  };
    point c = { 100, 100 };
    point d = { -100, 300 };
    paths.push_back({a, b, c, d});

    for (int i = 0; i < 1000; ++i) {
        point p;
        float t = static_cast<float>(i)/999.0;
        bezier(p,a,b,c,d,t);
        printf("%f %f\n", p.x, p.y);
    }

    a = {700, 400};
    b = {400, 50};
    c = { 100, 400 };
    d = { -100, 300 };
    paths.push_back({a, b, c, d});

    a = {800, 100};
    b = {200, 300};
    c = { 100, 400 };
    d = { -100, 350 };
    paths.push_back({a, b, c, d});

    a = {800, 100};
    b = {200, -200};
    c = { 100, 400 };
    d = { -100, 350 };
    paths.push_back({a, b, c, d});


    a = {800, 100};
    b = {400, -200};
    c = { 100, 400 };
    d = { -100, 350 };
    paths.push_back({a, b, c, d});

    const float leftBound = (32 - (20 * cosf(PI /4))) - (((1.0f /6 ) * 20) * cosf(PI / 4)) + 190    ;
    const float rightBound =  (40 + (20 * cosf(PI /4))) - (((1.0f /6 ) * 20) * cosf(PI / 4)) + 210;
    const float lowerBound = 111  + y_up;
    const float upperBound = y_up + 160;

    charPosition[0] = {leftBound, upperBound};
    charPosition[1] = {rightBound, upperBound};
    charPosition[2] = {leftBound, lowerBound};
    charPosition[3] = {rightBound, lowerBound};

//    glutKeyboardFunc(keyPressed);
//    glutKeyboardUpFunc(keyUp);
    glutReshapeFunc(reshape);
    loadTextures();
    gluOrtho2D(0.0, 500, 0.0, 250);
    collectiblesGenTime = 2000;
    glutMainLoop();

}
