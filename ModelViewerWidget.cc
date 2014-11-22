#include "ModelViewerWidget.h"
#include <array>
#include <vector>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QMouseEvent>
#endif

#include <qgl.h>
#include "trackball.h"

static const char simple_frag[] =
#include "simple-frag.i"
;

static const char simple_vert[] =
#include "simple-vert.i"
;

using namespace std;

#define VIEW_SCALE_MAX 10.0
#define VIEW_SCALE_MIN 0.001

// a cube
float boxv[][3] = {
    { -0.5, -0.5, -0.5 }, // 0
    {  0.5, -0.5, -0.5 }, // 1 
    {  0.5,  0.5, -0.5 }, // 2
    { -0.5,  0.5, -0.5 }, // 3
    { -0.5, -0.5,  0.5 }, // 4 
    {  0.5, -0.5,  0.5 }, // 5 
    {  0.5,  0.5,  0.5 }, // 6 
    { -0.5,  0.5,  0.5 } // 7
};

QSize ModelViewerWidget::minimumSizeHint() const
{
  return QSize(300, 300);
}

QSize ModelViewerWidget::sizeHint() const
{
  return QSize(400, 400);
}

// Initialize opengl and build our static model.
void ModelViewerWidget::initializeGL()
{
  initializeGLFunctions();
  setShaders();

  buildModel();
  bindModel();

  // What is the difference between this and  qglClearColor(qtPurple.dark());
  glClearColor (0.5, 0.5, 0.5, 1.0);

  glEnable (GL_DEPTH_TEST);
  glEnable (GL_BLEND);
  glEnable(GL_MULTISAMPLE_ARB);
  glEnable(GL_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

  static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

// Draw the model to OpenGL.
void ModelViewerWidget::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();

  glScalef (this->view_scale, this->view_scale, this->view_scale); // 3

  float m[4][4];
  build_rotmatrix (m, this->view_quat);
  glMultMatrixf (&m[0][0]); 

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderModel();
    
  glPopMatrix();

}

void ModelViewerWidget::resizeGL(int width, int height)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double ar = 1.0*width / height; // aspect ratio
  glOrtho( -ar, ar, -1, 1, -100, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glViewport (0, 0,width,height);
}

// The mouse is delegated to the SGI trackball code to create a
// a new view.
void ModelViewerWidget::mousePressEvent(QMouseEvent *event)
{
  beginX = event->x();
  beginY = event->y();
}

void ModelViewerWidget::mouseMoveEvent(QMouseEvent *event)
{
  double w = width();
  double h = height();
  double x = event->x();
  double y = event->y();
  float d_quat[4];

  if (event->buttons() == Qt::LeftButton) {
    trackball (d_quat,
               (2.0 * beginX - w) / w,
               (h - 2.0 * beginY) / h,
               (2.0 * x - w) / w,
               (h - 2.0 * y) / h);
      
    add_quats (d_quat, view_quat, view_quat);
  
    beginX = x;
    beginY = y;
  }
  update();
}

void ModelViewerWidget::wheelEvent(QWheelEvent *event)
{
  if (event->delta() < 0)
      this->view_scale /= 1.2;
  else
      this->view_scale *= 1.2;

  if (this->view_scale > VIEW_SCALE_MAX)
      this->view_scale = VIEW_SCALE_MAX;
  else if (this->view_scale < VIEW_SCALE_MIN)
      this->view_scale = VIEW_SCALE_MIN;

  update();
}

// Create the static shaders.
struct StringHelper {
    const char *p;
    StringHelper(const char * s) : p(s) {}
    operator const char**() { return &p; }
};

void ModelViewerWidget::setShaders()
{
  GLuint v,f,p;

  v = glCreateShader(GL_VERTEX_SHADER);
  f = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(v, 1, StringHelper(simple_vert), NULL);
  glShaderSource(f, 1, StringHelper(simple_frag), NULL);

  glCompileShader(v);
  glCompileShader(f);

  p = glCreateProgram();
  glAttachShader(p,f);
  glAttachShader(p,v);

  glLinkProgram(p);
  glUseProgram(p);
}

// Build a model and bind it to opengl. This may be reading a model from
// an STL file etc.
void ModelViewerWidget::buildModel()
{
    // Build an arbitrary model
    int n = 11;
    float s = 1.0/n;
    for (int i=0; i<n; i++)
      for (int j=0; j<n; j++)
        for (int k=0; k<n; k++) {
            Vec3 color (1,0,0);
            // add some color
            if ((i==0 && j==0)
                || (i==0 && j==n-1)
                || (i==n-1 && j==0)
                || (i==n-1 && j==n-1)
                || (i==0 && k==0)
                || (i==0 && k==n-1)
                || (i==n-1 && k==0)
                || (i==n-1 && k==n-1)
                || (j==0 && k==0)
                || (j==0 && k==n-1)
                || (j==n-1 && k==0)
                || (j==n-1 && k==n-1))
              color = Vec3(0,1,0);

            AddIcosahedron(Vec3(s*(i-0.5*n),s*(j-0.5*n),s*(k-0.5*n)),
                           s*0.5, color);
        }
    printf("Num Vertices = %d\n", vertices.size());
}

// Bind the model in the vertices array to opengl
void ModelViewerWidget::bindModel()
{
    // Create a temporary faces array. 
    vector<GLuint> faces(vertices.size());
    for (size_t i=0; i<faces.size(); i++)
      faces[i] = i;
  
    glGenBuffers(1, &geometry_array);
    glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*vertices.size(),
                 NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*vertices.size(),
                    &vertices[0].v.x);
    
    glGenBuffers(1, &indice_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(int)*3*faces.size(), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                    sizeof(int)*3*faces.size(), &faces[0]);

}

// Render the model in opengl
void ModelViewerWidget::renderModel()
{
    glBindBuffer(GL_ARRAY_BUFFER, geometry_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_array);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), NULL);
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (float*)(sizeof(GLfloat)*3));
    glColorPointer(3, GL_FLOAT, sizeof(Vertex), (float*)(sizeof(GLfloat)*6));

    glDrawElements(GL_TRIANGLES, vertices.size(), GL_UNSIGNED_INT, NULL);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void ModelViewerWidget::AddIcosahedron(Vec3 pos,
                                       float scale,
                                       Vec3 color)
{
  const GLuint ff[][3] { {0,1,2}, {0,2,3}, {0,3,4}, {0,4,5},
        {0,5,1}, {11,7,6}, {11,8,7}, {11,9,8}, {11,10,9}, {11,6,10},
        {1,6,2}, {2,7,3}, {3,8,4}, {4,9,5}, {5,10,1}, {6,7,2},
        {7,8,3}, {8,9,4}, {9,10,5}, {10,6,1} };
  int num_faces = 20;
  
  const GLfloat vv[][3] {
        { 0.000,  0.000,  1.000 },
        { 0.894,  0.000,  0.447 },
        { 0.276,  0.851,  0.447 },
        {-0.724,  0.526,  0.447 },
        {-0.724, -0.526,  0.447 },
        { 0.276, -0.851,  0.447 },
        { 0.724,  0.526, -0.447 },
        {-0.276,  0.851, -0.447 },
        {-0.894,  0.000, -0.447 },
        {-0.276, -0.851, -0.447 },
        { 0.724, -0.526, -0.447 },
        { 0.000,  0.000, -1.000 } };

  for (int i=0; i<num_faces; i++)
  {
      for (int j=0; j<3; j++) {
          const GLfloat *pp = vv[ff[i][j]];
          Vec3 v {pp[0]*scale+pos.x,pp[1]*scale+pos.y,pp[2]*scale+pos.z};
          Vec3 n {pp[0],pp[1],pp[2]};
          vertices.emplace_back(v, n, color);
      }
  }
}
