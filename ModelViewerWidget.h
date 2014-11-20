#ifndef FERNWIDGET_H
#define FERNWIDGET_H

#include <QGLWidget>
#include <QGLFunctions>

class ModelViewerWidget : public QGLWidget, protected QGLFunctions
{
  Q_OBJECT

public:
  ModelViewerWidget(QWidget */*parent*/ = nullptr) {}

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

  struct Vec3 {
        Vec3(GLfloat _x, GLfloat _y, GLfloat _z)
        : x(_x),y(_y),z(_z) {};
        
        GLfloat x,y,z;
  };

  struct Vertex {
      Vertex(Vec3 _v, Vec3 _n, Vec3 _c)
          : v(_v), n(_n), c(_c)
      {}

      Vec3 v,n,c;
  };


protected:
  void initializeGL();
  void paintGL();
  void resizeGL(int width, int height);
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void wheelEvent(QWheelEvent *event);

private:
    void setShaders();
    void buildModel();
    void bindModel();
    void renderModel();
    void AddIcosahedron(Vec3 pos,
                        float scale,
                        Vec3 color);

    // Initial quat view
    float view_quat[4] {0.552334, 0.122235, 0.160835, 0.808776};
    double beginX = -1;
    double beginY = -1;
    double view_scale = 1.0;
    GLuint geometry_array = 0;
    GLuint indice_array = 0;
    std::vector<Vertex> vertices;
};

#endif /* FERNWIDGET */
