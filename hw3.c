/*
 *  Lighting
 *
 *  Demonstrates basic lighting using a sphere and a cube.
 *
 *  Key bindings:
 *  l          Toggles lighting
 *  a/A        Decrease/increase ambient light
 *  d/D        Decrease/increase diffuse light
 *  s/S        Decrease/increase specular light
 *  e/E        Decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  F1         Toggle smooth/flat shading
 *  F2         Toggle local viewer mode
 *  F3         Toggle light distance (1/5)
 *  F8         Change ball increment
 *  F9         Invert bottom normal
 *  m          Toggles light movement
 *  []         Lower/rise light
 *  p          Toggles ortogonal/perspective projection
 *  +/-        Change field of view of perspective
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx229.h"

int axes=1;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle       //  Field of view (for perspective)
int light=1;      //  Lighting
int theta_loc = 220;         //  angle for locomotion
int fov=90;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=5.0;   //  Size of world
double eye_x = 5;
double eye_y = 5;
double eye_z = 1.5;
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

unsigned int texture[4]; // Textures



static void sphere2(double x,double y,double z,double r)
{
   const int d=5;
   int th,ph;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
        glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
        glVertex3d(Sin(th)*Cos(ph+d) , Sin(ph+d) , Cos(th)*Cos(ph+d));
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}
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

    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    glEnd();
    //  Back
    glBegin(GL_QUADS);
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
    //  Undo transformations and textures
    glPopMatrix();
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


  glColor3f(.5,.35,.05);
  glBindTexture(GL_TEXTURE_2D,texture[2]);
  float brown[] = {.5,.35,.05,1};
  float black[] = {0,0,0,1};
  glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,0);
  glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,brown);
  glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
  cube(0,0,0,2,5,.1,0);
  glPushMatrix();
  glTranslated(0,0,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(2,.1);

  glPushMatrix();
  glTranslated(0,-1*space_between_poles,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(2,.1);

  glPushMatrix();
  glTranslated(0,space_between_poles,0);
  glRotated(90,0,0,1);
  glScaled(1,1,2.2);
  pole(2,.1);


}


 static void torus(double a, double c)
 {
   const int d=5;
   int theta,phi;
    //
    //glBegin(GL_QUAD_STRIP);
    /*
    int bands = 360/d;0
    double increment = 360/d;
    double red = 1;
    double green = 0;
    double blue = 0;
    */
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


void display()
{
   const double len=2.0;  //  Length of axes
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
  //double Ex = -2*dim*Sin(th)*Cos(ph);
  //double Ey = +2*dim        *Sin(ph);
  //double Ez = +2*dim*Cos(th)*Cos(ph);
  //gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);

  gluLookAt(eye_x,eye_y, eye_z , eye_x + Cos(theta_loc),eye_y + Sin(theta_loc),eye_z , 0,0,1);

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
        //  Translate intensity to color vectors
        float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
        float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
        float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
        //  Light position
        float Position[]  = {distance*Cos(zh),distance*Sin(zh),zlight,1.0};

        glColor3f(1,1,1);
        sphere2(Position[0],Position[1],Position[2] , 0.1);
        //  OpenGL should normalize normal vectors
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
   else
     glDisable(GL_LIGHTING);

   //  Draw scene
   /*
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   */
   glPushMatrix();
   //  Offset
   glTranslated(0,-4,.25);
   //glRotated(90,0,1,0);
   glScaled(1,1,.25);
   glColor3f(1,0,0);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,1);
   //glRotated(90,0,0,1);
   glScaled(.75,.75,.25);
   glColor3f(1,1,0);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,2);
   //glRotated(90,0,0,1);
   glScaled(.5,.5,.25);
   glColor3f(0,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   torus(.5,1);

   glPushMatrix();
   //  Offset
   glTranslated(0,-4,3);
   //glRotated(90,0,0,1);
   glScaled(.25,.25,.25);
   glColor3f(1,0,1);
   glBindTexture(GL_TEXTURE_2D,texture[2]);
   torus(.5,1);

   hanoi_stand(4);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }

   //  Display parameters
   glWindowPos2i(5,5);
   Print("Dim=%.1f FOV=%d Light=%s",
     dim,fov,light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,zlight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   }
  //  Left arrow key - decrease angle by 5 degrees
  else if (key == GLUT_KEY_LEFT)
  {
    theta_loc += 5;
   }
  //  Up arrow key - increase elevation by 5 degrees
  else if (key == GLUT_KEY_UP)
  {
      eye_x += .1*Cos(theta_loc);
      eye_y += .1*Sin(theta_loc);

   }
  //  Down arrow key - decrease elevation by 5 degrees
  else if (key == GLUT_KEY_DOWN)
  {

      eye_x -= .1*Cos(theta_loc);
      eye_y -= .1*Sin(theta_loc);

   }
  //  PageUp key - increase dim
  else if (key == GLUT_KEY_PAGE_UP)
     eye_z += 0.1;
  //  PageDown key - decrease dim
  else if (key == GLUT_KEY_PAGE_DOWN)
     eye_z -= 0.1;
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
   if (ch == 27)
      exit(0);
   //  Reset view angle+++
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      zlight -= 0.1;
   else if (ch==']')
      zlight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
    else if (ch=='1')
        distance +=1;
    else if (ch=='2' && distance>0)
      distance -=1;

   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
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

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(1000,1000);
   glutCreateWindow("Jensen Dempsey: Assignment 3");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   texture[3] = LoadTexBMP("block.bmp");
   texture[0] = LoadTexBMP("cleanmetal.bmp");
   texture[1] = LoadTexBMP("wood.bmp");
   texture[2] = LoadTexBMP("metal.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
