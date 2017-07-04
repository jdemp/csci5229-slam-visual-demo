/*
 *  Lighting
 *
 */
#include "CSCIx229.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>


#define MATCH 0
#define LANDMARK_CAMERA_0 1
#define LANDMARK_CAMERA_1 2
#define BAD_MATCH 3
#define TRANSFORM 4

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle       //  Field of view (for perspective)
int light=0;      //  Lighting
int theta_loc = 220;         //  angle for locomotion
int fov=90;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
double eye_x = 5;
double eye_y = 5;
double eye_z = 1.5;
int view = 1;
// Light values
int one       =   1;  // Unit value
int distance  =   10;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float zlight  =   0;  // Elevation of light
int iteration = 0;
int step = 0;
int potential_landmarks_index = 0;
int camera_corners[4][3] ={
              {1,1,1},
              {1,1,-1},
              {-1,1,1},
              {-1,1,-1}
};

double demo_poses_array[10][4] = {
              {-3,-3,1.2,-20},//1
              {-0,-3,1.2,20},//2
              {2.5,-2,1.1,45},//3
              {3.2,0,1.1,80},//4
              {2.7,2,1.0,120},//5
              {1.0,2,1.2,160},//6
              {-.5,2,1.3,165},//7
              {-1.5,2.4,1.3,180},//8
              {-2.85,0,1.2,250},//9
              {-2.8,-1,1.1,290}//10
};

double loop_closure_array[10][4] = {
              {-3,-3,1.2,-20},//1
              {-0,-3,1.2,20},//2
              {2.5,-2,1.1,45},//3
              {3.2,0,1.1,80},//4
              {2.7,2,1.1,120},//5
              {1.0,2,1.1,160},//6
              {-.5,2,1.1,165},//7
              {-1.5,2.4,1.1,180},//8
              {-2.85,0   ,1.2,250},//9
              {-3,-3,1.2,-20}//10
};
int num_landmarks =24;
double init_landmarks_array[24][3] ={
                {0,0,0},
                {2,1,.9},
                {2,-1,.9},
                {-2,1,.9},
                {-2,-1,.9},//4
                {-1.5,-.5,.9},//5
                {-.10,-.45,1},//6
                {1.9,1,.9},//7
                {-.5,.5,.9},//8
                {2,1,.6},//9
                {2,-1,.7},//10
                {-2,1,.5},//11
                {-2,-1,.8},//12
                {1.5,-.5,.9},//13
                {-0,-.5,1},//14
                {-1.9,1,.9},//15
                {-.5,-.5,.9},//16
                {0,-1,.8},//`17
                {1.5,-1,.85},//18
                {-1.4,-1,.79},//19
                {-1.5,-.8,1},//20
                {.35,-.5,.95},//21
                {2,.5,.86},//22
                {1.5,1,.86}//23

};

int cam_landmark_array[10][10]={
            {3,4,12,20,19,5,0,0,0,0},
            {3,4,19,20,5,6,0,0,0,0},
            {1,6,4,2,13,10,17,21,0,0},
            {1,9,21,22,13,0,0,0,0,0},
            {9,3,1,21,23,6,0,0,0,0},//5
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0,0,0},
            {3,4,12,20,19,5,0,0,0,0}//10
};

bool camera_transforms[10];

struct Pose{
  double x;
  double y;
  double z;
  double d;
};

struct Landmark{
  double x;
  double y;
  double z;
};

struct Point{
  double x;
  double y;
  double z;
};

struct Camera{
  struct Pose pose;
  //the indexes of landmarks this camera can see (up to 10)
  // the corners of the camera
  struct Point camera_corners[4];
  bool draw_landmarks;
  bool visible;
  int visible_landmarks[10];
  bool show_new;
  bool show_old;
  bool is_selected;
  bool show_camera_points;
};


unsigned int texture[4]; // Textures
unsigned int objects[4];
struct Camera cameras[10];
struct Landmark landmarks[100];
/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */


 static void cube(double x,double y,double z,
                  double dx,double dy,double dz,
                  double th)
 {
    //  Set specular color to white
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);
    //  Enable textures
    //  Front
    //glColor4f(.5,.5,.5,1);
    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    glEnd();
    //  Back
    glBegin(GL_QUADS);
    //glColor4f(1,1,1,1);
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
    glEnd();
    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
    glEnd();
    //  Lef
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    glEnd();
    glPopMatrix();

}

void seat()
{
  cube(0,0,.4,.5,.5,.05,0);
  cube(.45,.45,.2,.05,.05,.2,0);
  cube(-.45,.45,.2,.05,.05,.2,0);
  cube(.45,-.45,.2,.05,.05,.2,0);
  cube(-.45,-.45,.2,.05,.05,.2,0);
}

void table()
{
  //will apply texture
  glColor3f(.5,.35,.05);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  float brown[] = {.2,.1,.05,1};
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  cube(0,0,.8,2,1,.1,0);
  cube(1.9,.9,.4,.1,.1,.4,0);
  cube(-1.9,.9,.4,.1,.1,.4,0);
  cube(1.9,-.9,.4,.1,.1,.4,0);
  cube(-1.9,-.9,.4,.1,.1,.4,0);

}

void ground()
{
  glColor4f(1,1,1,1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  float brown[] = {.5,.35,.05,1};
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  cube(0,0,-.1,5,5,.05,0);
}

void walls()
{
  glColor4f(1,1,1,1);
  glBegin(GL_QUADS);
  glNormal3f(-1,-0,0);
  glVertex3f(5,5,-.1);
  glVertex3f(5,-5,-.1);
  glVertex3f(5,-5,5);
  glVertex3f(5,5,5);

  glNormal3f(0,-1,0);
  glVertex3f(5,5,-.1);
  glVertex3f(-5,5,-.1);
  glVertex3f(-5,5,5);
  glVertex3f(5,5,5);

  glNormal3f(1,0,0);
  glVertex3f(-5,5,-.1);
  glVertex3f(-5,-5,-.1);
  glVertex3f(-5,-5,5);
  glVertex3f(-5,5,5);

  glNormal3f(0,1,0);
  glVertex3f(5,-5,-.1);
  glVertex3f(-5,-5,-.1);
  glVertex3f(-5,-5,5);
  glVertex3f(5,-5,5);
  glEnd();
}

/*
 *  Draw a pole
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void pole(double height, double radius)
{
  const int d = 5;
  int theta;
  double z;
  for (z=0;z<=height;z+=.1)
  {
    glBegin(GL_QUAD_STRIP);
    //glColor3f(0,1,0);

    for (theta=0;theta<=360;theta+=d)
    {
      double x = radius*Cos(theta);
      double y = radius*Sin(theta);
      glNormal3f(x,y,0);
      glTexCoord2f(x,y);
      glVertex3f(x,y,z);
      glNormal3f(x,y,0);
      glTexCoord2f(x,y);
      glVertex3f(x,y,z+.1);

    }
    glEnd();
  }
  glPopMatrix();
}



static void hanoi_stand(double space_between_poles)
{


  glColor3f(.1,.1,.1);
  glBindTexture(GL_TEXTURE_2D,texture[0]);
  float brown[] = {.1,.1,.1,1};
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  cube(0,0,0,2,6,.1,0);
  glPushMatrix();
  glTranslated(0,0,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(1,.1);

  glPushMatrix();
  glTranslated(0,-1*space_between_poles,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(1,.1);

  glPushMatrix();
  glTranslated(0,space_between_poles,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(1,.1);
}

 static void torus(double a, double c)
 {
   const int d=5;
   int theta,phi;
    for (theta=0;theta<=360;theta+=d)
    {
      glBegin(GL_QUAD_STRIP);
      //glColor3f(red,green,blue);
      for (phi=0;phi<=360;phi+=d)
      {
        double x = (c+a*Cos(theta))*Cos(phi);
        double y = (c+a*Cos(theta))*Sin(phi);
        double z = a*Sin(theta);
        double x1 = (c+a*Cos(theta+d))*Cos(phi+d);
        double y2 = (c+a*Cos(theta+d))*Sin(phi+d);
        double z3 = a*Sin(theta+d);
        glNormal3f(x,y,z);
        glTexCoord2f(x,y);
        glVertex3f(x,y,z);
        glNormal3f(x1,y2,z3);
        glTexCoord2f(x1,y2);
        glVertex3f(x1,y2,z3);
      }
      glEnd();

    }
    //glEnd();




    //  Undo transofrmations
    glPopMatrix();
 }


void line(double x, double y, double z, double x1, double y1, double z1, int type)
{
  glLineWidth(5);
  glBegin(GL_LINES);
  if (type==MATCH) glColor4f(0,1,0,1);
  else if (type==BAD_MATCH) glColor4f(1,0,0,1);
  else if (type==LANDMARK_CAMERA_0) glColor4f(0,0,1,1);
  else if (type==LANDMARK_CAMERA_1) glColor4f(1,0,1,1);
  else if (type==TRANSFORM) glColor4f(0,1,0,1);
  else glColor4f(.5,.5,.5,1);
  glVertex4d(x,y,z,1);
  glVertex4d(x1,y1,z1,1);
  glEnd();
}

void landmark(struct Landmark loc)
{
  glPointSize(10);
  glBegin(GL_POINTS);
  //glColor4f(1,1,0,1);
  glVertex4d(loc.x,loc.y,loc.z,1);
  glEnd();
}

void camera(struct Pose pose)
{
  //may redraw as lines instead of polygons
  glPushMatrix();
  glTranslated(pose.x,pose.y,pose.z);
  glRotated(pose.d,0,0,1);
  glScaled(.5,.5,.5);
  glLineWidth(5);
  glBegin(GL_LINE_STRIP);
  glColor4f(1,1,1,1);
  glVertex3d(0,0,0);
  glVertex3d(1,1,1);
  glVertex3d(1,1,-1);

  glVertex3d(0,0,0);
  glVertex3d(1,1,1);
  glVertex3d(-1,1,1);

  glVertex3d(0,0,0);
  glVertex3d(-1,1,1);
  glVertex3d(-1,1,-1);

  glVertex3d(0,0,0);
  glVertex3d(-1,1,-1);
  glVertex3d(1,1,-1);

  glEnd();

  glBegin(GL_QUADS);
  glColor4f(.5,.5,.5,.5);
  glVertex3d(1,1,1);
  glVertex3d(-1,1,1);
  glVertex3d(-1,1,-1);
  glVertex3d(1,1,-1);
  glEnd();

  glPopMatrix();
}

//finds landmarks visible to camera and adds 10 to its list
void calcLandmarks()
{
  cameras[iteration].draw_landmarks = true;
  //will calc additional ones and verify given ones
}

//sets the visibility of the camera-landmark lines
void setCamLines(int newCamID, int oldCamID)
{
  cameras[newCamID].show_new=true;
  if (oldCamID>0)
  {
    cameras[oldCamID].show_old=true;
    cameras[oldCamID].show_new=false;
    cameras[oldCamID-1].show_old=false;
  }
  else if (oldCamID==0)
  {
    cameras[oldCamID].show_old=true;
    cameras[oldCamID].show_new=false;
  }
}


void next_step()
{
  if (iteration==10)
  {
    for (int i=0;i<10;i++)
    {
      cameras[i].pose.x = loop_closure_array[i][0];
      cameras[i].pose.y = loop_closure_array[i][1];
      cameras[i].pose.z = loop_closure_array[i][2];
      cameras[i].pose.d = loop_closure_array[i][3];
      /*calc camera camera conrers
      for (int i=0;i<4;i++)
      {
        cameras[i].camera_corners[i].x = .5*camera_corners[i][0]*Cos(cameras[i].pose.d) - camera_corners[i][1]*Sin(cameras[i].pose.d) + cameras[i].pose.x;
        cameras[i].camera_corners[i].y = camera_corners[i][0]*Sin(cameras[i].pose.d) + .5*camera_corners[i][1]*Cos(cameras[i].pose.d) + cameras[i].pose.y;
        cameras[i].camera_corners[i].z = .5*camera_corners[i][2] + cameras[i].pose.z;
      }
      */
    }
    iteration++;


  }

  else if (iteration==9)
  {
    cameras[9].visible=true;
    cameras[9].draw_landmarks=true;
    cameras[9].show_new=true;
    cameras[0].show_old=true;
    iteration++;
  }

  else if (iteration>4)
  {
    step=0;
    cameras[iteration].visible=true;
    camera_transforms[iteration]=true;
    iteration++;

  }

  else if (step==0) //add a camera
  {
    cameras[iteration].visible=true;
    step++;
  }
  else if (step==1) //detect landmarks in frame of camera
  {
    calcLandmarks();
    step++;
  }
  else if (step==2) //show the landmarks connected to camera
  {
    setCamLines(iteration,iteration-1);
    step++;
  }
  else if (step==3 && iteration!=0) //show correspondences between the new cameras
  {
    cameras[iteration].show_camera_points=true;
    step++;
  }
  else if (step==4 && iteration!=0) //draw a line showing transform
  {

    camera_transforms[iteration]=true;
    cameras[iteration].show_camera_points=false;
    cameras[iteration-1].show_old=false;
    cameras[iteration].show_new=false;
    //increment
    step=0;
    iteration++;
  }
  else
  {
    cameras[iteration].show_camera_points=false;
    cameras[iteration].show_old=false;
    cameras[iteration].show_new=false;
      step=0;
      iteration++;
  }
}

void display()
{

    float Ambient[]   = {.5,.5,.5,1.0};
    float Diffuse[]   = {.5,.5,.5,1.0};
    float Specular[]  = {1,1,1,1.0};
    float Position[] = {0,0,5,1};
   //const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   //  Undo previous transformations
   glLoadIdentity();
   if(light){
   glColor3f(1,1,1);
   glPushMatrix();
   glTranslated(0,0,5);
   glutSolidSphere(0.03,10,10);
   glPopMatrix();
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
 }

   gluLookAt(eye_x,eye_y, eye_z , eye_x + Cos(theta_loc),eye_y + Sin(theta_loc),eye_z , 0,0,1);

   //  Flat or smooth shading
   glShadeModel(GL_SMOOTH);

   //  Light switch
   //rescale these to fit on the table
   //  Draw scene
   if(view%3!=2)
   {

   glPushMatrix();
   glTranslated(-.5,-.4,.9);
   glRotated(80,0,0,1);
   glScaled(.15,.15,.2);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,.25);
   //glRotated(90,0,1,0);
   glScaled(1,1,.25);
   glColor4f(1,0,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,.7);
   //glRotated(90,0,0,1);
   glScaled(.75,.75,.25);
   glColor4f(1,1,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,.9);
   //glRotated(90,0,0,1);
   glScaled(.5,.5,.25);
   glColor4f(0,1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,1.25);
   //glRotated(90,0,0,1);
   glScaled(.25,.25,.25);
   glColor4f(1,0,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   torus(.5,1);
   hanoi_stand(4);

   glPushMatrix();
   //  Offset
   glTranslated(0,4,1.6);
   //glRotated(90,0,0,1);
   glScaled(1.25,1.25,0.25);
   glColor4f(0,1,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[1]);
   torus(.5,1);
   hanoi_stand(4);

   glPopMatrix();
   table();

   glColor3f(.5,.3,.3);
   glPushMatrix();
    glTranslated(-.75,-1,0);
    //glScaled()
     seat();
     glPopMatrix();

     glPushMatrix();
      glTranslated(.75,-1,0);
      //glScaled()
       seat();
       glPopMatrix();

    glPushMatrix();
    glTranslated(-.75,1,0);
        //glScaled()
    seat();
    glPopMatrix();
    glPushMatrix();
    glTranslated(.75,1,0);
          //glScaled()
    seat();
    glPopMatrix();

    glPushMatrix();
    glColor3f(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
    glTranslated(1.5,.5,1.05);
    glRotated(90,1,0,0);
    glRotated(15,0,1,0);
    glScaled(.15,.15,.15);
    glCallList(objects[0]);
    glPopMatrix();

    glPushMatrix();
    glColor3f(.5,.5,.5);
    glBindTexture(GL_TEXTURE_2D,texture[2]);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,100);
    glTranslated(-1.4,-.4,1.05);
    glRotated(90,1,0,0);
    glRotated(280,0,1,0);
    glScaled(.15,.15,.15);
    glCallList(objects[0]);
    glPopMatrix();

   }
   ground();
   glColor3f(.5,.5,.5);
   walls();
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_LIGHTING);
   /*
   for (int i=1;i<num_landmarks;i++)
   {
     landmark(landmarks[i]);
   }
   */

   //draw landmarks that are to be drawn
   for (int i=0;i<10;i++)
   {
     //printf("about drawing landmarks for cam%d",i);
     if (view%3!=0){
     if(cameras[i].draw_landmarks==true)
     {
       //printf("drawing landmarks for cam%d",i);
       for (int lm=0; lm<10;lm++)
       {

         int index = cameras[i].visible_landmarks[lm];
         //printf("Index is %d\n",index);
         if(index>0)
         {
           if (cameras[i].is_selected) glColor4f(0,1,0,1);
           else glColor4f(1,1,0,1);
           landmark(landmarks[index]);
         }
       }
     }}

     if(cameras[i].show_new==true)
     {
       for (int lm=0; lm<10;lm++)
       {
         int index = cameras[i].visible_landmarks[lm];
         if (index>0)
         {
           double x = landmarks[index].x;
           double y = landmarks[index].y;
           double z = landmarks[index].z;
           double x1 = cameras[i].pose.x;
           double y1 = cameras[i].pose.y;
           double z1 = cameras[i].pose.z;
           //printf("%d %d %d %d %d %d\n",x,y,z,x1,y1,z1);
           line(x,y,z,x1,y1,z1,LANDMARK_CAMERA_1);
          }
       }
     }

       if(cameras[i].show_old==true)
       {
         for (int lm=0; lm<10;lm++)
         {
           int index = cameras[i].visible_landmarks[lm];
           if (index>0)
           {
             double x = landmarks[index].x;
             double y = landmarks[index].y;
             double z = landmarks[index].z;
             double x1 = cameras[i].pose.x;
             double y1 = cameras[i].pose.y;
             double z1 = cameras[i].pose.z;
             //printf("%d %d %d %d %d %d\n",x,y,z,x1,y1,z1);
             line(x,y,z,x1,y1,z1,LANDMARK_CAMERA_0);
            }
         }
       }
     }
     //draw camera points and lines
    if(step==4)
    {
        for(int lm0=0;lm0<10;lm0++){ //iterate through the old cam
          for(int lm1=0;lm1<10;lm1++){ //iterate through the new cam
            if(cameras[iteration].visible_landmarks[lm0] == cameras[iteration-1].visible_landmarks[lm1] && cameras[iteration].visible_landmarks[lm0]!=0)
            {
              double lmx = landmarks[cameras[iteration].visible_landmarks[lm0]].x;
              double lmy = landmarks[cameras[iteration].visible_landmarks[lm0]].y;
              double lmz = landmarks[cameras[iteration].visible_landmarks[lm0]].z;
              double c1x = cameras[iteration].pose.x;
              double c1y = cameras[iteration].pose.y;
              double c1z = cameras[iteration].pose.z;
              double c2x = cameras[iteration-1].pose.x;
              double c2y = cameras[iteration-1].pose.y;
              double c2z = cameras[iteration-1].pose.z;

              double d1 = pow(pow(lmx-c1x,2)+pow(lmy-c1y,2)+pow(lmz-c1z,2),0.5);
              double d2 = pow(pow(lmx-c2x,2)+pow(lmy-c2y,2)+pow(lmz-c2z,2),0.5);


              double lm_c1_x = c1x+(.7*((lmx-c1x)/d1));
              double lm_c1_y = c1y+(.7*((lmy-c1y)/d1));
              double lm_c1_z = c1z+(.7*((lmz-c1z)/d1));

              double lm_c2_x = c2x+(.7*((lmx-c2x)/d2));
              double lm_c2_y = c2y+(.7*((lmy-c2y)/d2));
              double lm_c2_z = c2z+(.7*((lmz-c2z)/d2));

              glColor4f(0,0,1,1);
              glPointSize(10);
              glBegin(GL_POINTS);
              glVertex3d(lm_c1_x,lm_c1_y,lm_c1_z);
              glVertex3d(lm_c2_x,lm_c2_y,lm_c2_z);
              glEnd();

              line(lm_c1_x,lm_c1_y,lm_c1_z,lm_c2_x,lm_c2_y,lm_c2_z,MATCH);


              break;
            }

          }
        }
    }

   //draw transform lines
   for(int i=1;i<10;i++)
   {
     if (camera_transforms[i]==true)
     {
       line(cameras[i-1].pose.x,cameras[i-1].pose.y,cameras[i-1].pose.z,
              cameras[i].pose.x,cameras[i].pose.y,cameras[i].pose.z, TRANSFORM);
     }
   }

   for(int i=0;i<10;i++)
   {
     if (cameras[i].visible==true) camera(cameras[i].pose);

   }


   //cube(0,0,0,1,1,1,0);



   //  Draw axes - no lighting from here on
   //glDisable(GL_LIGHTING);

   //  Display parameters
   glColor3f(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d FOV=%d Step=%d Interation=%d\n",
     theta_loc,fov,step+1,iteration+1);
  if (iteration==11) Print(" This causes all frames poses to be adjusted though Bundle Adjustment");
  else if (iteration==10) Print (" Frame 10 Detects the Same features that frame 1 did");
  else if (iteration>4) Print(" Demonstrating Loop Closure");
  else if (step==1) Print(" Add a camera frame");
  else if (step==2) Print(" Detect Features in Camera frame");
  else if (step==3) Print(" Project Features into the Environment");
  else if (step==4) Print(" Find features tracked by consecutive frames");
  else if (step==0 && iteration>1)Print(" Estimate a transform based on tracked features");

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}


void clearCameras()
{
 for (int i=0;i<5;i++)
 {
   cameras[i].is_selected = false;
 }
}
void setCameraView(int camId)
{
  eye_x = cameras[camId].pose.x;
  eye_y = cameras[camId].pose.y;
  eye_z = cameras[camId].pose.z;
  theta_loc = cameras[camId].pose.d +90;

  clearCameras();
  cameras[camId].is_selected=true;
}


/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
  //  Right arrow key - increase angle by 5 degrees
  if (key == GLUT_KEY_RIGHT)
  {
    theta_loc -= 5;
    clearCameras();
   }
  //  Left arrow key - decrease angle by 5 degrees
  else if (key == GLUT_KEY_LEFT)
  {
    theta_loc += 5;
    clearCameras();
   }
  //  Up arrow key - increase elevation by 5 degrees
  else if (key == GLUT_KEY_UP)
  {
      eye_x += .1*Cos(theta_loc);
      eye_y += .1*Sin(theta_loc);
      clearCameras();

   }
  //  Down arrow key - decrease elevation by 5 degrees
  else if (key == GLUT_KEY_DOWN)
  {

      eye_x -= .1*Cos(theta_loc);
      eye_y -= .1*Sin(theta_loc);
      clearCameras();
   }
  //  PageUp key - increase dim
  else if (key == GLUT_KEY_PAGE_UP)
  {   eye_z += 0.1;
     clearCameras();}
  //  PageDown key - decrease dim
  else if (key == GLUT_KEY_PAGE_DOWN)
  {   eye_z -= 0.1;
     clearCameras();}
  //  Keep angles to +/-360 degrees
  //  Update projection
  Project(fov,asp,dim);
  //  Tell GLUT it is necessary to redisplay the scene
  glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
    if (ch == 27) exit(0);
    else if (ch=='v') view++;
    else if (ch=='l') light = 1-light;
    else if (ch=='1') setCameraView(0);
    else if (ch=='2') setCameraView(1);
    else if (ch=='3') setCameraView(2);
    else if (ch=='4') setCameraView(3);
    else if (ch=='5') setCameraView(4);
    else if (ch=='6') setCameraView(5);
    else if (ch=='7') setCameraView(6);
    else if (ch=='8') setCameraView(7);
    else if (ch=='9') setCameraView(8);
    else if (ch=='0') setCameraView(9);
    else if (ch==' '){
      if (iteration<11) next_step();}

   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  Animate if requested
   //glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(mode?fov:0,asp,dim);
}

int main(int argc,char* argv[])
{
    for (int i=0;i<10;i++)
    {
      cameras[i].pose.x = demo_poses_array[i][0];
      cameras[i].pose.y = demo_poses_array[i][1];
      cameras[i].pose.z = demo_poses_array[i][2];
      cameras[i].pose.d = demo_poses_array[i][3];
      /*calc camera camera conrers
      for (int i=0;i<4;i++)
      {
        cameras[i].camera_corners[i].x = .5*camera_corners[i][0]*Cos(cameras[i].pose.d) - camera_corners[i][1]*Sin(cameras[i].pose.d) + cameras[i].pose.x;
        cameras[i].camera_corners[i].y = camera_corners[i][0]*Sin(cameras[i].pose.d) + .5*camera_corners[i][1]*Cos(cameras[i].pose.d) + cameras[i].pose.y;
        cameras[i].camera_corners[i].z = .5*camera_corners[i][2] + cameras[i].pose.z;
      }
      */
      cameras[i].visible=false;
      for (int k=0;k<10;k++)
      {
        cameras[i].visible_landmarks[k] = cam_landmark_array[i][k];
      }
      cameras[i].draw_landmarks=false;
      cameras[i].show_old=false;
      cameras[i].show_new=false;
      cameras[i].is_selected=false;
      cameras[i].show_camera_points=false;

    }

    camera_transforms[0]=true;

    for (int i=0;i<num_landmarks;i++)
    {
      landmarks[i].x = init_landmarks_array[i][0];
      landmarks[i].y = init_landmarks_array[i][1];
      landmarks[i].z = init_landmarks_array[i][2];
    }
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 1000x1000
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(1000,1000);
   glutCreateWindow("Jensen Dempsey: Project SLAM Demo");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);

   texture[0] = LoadTexBMP("wood.bmp");
   texture[1] = LoadTexBMP("cleanmetal.bmp");
   texture[2] = LoadTexBMP("metal.bmp");
   //texture[2] = LoadTexBMP("metal.bmp");


   objects[0] = LoadOBJ("armadillo.obj");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
