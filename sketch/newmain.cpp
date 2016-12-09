/*
 * PRANJAL SAHU
 *
 *
 *
 * Refer
 * 1. http://felix.abecassis.me/2011/09/opencv-morphological-skeleton/
 * 2. https://web.archive.org/web/20160322113207/http://opencv-code.com/quick-tips/implementation-of-thinning-algorithm-in-opencv/
 * 4. Finding faces in a planar embedding of a graph
 * 5. Polygon tesselation http://www.songho.ca/opengl/gl_tessellation.html
 * 6. http://www-h.eng.cam.ac.uk/help/tpl/graphics/using_glui.html
 * */

#ifdef WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <iostream>
#include <fstream>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <GL/glui.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtc/constants.hpp>

#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


#ifndef NEWMAIN_H
#define NEWMAIN_H

#define GL_GLEXT_PROTOTYPES
#include <queue>
#include <list>
#include <vector>
#include <fstream>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <list>
#include <math.h>
#include "myline.h"
#include "polygon.h"
#include "plane.h"
#include "cost.h"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "myutilities.h"

using namespace cv;
using namespace std;


int godisp = 1;
// for right click menu
void createmymenua(void);
void menu(int value);
void disp(void);

int merge_iteration =0;

int sa_width  = 600;
int sa_height = 600;

// Scale of the render
// make it configurable in GUI
int render_scale = 60;
float img_scale = 3.0;


//flags to be used by keyboard for three different operations
int count     = 0;
int entertext = 0;
int drawline  = 0;
int cropimage = 0;
int drawpoly  = -1;		// get points of polygon
int donepoly  = 0;		// stop taking points for polygon
int polytype  = 0;		// 1 means y polygon and 2 means star polygon

char etext[100];

Mat imga, bw, img, imgc;
Mat dst_norm_scaled;
Mat m, m2, m3;

std::vector<glm::vec2> all_lines_vec;
std::vector<myline*> all_mylines;

// 2d hash for storing lines at each pixel
std::map<i2tuple, myline*> all_lines;
std::map<i2tuple, int> map_slopes;
std::vector<myline*> all_lines_to_merge;
std::vector<myline*> all_lines_created;
vector<i2tuple> points_vector;

vector<i2tuple> original_corner_points; // this is required for point manipulation
vector<i2tuple> corner_points;      // stores the corner points obtained
vector<int> corner_points_colors;   // store the color of corner points to plot
vector<int> num_of_lines;           // number of line occuring at ith corner point
std::map<std::tuple<int, int>, int> map_num_of_lines;
std::vector<myline*> valid_lines_undirected;  // all the valid lines finally obtained
std::vector<myline*> valid_lines_directed;   // all the valid lines finally obtained

GLUtesselator* tess;

#define PI 3.14159265

int minpx = 100000;					// for polygon bounding box
int minpy = 100000;
int maxpx = -1;
int maxpy = -1;


// for storing how much the vectorized image has moved
// this is needed to manipulate points efficiently
int trans_x = 0;
int trans_y = 0;

int win1;
int win2 = -1;

#define MAXSIZE 1000
unsigned int w, h, d;
FILE *fp;

int px[10], py[10];
int click = 0, drawme = 0, dragging  =0;
unsigned char *image2;
unsigned char myimage[MAXSIZE][MAXSIZE][3];
unsigned char original[MAXSIZE][MAXSIZE][3];
unsigned char formaskpoly[MAXSIZE][MAXSIZE][3];			// for storing the inside pixels of a polygon

int newwidth, newheight;

int polysize;						// for storing the size of the polygon
std::queue<int> p1;					// for intersection points x coordinate

// different display types for labelled lines, polygons, and projected polygon
// start with only plotting lines
int  display_type = 4;

bool polygon_done = false;  // polygon extraction done or not
bool work_3d_done = false;  // if it is true then don't do the 3d construction work again
bool huffman_done = false;  // huffman labels work done
bool draw_axis    = false;  // start drawing axis
int point_selected = -1;


// plane to project for demo
plane* plane_to_project;

float angle_p = 0;
float tr_x = 0;
float tr_y = 0;
float tr_z = 0;

// all the polygons present in the given image
std::vector<polygon*> all_polygons;

// starts with one polygon
// next all the polygons which are adjacent are inserted in it
std::vector<polygon*> polygons_to_place;

//bool isleft(mypointa *a, mypointa *b, mypointa *c){
//    return ((b->x - a->x)*(c->y - a->y) - (b->y - a->y)*(c->x - a->x)) > 0;
//}


// glut look at z position
int zglut = 100;

std::vector<glm::vec2> axis_2d_points;

cost* cost_obj;

float xview = 0, yview = 0, zview = 0;

// Camera matrix
glm::mat4 View = glm::lookAt(
                             glm::vec3(-5,5,5), // Camera is at (4,3,3), in World Space
                             glm::vec3(0,0,0), // and looks at the origin
                             glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                             );


glm::mat4 ViewI = glm::inverse(View);




//Add positioned light
GLfloat lightColors[][4] = {{1.f, 1.f, 1.f, 1.0f}, {0.8f, 0.8f, 0.8f, 1.0f}};
//GLfloat lightPos[][4] = {{-5.0f, 5.0f, 5.0f, 1.0f}, {5.0f, 5.f, 5.f, 0.0f}};   // Positioned at (4, 0, 8)
GLfloat lightPos[][4] = {{-5.0f, 5.0f, -2.0f, 1.0f}, {5.0f, 5.f, 0.5, 0.0f}};



// points in the camera plane
// get the world space coordinates for these
// then project these coordinates onto hinging plane to get the best plane and the projected polygon will be the final
std::vector<glm::vec4> points_in_camera;


std::vector<glm::vec3> points_to_render_vec_global;


// sequence of polygon which are going to be placed
int poly_seq[3];
int current_polygon = 0;



// vector for storing already placed points
// will be used to check if the current polygon already has one of its three vertices placed in 3d
// if yes then get the normal direction from those 3 points and project onto that
// and place that polygon as well
std::vector<glm::vec2> corres_2d;
std::vector<glm::vec3> corres_3d;



// vector for storing the already inserted edges in 2d and 3d
// this will store only the indices of the vertex in corres_2d and corred_3d
// to get vertex value just index the vector corres_2d and corres_3d
// This will be used for solving the parallelism and collinearity constraints
std::vector<pair<int, int>> edges_list;



GLUI *glui_subwin;



void thinningIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1);
    
    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);
            
            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
            (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
            (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
            (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);
            
            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }
    
    im &= ~marker;
}

void thinning(cv::Mat& im)
{
    im /= 255;
    
    Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::Mat diff;
    
    do {
        thinningIteration(im, 0);
        thinningIteration(im, 1);
        cv::absdiff(im, prev, diff);
        im.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);
    
    im *= 255;
}

void get_transpose(Mat& im, Mat& im2){
    for (int i = 1; i < im.rows-1; i++)
    {
        for (int j = 1; j < im.cols-1; j++)
        {
            im2.at<uchar>(i, j) = im.at<uchar>(j, i);
        }
    }
}

void get_positive_y(Mat &im){
    Mat temp = cv::Mat::zeros(im.size(), CV_8UC1);
    
    for(int i=1;i < im.rows-1;++i){
        for(int j=1;j< im.cols-1;++j){
            temp.at<uchar>(i, im.cols-j) = im.at<uchar>(i, j);
        }
    }
    
    for(int i=1;i < im.rows-1;++i){
        for(int j=1;j< im.cols-1;++j){
            im.at<uchar>(i, j) = temp.at<uchar>(i, j);
        }
    }
    
    return;
}


int get_max_slope(int x, int y, std::vector<i2tuple> points_vector, Mat& im){
    int max_slope = 0;
    int max_count  = -1;
    
    for(int i=0; i<=180; i = i+SLOPE_ITER_DIFF){
        int theta = i;
        float m = tan(theta);
        
        // get a sweeping line
        myline *a = new myline(x, y, m);
        
        // instead of sloving for all intersection we will try only for a line segment
        // and therefore look for in neighbour hood of 10 pixels
        // and therefore only 400 pixel entries to check
        int count = 0;
        int startx = mymax(0, x-10);
        int endx   = mymin(im.rows, x+10);
        int starty = mymax(0, y-10);
        int endy   = mymin(im.cols, y+10);
        
        for(int i=startx;i<endx;++i){
            for(int j=starty;j<endy;++j){
                if(im.at<uchar>(i, j) > 120 && a->checkpointlies(i, j) == 1){
                    count = count +1;
                }
            }
        }
        
        delete(a);
        
        if(count >= max_count){
            max_count = count;
            max_slope = theta;
        }
    }
    //printf("%d, %d\n", x, y);
    return max_slope;
}

int get_max_slopea(int x, int y, std::vector<i2tuple> points_vector){
    int max_slope = 0;
    int max_count = -1;
    
    for(int i=0; i<=180; i = i+SLOPE_ITER_DIFF){
        int theta = i;
        float m = tan(theta);
        
        // get a sweeping line
        myline *a = new myline(x, y, m);
        
        int count = 0;
        for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it){
            i2tuple pt = *it;
            if ( a->checkpointlies(std::get<0>(pt), std::get<1>(pt)) == 1){
                count = count+1;
            }
        }
        
        delete(a);
        
        if(count >= max_count){
            max_count = count;
            max_slope = theta;
        }
    }
    printf("%d, %d\n", x, y);
    return max_slope;
}

// fills the points_vector
void fill_points_vector(Mat& im){
    // get all the non zero pixels
    if(points_vector.size() == 0){
        for(int i=1; i<im.rows; ++i){
            for(int j=1;j<im.cols;++j){
                if(im.at<uchar>(i, j) > 120){
                    points_vector.push_back(i2tuple(i, j));
                }
            }
        }
    }
    
    return;
}

// pass the matrix and perform the sweepline algorithm
std::map<i2tuple, int> sweepline(Mat& im){
    
    fill_points_vector(im);
    
    // stores the slopes for each pixel
    std::map<i2tuple, int> all_line_slopes;
    
    // for each positive pixel
    for(std::vector<i2tuple>::iterator it = points_vector.begin(); it != points_vector.end(); ++it) {
        i2tuple pt = *it;
        all_line_slopes[pt] = get_max_slope(get<0>(pt), get<1>(pt), points_vector, im);
    }
    
    return all_line_slopes;
}

float mytan(int degree){
    return tan(degree * PI/180.0);
}


void updatea(int value) {
    glutPostRedisplay();
    glutTimerFunc(25, updatea, 0);
}

void plot_points(vector<i2tuple> points_to_plot){
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };
    
    for(std::vector<i2tuple>::iterator iterator = points_to_plot.begin(); iterator != points_to_plot.end(); iterator++) {
        i2tuple p = *iterator;
        
        glBegin( GL_POINTS );
        glColor3f(0.0f, 1.0f, 0.0f);
        GLfloat px = (get<0>(p)-100)*img_scale/sa_width;
        GLfloat py = (get<1>(p)-100)*img_scale/sa_height;
        glVertex2f(px, py);
        glEnd();
    }
    
    return;
}

// corrects the coordinate such that x denotes x in both display and array
// similary y denotes both y in display and array
// helps in understanding
void get_correct_coord(std::vector<myline*> original_lines){
    for(std::vector<myline*>::iterator iterator = original_lines.begin(); iterator != original_lines.end(); iterator++) {
        myline * linet = *iterator;
        int tp = linet->x1;
        linet->x1 = linet->y1;
        linet->y1 = tp;
        tp = linet->x2;
        linet->x2 = linet->y2;
        linet->y2 = tp;
        
        linet->y1 = sa_height-linet->y1;
        linet->y2 = sa_height-linet->y2;
    }
    
    return;
}


// corrects the coordinate such that x denotes x in both display and array
// similary y denotes both y in display and array
// helps in understanding
// also it subtracts the minimum y coordinate from each point
// therefore the bottom point is our origin

std::vector<i2tuple> get_correct_coord_point_and_line(std::vector<i2tuple> original_points, std::vector<myline*> valid_lines_undirected){
    std::vector<i2tuple> temp;
    
    int minx = 100000;
    int miny  = 100000;
    
    for(int i=0;i<original_points.size();++i){
        i2tuple mp = original_points[i];
        int x = get<1>(mp);
        int y = sa_height-get<0>(mp);
        temp.push_back(i2tuple(x, y));
        if(miny > y){
            miny  = y;
            minx = x;
        }
        
    }
    
    // shift the points so that the bottom most point is origin
    for(int i=0;i<temp.size();++i){
        i2tuple mp = temp[i];
        temp[i] = i2tuple(get<0>(mp)-minx, get<1>(mp)-miny);
        original_corner_points[i] = temp[i];
    }
    
    get_correct_coord(valid_lines_undirected);
    for(int i=0;i< valid_lines_undirected.size();++i){
        valid_lines_undirected[i]->x1 = valid_lines_undirected[i]->x1-minx;
        valid_lines_undirected[i]->x2 = valid_lines_undirected[i]->x2-minx;
        valid_lines_undirected[i]->y1 = valid_lines_undirected[i]->y1-miny;
        valid_lines_undirected[i]->y2 = valid_lines_undirected[i]->y2-miny;
    }
    
    //printf("MINX IS %d and MINY IS %d\n", minx, miny);
    return temp;
}



void plot_line_3d(std::vector<mypoint*> all_points, int color){
    glBegin(GL_LINE_LOOP);
    glLineWidth(15);
    glColor3f(0.0f, 1.0f, 0.0f);
    for(std::vector<mypoint*>::iterator iterator = all_points.begin(); iterator != all_points.end(); iterator++) {
        mypoint *m = *iterator;
        glVertex3f(m->x, m->y, m->z);
    }
    glEnd();
    
    return;
}

// plots the corner points
void plot_corner_points(){
    float pointsize = 10.0;
    
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f },
        {1.0f, 0.0f, 0.0f }, {1.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f } };
    
    for(int i=0;i<corner_points.size();++i){
        int x = std::get<0>(corner_points[i]);
        int y = std::get<1>(corner_points[i]);
        if(point_selected == i){
            glPointSize(pointsize+7);
        }
        else{
            glPointSize(pointsize);
        }
        glBegin(GL_POINTS);
        glColor3f(colors[corner_points_colors[i]][0], colors[corner_points_colors[i]][1], colors[corner_points_colors[i]][2]);
        glVertex2f(x*img_scale/sa_width, y*img_scale/sa_height);
        //printf("%f, %f\n", x*img_scale/sa_width, y*img_scale/sa_height);
        glEnd();
    }

//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(1, 1);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(1.5, 1.5);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(2.5, 2.5);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(3, 3);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(0.5, 0.5);
//    glEnd();
//    
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(0, 0);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(-3, 3);
//    glEnd();
//    
//    glPointSize(pointsize);
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(-1.5-1, -1);
//    glEnd();
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(-1.5-2.5, -2.5);
//    glEnd();
//    glBegin(GL_POINTS);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(-1.5-3.0, -3.0);
//    glEnd();
//    glPointSize(pointsize);
//    glColor3f(colors[0][0], colors[0][1], colors[0][2]);
//    glVertex2f(0.75, 0.5);
//    glEnd();
}


void plot_line(std::vector<mypoint*> all_points, int color){
    glBegin(GL_LINE_LOOP);
    glLineWidth(5);
    for(std::vector<mypoint*>::iterator iterator = all_points.begin(); iterator != all_points.end(); iterator++) {
        mypoint *m = *iterator;
        GLfloat px = (m->x)*img_scale/sa_width;
        GLfloat py = (m->y)*img_scale/sa_height;
        glVertex2f(px, py);
    }
    glEnd();
    
    return;
}

void plot_line(myline *linet, int color){
    glBegin(GL_LINE_LOOP);
    glLineWidth(5);
    GLfloat colors[][3] = { { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f }, {0.0f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f } };

    int r = color;
    glColor3f(colors[r][0], colors[r][1], colors[r][2]);
    
    GLfloat px = (linet->x1+trans_x)*img_scale/sa_width;
    GLfloat py = (linet->y1+trans_y)*img_scale/sa_height;
    
    GLfloat qx = (linet->x2+trans_x)*img_scale/sa_width;
    GLfloat qy = (linet->y2+trans_y)*img_scale/sa_height;
    
    glVertex2f(px, py);
    glVertex2f(qx, qy);
    
    glEnd();
}

void plot_lines(std::vector<myline*> lines_to_plot, std::vector<int> color){
    GLfloat colors[][3] = { { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f }, {0.0f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } ,{1.0f, 1.0f, 0.0f } };

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque

    
    int index = 0;
    for(std::vector<myline*>::iterator iterator = lines_to_plot.begin(); iterator != lines_to_plot.end(); iterator++) {
        myline * linet = *iterator;
        plot_line(linet, color[index]);
        index = index+1;
    }
    
    return;
}

void plot_lines(std::vector<myline*> lines_to_plot, int color){
    std::vector<int> colors;
    for(int i=0;i<lines_to_plot.size();++i){
        colors.push_back(color);
    }
    plot_lines(lines_to_plot, colors);
    return;
}


void prepare_points_to_project(polygon *pt){
    
    // always clear points before pushing new one
    points_in_camera.clear();
    
    // points in the camera reference
    std::vector<mypoint*>polygon_points  = pt->get_points();
    for(int i=0;i<polygon_points.size();++i){
        points_in_camera.push_back(glm::vec4(polygon_points[i]->x, polygon_points[i]->y, 0, 1));
    }
    
    // get world coordinates for these points
    for(int i=0;i<points_in_camera.size();++i){
        points_in_camera[i] = ViewI*points_in_camera[i];
    }
}



// checks if for a polygon two of its adjaent polygon are placed or not
bool check_2_polygons_already_placed(polygon *pt){
    std::vector<glm::vec2> twod_temp = pt->get_points_vec();
    int count = 0;
    
    std::vector<polygon*> adjacent_polygons = pt->get_adjacent_polygons_using_huffman(all_polygons, corres_2d, corres_3d, false);
    for(int i=0;i<adjacent_polygons.size();++i){
        if(adjacent_polygons[i]->placed){
            count = count+1;
            if(count >=2 ){
                printf("testing bug");
                return true;
            }
        }
    }
    return false;
}

// if 2 polygons are already placed then it searches for the 3rd point
// first two are the rotation axis and the third one is searched such that it does not
// appears in the same polygon
// a plane is formed using these points and returned
plane* get_plane(polygon *pt){
    std::vector<glm::vec2> twod_temp = pt->get_points_vec();
    std::vector<glm::vec3> plane_points;
    std::vector<glm::vec2> plane_points_2d;
    
    int count = 0;
    
    std::vector<glm::vec2> twod_tempa = pt->rotation_axis_2d_points;

    for(int j=0;j<twod_tempa.size(); ++j){
        for(int i=0;i<corres_2d.size(); ++i){
            if(twod_tempa[j][0] == corres_2d[i][0] && twod_tempa[j][1] == corres_2d[i][1]){
                ++count;
                plane_points.push_back(corres_3d[i]);
                plane_points_2d.push_back(corres_2d[i]);
            }
        }
    }
    
    // get all the points of adjacent polygon
    twod_tempa = pt->adjacent_polygon->get_points_vec();
    
    // check for the third point
    // it should not come from the polygon from which axis of rotation was defined
    // therefore iterate over all 2d points and check if it is lying in the adjacent polygon is yes then reject else accept
    
    for(int ik=0;ik < corres_2d.size(); ++ik){
        bool flag = true;
        for(int j=0; j< twod_tempa.size();++j){
            if(corres_2d[ik][0] == twod_tempa[j][0] && corres_2d[ik][1] == twod_tempa[j][1]){
                flag = false;
                break;
            }
        }
        
        if(flag){
            // check if the point belongs to the polygon or not
            for(int j=0;j<twod_temp.size();++j){
                // form the plane using these 3 points and return
                if(twod_temp[j][0] == corres_2d[ik][0] && twod_temp[j][1] == corres_2d[ik][1]){
                    plane_points_2d.push_back(corres_2d[ik]);
                    plane_points.push_back(corres_3d[ik]);
                    return new plane(plane_points);
                }
            }
        }
    }
        
    return new plane(plane_points);
}



// maintains the list of points which are alrady rendered
// always check by 2d points since 3d are in float
void insert_corres(std::vector<glm::vec3> threed, std::vector<glm::vec2> twod){
    for(int i=0;i<twod.size();++i){
        bool flag =  true;
        for(int j=0;j<corres_2d.size();++j){
            if(corres_2d[j][0] == twod[i][0] && corres_2d[j][1] == twod[i][1]){
                flag = false;
            }
        }
        
        
        // if not present already then push it in vector
        if(flag){
            corres_2d.push_back(glm::vec2(twod[i]));
            corres_3d.push_back(glm::vec3(threed[i]));
        }
    }
    
    return;
}


bool check_2d_equal(glm::vec2 a, glm::vec2 b){
    if(a[0] == b[0] && a[1] == b[1])
        return true;
    else
        return false;
}

// get index in corres_2d for a vertex
int get_vertex_index(glm::vec2 v){
    for(int i=0;i<corres_2d.size(); ++i){
        if(corres_2d[i][0] == v[0] && corres_2d[i][1] == v[1])
            return i;
    }
    
    // not found
    // ERROR case
    printf("SOME ERROR OCCURED IN get_vertex_index");
    return -1;
}


// maintains the list of edges which are already rendered
// always check by 2d points since 3d are in float
// should be called only after insert_corres has been inserted
void insert_edges(std::vector<glm::vec3> threed, std::vector<glm::vec2> twod){
    for(int i=0;i<twod.size();++i){
        
        bool flag =  true;
        
        glm::vec2 bf, bs;
        
        // get first and second vertex of edge
        // for last value 2nd vertex is the first one in list
        if (i == twod.size()-1){
            bf = twod[i];
            bs = twod[0];
        }else{
            bf = twod[i];
            bs = twod[i+1];
        }
        
        
        // check in two d points list if it is already placed
        for(int j=0; j< edges_list.size(); ++j){
            
            glm::vec2 af = corres_2d[edges_list[j].first];
            glm::vec2 as = corres_2d[edges_list[j].second];
            
            if((check_2d_equal(af, bf) && check_2d_equal(as, bs)) || (check_2d_equal(af, bs) && check_2d_equal(bf, as))){
                flag = false;
            }
        }
        
        
        // if not present already then push it in vector
        if(flag){
            edges_list.push_back(make_pair(get_vertex_index(bf), get_vertex_index(bs)));
        }
    }
    
    return;
}



// returns the next polygon to be placed in 3d
polygon * get_next_polygon_to_render(std::vector<polygon*> polygons_to_place){
    // getting the polygon having the lowest cost
    
    float mincost = 10000;
    polygon *mincost_polygon = NULL;
    
    for(std::vector<polygon*>::iterator it = polygons_to_place.begin(); it != polygons_to_place.end(); ++it){
        polygon *pp = *it;
        if(!pp->placed){
            std::pair<std::vector<glm::vec3>, std::pair<float, float>> return_pair = pp->get_min_cost_angle_points(corres_2d, corres_3d,edges_list,plane_to_project, cost_obj, render_scale);
            if(return_pair.second.first < mincost){
                mincost         = return_pair.second.first;
                mincost_polygon = pp;
            }
        }
    }
    
    printf("mincost polygon\n");
    return mincost_polygon;
}

// inserts the polygon list all_polygons_to_add into the polygons_to_place
void insert_into_polygons_to_render_list(std::vector<polygon*> adjacent_polygons){
    
    //printf("TESTING BUG 7 start\n");
    for(std::vector<polygon*>::iterator it = adjacent_polygons.begin(); it != adjacent_polygons.end(); ++it){
        polygon *pp = *it;
        // insert if it is already not present in the list
        if(!pp->is_part_of(polygons_to_place)){
            //printf("TESTIG BUG 11 start\n");
            polygons_to_place.push_back(pp);
            //printf("TESTIG BUG 11 end\n");
        }
    }
    //printf("TESTING BUG 7 end\n");
    return;
}

void TessError(GLenum err)
{
    fprintf(stderr,"Tessellation Error: %s\n",gluErrorString(err));
    exit(1);
}

void TessCombine(double coords[3],double* data[4],float weight[4],double** result)
{
    printf(">>>>> INSIDE TESS COMBINE CHECK IT <<<<< \n");
    *result = (double*) malloc(3 * sizeof(double));
    (*result)[0] = coords[0];
    (*result)[1] = coords[1];
    (*result)[2] = coords[2];
}

// use the normal of the polygon for all the tesselated smaller polygons
void myglBegin(GLenum type, void *polygon_data){
    glBegin(type);
    double *ptr = (double*) polygon_data;
    double a = *ptr;
    double b = *(ptr+1);
    double c = *(ptr+2);
    glNormal3f(a, b, c);
    glColor3f(0.8f, 0.8f, 0.8f);
    //glCullFace(GL_FRONT);
    //glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 128);
}

void TesselationClass(double points3d[][3], int size, plane *p)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    gluTessProperty(tess,GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
    gluTessCallback(tess, GLU_TESS_BEGIN_DATA, (void (CALLBACK *)())myglBegin);
    gluTessCallback(tess,GLU_TESS_END    ,glEnd);
    gluTessCallback(tess,GLU_TESS_VERTEX , (void (CALLBACK *)())glVertex3dv);
    gluTessCallback(tess,GLU_TESS_COMBINE,(void (CALLBACK *)())TessCombine);
    gluTessCallback(tess,GLU_TESS_ERROR  ,(void (CALLBACK *)())TessError);
    
    gluTessBeginPolygon(tess, p->normal_array);
    gluTessBeginContour(tess);
    
    //printf("===============================\n");
    for(int k=0;k<size;++k){
        gluTessVertex(tess, points3d[k], points3d[k]);
    }
    //printf("===============================\n");
    gluTessEndContour(tess);
    gluTessEndPolygon(tess);
    //gluDeleteTess(tess);
}




void displayone() {
    
    
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 1.0f, 0.0f }, {1.0f, 0.0f, 0.0f } };
    

    
    //glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glLoadIdentity();                 // Reset the model-view matrix
    
    

    if(display_type == 3){
        glTranslatef(1.5f, 0.0f, -7.0f);
    }else{
        glTranslatef(0, 0.0f, -7.0f);
    }
    
    
    if(display_type == 4){            // show original lines without calculating huffman labels
        //glutWireCube(2);
        plot_lines(valid_lines_undirected, 0);
        plot_corner_points();
    }
    else if(display_type == 0){       // calculate huffman labels and show lines
        std::vector<int> line_colors = get_line_labels(valid_lines_undirected);
        plot_lines(valid_lines_undirected, line_colors);
        plot_corner_points();
    }
    else if(display_type == 1){
        int index  = 0;
        for(std::vector<polygon*>::iterator iter = all_polygons.begin(); iter != all_polygons.end(); iter++){
            //polygon *pl = all_polygons[6];
            polygon* pl = *iter;
            plot_lines(pl->lines, index%4);
            ++index;
        }
    }
    else if(display_type == 2){
        polygon* p = all_polygons[4];
        plot_lines(p->lines, 0);
    }
    else if(display_type == 3){
        
        
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        
        // Camera position angle
        gluLookAt(-5+xview, 5+yview, 5+zview, 0, 0, 0, 0, 1, 0);
        
        //Add ambient light
        GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.2, 0.2, 0.2)
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
        
        // Lighting from different position
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColors[0]);
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos[0]);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColors[1]);
        glLightfv(GL_LIGHT1, GL_POSITION, lightPos[1]);
        
        
        for(int i =0;i<all_polygons.size();++i){
            if(all_polygons[i]->placed){
                glBegin(GL_LINE_LOOP);
                glLineWidth(105);
                glColor3f(0.0f, 0.0f, 0.0f);
                for(int j=0;j<all_polygons[i]->points_to_render_vec.size();++j){
                    glVertex3f(all_polygons[i]->points_to_render_vec[j][0]/render_scale, all_polygons[i]->points_to_render_vec[j][1]/render_scale, all_polygons[i]->points_to_render_vec[j][2]/render_scale);
                }
                glEnd();
                
                TesselationClass(all_polygons[i]->vertices, all_polygons[i]->points_to_render_vec.size(), all_polygons[i]->plane_to_project);
            }
        }
        
       
        int cpl = 0;
        for(int kl = 0;kl<all_polygons.size();++kl){
            if(all_polygons[kl]->placed)
                cpl = cpl+1;
        }
        
        //printf("COUNT OF ALREADY PLACED POLYGONS %d\n", cpl);
//        glPushMatrix();
//        glTranslatef(tr_x, tr_y, tr_z);
//        drawmycuboid(1, 2, 5);
//        glPopMatrix();
    }
    
    glutSwapBuffers();
}



class sorter_check {
    int test;
public:
    sorter_check(int type) : test(type) {}
    bool operator()(const int o1, const int o2) const {
        return abs(o1-test) < abs(o2-test);
    }
};


struct sortbyxasc {
    bool operator()(const myline* o1, const myline* o2) const {
        //return 100*(std::abs(o1->m-o2->m))+0.5*(o1->get_distance(o2));
        return o1->get_distance(o2);
    }
};

// short length lines will be merged first
struct sortbylength {
    bool operator()(const myline* o1, const myline* o2) const {
        return (o1->get_line_length() - o2->get_line_length() > 0);
    }
};

// sort from left to right
struct sortfromlefttoright {
    bool operator()(const myline* o1, const myline* o2) const {
        if(o1->x1 == o2->x1){
            return o1->y1-o2->y1;
        }
        else
            return o1->x1 < o2->x1;
    }
};

// returns the dual edges for all lines
std::vector<myline*> get_reverse_lines(std::vector<myline*> vl){
    std::vector<myline*> reverse_lines;
    for(std::vector<myline*>::iterator iterator = vl.begin(); iterator != vl.end(); ++iterator) {
        myline *t  = *iterator;
        myline *nt = new myline(t->x2, t->x1, t->y2, t->y1);
        nt->reverse_line = t;
        t->reverse_line   = nt;
        reverse_lines.push_back(nt);
    }
    return reverse_lines;
}


void move_points(){
    for(int i=0;i<corner_points.size();++i){
        std::get<0>(corner_points[i]) = std::get<0>(original_corner_points[i])+trans_x;
        std::get<1>(corner_points[i]) = std::get<1>(original_corner_points[i])+trans_y;
    }
}

void book_keeping(){
    map_num_of_lines.clear();
    corner_points_colors.clear();
    corner_points.clear();
    
    // initialize map with 0
    for(int ik=0; ik < original_corner_points.size();++ik){
        int x = std::get<0>(original_corner_points[ik]);
        int y = std::get<1>(original_corner_points[ik]);
        map_num_of_lines[i2tuple(x, y)] = 0;
        corner_points.push_back(original_corner_points[ik]);
        corner_points_colors.push_back(ik%7);
    }
    
    
    // get count of each corner point
    for(int jk=0;jk<all_mylines.size();++jk){
        if(all_mylines[jk]->x1 == -15){
            printf("testing");
        }
        map_num_of_lines[i2tuple(all_mylines[jk]->x1, all_mylines[jk]->y1)] = map_num_of_lines[i2tuple(all_mylines[jk]->x1, all_mylines[jk]->y1)]+1;
        map_num_of_lines[i2tuple(all_mylines[jk]->x2, all_mylines[jk]->y2)] = map_num_of_lines[i2tuple(all_mylines[jk]->x2, all_mylines[jk]->y2)]+1;
    }
    
    valid_lines_undirected = all_mylines;
    
    std::vector<myline*> rv = get_reverse_lines(valid_lines_undirected);
    
    valid_lines_directed.clear();
    
    // add original eges and then
    // add the reverse edges for all edges
    valid_lines_directed.insert(valid_lines_directed.end(), valid_lines_undirected.begin(), valid_lines_undirected.end());
    valid_lines_directed.insert(valid_lines_directed.end(), rv.begin(), rv.end());
    
    move_points();
    
}


// get the number of corner points
// and get the number of lines intersecting at each corner
void get_num_corner_points(){
    
    original_corner_points.clear();
    
    // getting corner points
    for(int ik=0;ik<all_mylines.size();++ik){
        
        bool flag1 = true;
        bool flag2 = true;
        
        for(int jk=0;jk<original_corner_points.size();++jk){
            
            // if either of the point is already inserted then skip that corner
            if(all_mylines[ik]->x1 == std::get<0>(original_corner_points[jk]) && all_mylines[ik]->y1 == std::get<1>(original_corner_points[jk])){
                flag1 = false;
            }
            if(all_mylines[ik]->x2 == std::get<0>(original_corner_points[jk]) && all_mylines[ik]->y2 == std::get<1>(original_corner_points[jk])){
                flag2 = false;
            }
        }
        if(flag1){
            original_corner_points.push_back(i2tuple(all_mylines[ik]->x1, all_mylines[ik]->y1));
        }
        if(flag2){
            original_corner_points.push_back(i2tuple(all_mylines[ik]->x2, all_mylines[ik]->y2));
        }
    }
    
    book_keeping();
    
}

// returns the lines coming on that corner
std::pair<std::vector<int>, bool> get_lines_of_corner(int i){
    int count = 0;
    std::vector<int> pp;
    bool flag = false;
    
    // if only two lines pass through this corner
    int px1 = std::get<0>(original_corner_points[i]);
    int py1 = std::get<1>(original_corner_points[i]);
    
    // get indices of line segments which are going to be merged
    for(int j=0;j<all_mylines.size();++j){
        if((all_mylines[j]->x1 == px1 && all_mylines[j]->y1 == py1) || (all_mylines[j]->x2 == px1 && all_mylines[j]->y2 == py1)){
            pp.push_back(j);
            ++count;
            
            if(count == 2){
                // no other line can use this now
                flag = true;
                break;
            }
        }
    }
    
    return make_pair(pp, flag);

}

myline * get_new_line(int index[]){
    myline * addline;
    
    if((all_mylines[index[0]]->x1 == all_mylines[index[1]]->x1) && (all_mylines[index[0]]->y1 == all_mylines[index[1]]->y1)){
        addline = new myline(all_mylines[index[0]]->x2, all_mylines[index[1]]->x2, all_mylines[index[0]]->y2, all_mylines[index[1]]->y2);
    }else if((all_mylines[index[0]]->x1 == all_mylines[index[1]]->x2) && (all_mylines[index[0]]->y1 == all_mylines[index[1]]->y2)){
        addline = new myline(all_mylines[index[0]]->x2, all_mylines[index[1]]->x1, all_mylines[index[0]]->y2, all_mylines[index[1]]->y1);
    }else if((all_mylines[index[0]]->x2 == all_mylines[index[1]]->x2) && (all_mylines[index[0]]->y2 == all_mylines[index[1]]->y2)){
        addline = new myline(all_mylines[index[0]]->x1, all_mylines[index[1]]->x1, all_mylines[index[0]]->y1, all_mylines[index[1]]->y1);
    }else{
        addline = new myline(all_mylines[index[0]]->x1, all_mylines[index[1]]->x2, all_mylines[index[0]]->y1, all_mylines[index[1]]->y2);
    }
    return addline;
}


// deletes the selected point from the list of corners
void delete_point_from_corners_and_lines(int i){
    int index[2];
    bool flag = false;
    std::vector<myline *> all_mylinestemp(all_mylines);

    // if only two lines pass through this corner
    int px1 = std::get<0>(original_corner_points[i]);
    int py1 = std::get<1>(original_corner_points[i]);
    
    // return if more than 2 lines are present on that corner
    int nump = map_num_of_lines[i2tuple(px1, py1)];
    if(nump != 2){
        return;
    }
    
    std::pair<std::vector<int>, bool> temp =  get_lines_of_corner(i);
    index[0] = temp.first[0];
    index[1] = temp.first[1];
    flag  = temp.second;
    
    myline * addline = get_new_line(index);
    
    all_mylinestemp[index[0]] = NULL;
    all_mylinestemp[index[1]] = NULL;
    all_mylinestemp.push_back(addline);
    
    std::vector<myline*> all_mylines_a;
    
    for(int i=0;i<all_mylines.size();++i){
        if(all_mylinestemp[i] != NULL){
            all_mylines_a.push_back(all_mylines[i]);
        }
    }
    int added = all_mylinestemp.size() - all_mylines.size();
    
    for(int i=all_mylinestemp.size()-1;i>=all_mylinestemp.size()-added;--i){
        all_mylines_a.push_back(all_mylinestemp[i]);
    }
    
    all_mylines = all_mylines_a;
    get_num_corner_points();
}


// merges two lines if they share a corner point
// and the point has only two lines passing through it
void new_mergelines(){
    
    vector<i2tuple> corner_points_temp;
    
    std::vector<myline *> all_mylinestemp(all_mylines);

    // select points which have only two lines passing through it
    
    int count = 0;
    int index[2];
    bool flag = false;
    
    for(int i=0;i<corner_points.size();++i){
        
        flag = false;
        
        // if only two lines pass through this corner
        int px1 = std::get<0>(corner_points[i]);
        int py1 = std::get<1>(corner_points[i]);
        
        if(map_num_of_lines[i2tuple(px1, py1)] == 2){
            std::pair<std::vector<int>, bool> temp =  get_lines_of_corner(i);
            index[0] = temp.first[0];
            index[1] = temp.first[1];
            flag  = temp.second;
        }

        // check if the absolute difference of slope is below some threhold
        if(flag){
            if(all_mylines[index[0]]->get_angle(all_mylines[index[1]], px1, py1) < 15){
                printf("merging lines");
                
                myline * addline = get_new_line(index);
                
                all_mylinestemp[index[0]] = NULL;
                all_mylinestemp[index[1]] = NULL;
                all_mylinestemp.push_back(addline);
                
                map_num_of_lines[i2tuple(px1, py1)] = 0;
                map_num_of_lines[i2tuple(addline->x1, addline->y1)] = 0;
                map_num_of_lines[i2tuple(addline->x2, addline->y2)] = 0;
                
                printf("after merging lines");
            }
        }
    }
    
    std::vector<myline*> all_mylines_a;
    
    for(int i=0;i<all_mylines.size();++i){
        if(all_mylinestemp[i] != NULL){
            all_mylines_a.push_back(all_mylines[i]);
        }
    }
    int added = all_mylinestemp.size() - all_mylines.size();
    
    for(int i=all_mylinestemp.size()-1;i>=all_mylinestemp.size()-added;--i){
        all_mylines_a.push_back(all_mylinestemp[i]);
    }
    
    
    all_mylines = all_mylines_a;
    get_num_corner_points();
    printf("after erasing lines\n");
    
}

void place_polygon(){
    
    polygon *next_polygon_to_place =  get_next_polygon_to_render(polygons_to_place);
    
    std::pair<std::vector<glm::vec3>, std::pair<float, float>> return_pair = next_polygon_to_place->get_min_cost_angle_points(corres_2d, corres_3d,edges_list,plane_to_project, cost_obj, render_scale);
    
    float minangle = return_pair.second.first;
    plane *temp    = next_polygon_to_place->get_plane_with_angle(minangle, plane_to_project);
    
    points_to_render_vec_global = return_pair.first;
    
    next_polygon_to_place->placed = true;
    
    
    for(int i=0;i<points_to_render_vec_global.size();++i){
        glm::vec3 tp = glm::vec3(points_to_render_vec_global[i][0], points_to_render_vec_global[i][1], points_to_render_vec_global[i][2]);
        next_polygon_to_place->points_to_render_vec.push_back(tp);
        next_polygon_to_place->vertices[i][0] = tp[0]/render_scale;
        next_polygon_to_place->vertices[i][1] = tp[1]/render_scale;
        next_polygon_to_place->vertices[i][2] = tp[2]/render_scale;
    }
    
    // insert the points in the already placed list
    insert_corres(points_to_render_vec_global, next_polygon_to_place->get_points_vec());
    // insert in the already inserted edges
    insert_edges(points_to_render_vec_global,  next_polygon_to_place->get_points_vec());
    
    
    std::vector<polygon*> adjacent_polygons = next_polygon_to_place->get_adjacent_polygons_using_huffman(all_polygons, corres_2d, corres_3d, true);
    
    // insert the adjacent polygons into the list of polygons
    insert_into_polygons_to_render_list(adjacent_polygons);

    
    
    current_polygon = current_polygon+1;
    
    while(true){
        bool new_polygon_added = false;
        
        for(int pl = 0;pl < all_polygons.size(); ++pl){
            
            if(all_polygons[pl]->placed)
                continue;
        
            if(check_2_polygons_already_placed(all_polygons[pl])){
                plane * newplane = get_plane(all_polygons[pl]);
                prepare_points_to_project(all_polygons[pl]);
            
                std::vector<glm::vec3> points_to_render_vec_temp = newplane->project_polygon(points_in_camera, -5, 5, 5);
            
            
                for(int k=0;k<points_to_render_vec_temp.size();++k){
                    glm::vec3 tp = glm::vec3(points_to_render_vec_temp[k][0], points_to_render_vec_temp[k][1], points_to_render_vec_temp[k][2]);
                    all_polygons[pl]->points_to_render_vec.push_back(tp);
                    all_polygons[pl]->vertices[k][0] = tp[0]/render_scale;
                    all_polygons[pl]->vertices[k][1] = tp[1]/render_scale;
                    all_polygons[pl]->vertices[k][2] = tp[2]/render_scale;
                }
            
                all_polygons[pl]->placed = true;
                new_polygon_added = true;
            
                
                // insert the points in the already placed list
                insert_corres(points_to_render_vec_temp, all_polygons[pl]->get_points_vec());
                insert_edges(points_to_render_vec_temp, all_polygons[pl]->get_points_vec());
                
                //printf("TESTING BUG 18 start\n");
                std::vector<polygon*> adjacent_polygons = all_polygons[pl]->get_adjacent_polygons_using_huffman(all_polygons, corres_2d,
                                                                                                                corres_3d, true);
                // insert the adjacent polygons into the list of polygons
                //printf("TESTING BUG 18 end\n");
                insert_into_polygons_to_render_list(adjacent_polygons);
                
            }
        }
        
        // break the loop if no new polygon is inserted
        if(!new_polygon_added)
            break;
    }
    
    //display_type = 2;
    printf("testng pranjal");
    
    return;
}


//Called when a key is pressed
void handleKeypressa(unsigned char key, int x, int y) {
    switch (key) {
        case 'p':
            display_type = 1;       // show polygons
            break;
        case 'l':
            display_type = 0;       // show lines
            break;
        case 'o':
            display_type = 2;       // show projected polygon
            break;
        case 'r':                   // display the project polygon
            display_type = 3;
            angle_p  = 1-angle_p;       // on each key press increases the angle by 1
            break;
        case 'q':
            angle_p = angle_p+0.5;
            break;
        case 'a':
            angle_p = angle_p-0.5;
            break;
        case 'x':
            tr_x = tr_x+0.5;
            break;
        case 'y':
            tr_y = tr_y+0.5;
            break;
        case 'z':
            tr_z = tr_z+0.5;
            break;
        case 'j':
            zglut = zglut-1;
            break;
        case '1':
            xview = xview+0.5;
            break;
        case '2':
            yview = yview +0.5;
            break;
        case '3':
            zview = zview+0.5;
            break;
        case '4':
            xview = xview-0.5;
            break;
        case '5':
            yview = yview-0.5;
            break;
        case '6':
            zview = zview-0.5;
            break;
        case 'g':
            place_polygon();
            break;
        case 27: //Escape key
            exit(0);
    }
}



// this function will be changed later to incorporate sweeping line
// at present we are only using the (number of points intersected)/(distane between this two points)
// can be changed later
std::vector<myline*> get_all_valid_lines(){
    vector<myline*> all_line_pairs;
    
    // generate all line pairs
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        for(std::vector<i2tuple>::iterator ita = it+1; ita != corner_points.end(); ++ita){
            int x1 = get<0>(*it);
            int y1 = get<1>(*it);
            int x2 = get<0>(*ita);
            int y2 = get<1>(*ita);
            if(!(x1 == x2 && y1 == y2)){
                all_line_pairs.push_back(new myline(x1, x2, y1, y2));
            }
        }
    }
    
    //printf("SIZE OF ALL LINES BEFORE %d\n", all_line_pairs.size());
    int valid_count = 0;
    for(std::vector<myline*>::iterator iterator = all_line_pairs.begin(); iterator != all_line_pairs.end();) {
        myline *ml = *iterator;
        int pointsize = ml->pointliecount(points_vector);
        float linelength = ml->get_line_max_dimension_length();
        
        // dimension across which ratio will be taken
        bool xismax = false;
        if(ml->get_line_max_dimension_length() ==  abs(ml->x2-ml->x1))
            xismax = true;
        
        float ratio = pointsize/linelength;
        //printf("A(%d,%d) B(%d,%d)    %d %f %f\n", ml->x1,ml->y1, ml->x2, ml->y2, pointsize, linelength, ratio);
        
        if( ratio < POINT_PAIR_LYING_THRESH){
            iterator = all_line_pairs.erase(iterator);     // erase a line if the number of points lying on that line
            //printf("size after removal is %d\n", all_line_pairs.size());
                                                // is smaller than the threshold * distance between those two point
        }
        else{
            //printf("lying count %d length is %f ratio is %f\n", pointsize, linelength, ratio);
            valid_count = valid_count +1;
            iterator++;
        }
    }
    
    //printf("SIZE OF ALL LINES AFTER %d valid_count is %d\n", all_line_pairs.size(), valid_count);
    return all_line_pairs;
}


void plotpoint(i2tuple pt){
    int i = get<0>(pt);
    int j = get<1>(pt);
    circle( dst_norm_scaled, Point( j, i ), 5,  Scalar(0, 255, 0), 2, 8, 0 );
    return;
}

void initGL() {
    int back = 1;
    GLfloat colors[][3] = { { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f, 1.0f }, {0.0f, 0.0f, 0.0f } };
    glClearColor(colors[back][0], colors[back][1], colors[back][2], 1.0f); // Set background color to black and opaque
    glClearDepth(1.0f);                   // Set background depth to farthest
    glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
    glShadeModel(GL_SMOOTH);   // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
    glEnable( GL_POINT_SMOOTH );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING); //Enable lighting
    glEnable(GL_LIGHT0); //Enable light #0
    glEnable(GL_LIGHT1); //Enable light #1
    glEnable(GL_NORMALIZE); //Automatically normalize normal
    //glEnable(GL_CULL_FACE);
    
}



void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0) height = 1;                // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
    
}

void plot_corner_points_and_lines(Mat dst_norm_scaled, std::vector<myline*> valid_lines_undirected, vector<i2tuple> corner_points){
    
    int count = 0;
    for(std::vector<i2tuple>::iterator it = corner_points.begin(); it != corner_points.end(); ++it){
        i2tuple pt = *it;
        int i = get<0>(pt);
        int j = get<1>(pt);
        //circle( dst_norm_scaled, Point( i, j ), 15,  Scalar(0, 255, 0), 2, 8, 0 );
        circle( dst_norm_scaled, Point( j, i ), 5,  Scalar(255, 255, 255), 2, 8, 0 );
        //printf("CORNER (%d, %d)\n", i, j);
        ++count;
        //if(count >1)
        //    break;
        
    }
    count =0;
    for(int i=0;i<valid_lines_undirected.size();++i){
        myline* pt = valid_lines_undirected[i];
        int x1 = pt->x1;
        int x2 = pt->x2;
        int y1 = pt->y1;
        int y2 = pt->y2;
        //line(dst_norm_scaled, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 0), 1, 8, 0);
        //if(i == 17 || i == 31){
        //    line(dst_norm_scaled, Point(y1, x1), Point(y2, x2), Scalar(255, 255, 255), 10, 8, 0);
        //}else{
            line(dst_norm_scaled, Point(y1, x1), Point(y2, x2), Scalar(255, 255, 255), 1, 8, 0);
        //}
        //printf("LINE (%d, %d) -> (%d, %d)\n", x1, y1, x2, y2);
    }
    
    return;
}

// get the nearest point to the point clicked
int get_nearest_point(int xa2, int ya2){
    
    int index = -1;
    float min = 100000;
    
    float x2 = (xa2+0.0-sa_width/2)/(sa_width/2)*3;
    float y2 = (ya2+0.0-sa_height/2)/(sa_height/2)*3;
    
    for(int i=0; i< corner_points.size();++i){
        float x1 = std::get<0>(corner_points[i])*(img_scale+0.0)/sa_width;
        float y1 = std::get<1>(corner_points[i])*(img_scale+0.0)/sa_height;
        
        float dist =sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
        
        if(dist < min){
            index = i;
            min   = dist;
        }
    }
    
    return index;
}




// first is origin
// then x, y, z
void mousemotion(int button, int state, int x, int y){
    if(state == GLUT_DOWN){
        printf("mouse >> %d %d\n", x, sa_height-y);
        
        int index = get_nearest_point(x, sa_height-y);
        
        point_selected = index;
        
        if(draw_axis){
            int px = std::get<0>(original_corner_points[point_selected]);
            int py = std::get<1>(original_corner_points[point_selected]);
            
            axis_2d_points.push_back(glm::vec2(px, py));
            // instantiate cost object when 4 points have been clicked
            if(axis_2d_points.size() == 4){
                cost_obj  = new cost(axis_2d_points);
                draw_axis =  false;
            }
        }
    }
}

//http://stackoverflow.com/questions/16990721/removing-blobs-from-a-binary-image

void remove_noise(Mat& im){
    // threashold specifying minimum area of a blob
    double threshold = 10;
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    vector<int> small_blobs;
    double contour_area;
    Mat temp_image;
    
    // find all contours in the binary image
    im.copyTo(temp_image);
    findContours(temp_image, contours, hierarchy, CV_RETR_CCOMP,
                 CV_CHAIN_APPROX_SIMPLE);
    
    // Find indices of contours whose area is less than `threshold`
    if ( !contours.empty()) {
        for (size_t i=0; i<contours.size(); ++i) {
            contour_area = contourArea(contours[i]) ;
            if ( contour_area < threshold)
                small_blobs.push_back(i);
        }
    }
    
    // fill-in all small contours with zeros
    for (size_t i=0; i < small_blobs.size(); ++i) {
        drawContours(im, contours, small_blobs[i], cv::Scalar(0),
                     CV_FILLED, 8);
    }
    
    printf("SIZE OF SMALL BLOBS IS %d\n", small_blobs.size());
    return;
}

// returns the minimum vertex
// has the minimum y and maximumx
glm::vec2 get_min(Mat& im){
    std::vector<glm::vec2> miny_points;
    int miny = 1000000;
    int maxx = -1;
    
    for (int i = 0; i < im.rows-1; i++){
        for (int j = 0; j < im.cols-1; j++){
            uchar p2 = im.at<uchar>(i, j);
            if(p2 > 0 && miny > j){
                miny = j;
                break;
            }
        }
    }
    
    for(int i=im.rows-1;i >= 0; --i){
        uchar p2 = im.at<uchar>(i, miny);
        if(p2 > 0){
            maxx = i;
            break;
        }
    }
    
    return glm::vec2(maxx, miny);
}


bool get_non_zero(Mat &im){
    for (int i = 0; i < im.rows-1; i++){
        for (int j = 0; j < im.cols-1; j++){
            if(im.at<uchar>(i, j) > 0){
                return true;
            }
        }
    }
    return false;
}

// once the line  has been fitted using split and merge
// zero out the line (all the white pixels in im2) so that rest of the pixels can be vectorized
void zero_the_line(Mat& im, Mat& im2, std::vector<glm::vec2> lines){
    for (int i = 0; i < im.rows-1; i++){
        for (int j = 0; j < im.cols-1; j++){
            if(im2.at<uchar>(i, j) > 0){
                im.at<uchar>(i, j) = 0;
            }
        }
    }
    return;
}

//Refer:: https://en.wikipedia.org/wiki/Ramer%E2%80%93Douglas%E2%80%93Peucker_algorithm

std::vector<glm::vec2> split_and_merge(std::vector<glm::vec2> points){
    // Find the point with the maximum distance
    int dmax  = 0;
    int index = 0;
    int size  = points.size();
    float d   = 0;
    int epsilon = 1;
    
    std::vector<glm::vec2> result;
    
    for(int i = 1; i < points.size()-1; ++i) {
        myline * l = new myline(points[0][0], points[size-1][0], points[0][1], points[size-1][1]);
        d = l->get_perpendicular_distance(points[i]);
        if ( d > dmax ) {
            index = i;
            dmax  = d;
        }
    }
    
    // If max distance is greater than epsilon, recursively simplify
    if ( dmax > epsilon ) {
        // Recursive call
        //std::vector<glm::vec2> points1(points.begin(), points.begin()+index+1);
        std::vector<glm::vec2> points1(points.begin(), points.begin()+index);
        std::vector<glm::vec2> points2(points.begin() + index, points.end());

        std::vector<glm::vec2> recResults1 = split_and_merge(points1);
        std::vector<glm::vec2> recResults2 = split_and_merge(points2);
        
        // add the reverse edges for all edges
        result.insert(result.end(), recResults1.begin(), recResults1.end()-1);
        result.insert(result.end(), recResults2.begin(), recResults2.end());
    } else {
        result.push_back(points[0]);
        result.push_back(points[size-1]);
    }
    
    return result;
}

std::vector<glm::vec2> get_polylines(Mat& im){
    std::vector<glm::vec2> points;
    
    im.copyTo(m2);
    im.copyTo(m3);
    
    for (int i = 0; i < im.rows-1; i++){
        for (int j = 0; j < im.cols-1; j++){
            m2.at<uchar>(i, j) = 0;
            m3.at<uchar>(i, j) = 0;
        }
    }
    
    glm::vec2 minp = get_min(im);
    int x = minp[0];
    int y = minp[1];

    m2.at<uchar>(x, y) = 1;
    
    points.push_back(glm::vec2(x, y));
    
    while(x < m2.rows && y < m2.cols && y >= 1 && x >= 1){
        if (im.at<uchar>(x, y+1) > 0 && m2.at<uchar>(x, y+1) == 0){
            m2.at<uchar>(x, y+1) = 255;
            y = y+1;
            points.push_back(glm::vec2(x, y));
            //printf("> %d %d\n", x, y);
        }
        else if (im.at<uchar>(x+1, y) > 0 && m2.at<uchar>(x+1, y) == 0){
            m2.at<uchar>(x+1, y) = 255;
            x = x+1;
            points.push_back(glm::vec2(x, y));
            //printf(">> %d %d\n", x, y);
        }
        else if (im.at<uchar>(x+1, y+1)  > 0 && m2.at<uchar>(x+1, y+1) == 0){
            m2.at<uchar>(x+1, y+1) = 255;
            y = y+1;
            x = x+1;
            points.push_back(glm::vec2(x, y));
            //printf(">>> %d %d\n", x, y);
        }
        else if (im.at<uchar>(x-1, y) > 0 && m2.at<uchar>(x-1, y) == 0){
            m2.at<uchar>(x-1, y) = 255;
            x = x-1;
            //printf(">> %d %d\n", x, y);
            points.push_back(glm::vec2(x, y));
        }
        else if (im.at<uchar>(x-1, y+1) > 0 && m2.at<uchar>(x-1, y+1) == 0){
            m2.at<uchar>(x-1, y+1) = 255;
            x = x-1;
            y = y+1;
            //printf(">> %d %d\n", x, y);
            points.push_back(glm::vec2(x, y));
        }
        else{
            break;
        }
    }
    
    return points;
}

void init_values(){
    // sequence of polygons to be placed this will be done automatically later
    poly_seq[0] = 0;
    poly_seq[1] = 3;
    poly_seq[2] = 4;
    
    tess = gluNewTess();

    imga = imread("/Users/pranjal/Desktop/image/huffman99.png", CV_LOAD_IMAGE_GRAYSCALE);
    imgc = imread("/Users/pranjal/Desktop/image/huffman99.png");

    
    bw   = imga > 120;
    img  = bw > 120;
    
    imwrite("/Users/Pranjal/Downloads/Graphics/test15.jpg", img);
    bitwise_not(bw, img);
    imwrite("/Users/Pranjal/Downloads/Graphics/test16.jpg", img);

    thinning(img);
    
    remove_noise(img);
    
    imwrite("/Users/Pranjal/Downloads/Graphics/test17.jpg", img);
}



// gets the line segments using the split and merge algorithm
// get all the non zero pixels


// removes the outer polygon from the list of all_polygons
void remove_outer_polygon(){
    polygon *to_erase;
    
    for(std::vector<polygon*>::iterator it = all_polygons.begin(); it != all_polygons.end(); ++it){
        polygon *pp = *it;
        std::vector<myline*> lines = pp->lines;
        
        bool flag = true;
        for(int i=0;i<lines.size();++i){
            if(lines[i]->label.compare("blue") != 0){
                flag = false;
                break;
            }
        }
        
        if(flag){
            to_erase = pp;
            break;
        }
    }
    
    all_polygons.erase(std::remove(all_polygons.begin(), all_polygons.end(), to_erase), all_polygons.end());
    return;
}


void merge_line_corners(){
    
    bool flag = false;
    bool mergeline =  false;
    int iteration = 0;
    
    int left_count = 0;
    
    while(1){
        
        mergeline  = false;
        flag       = false;
        left_count = 0;
        
        ++iteration;
        
        
        float minvalue_g  = 100000;
        int minindexa_g   = -1;
        int minindexb_g   = -1;
        float a, b, c, d;
        
        int count1_g = 0;
        int count2_g = 0;
        
        for(int i=0;i<all_mylines.size();++i){
            
            int count1 = 0;
            int count2 = 0;
            
            float minvalue  = 100000;
            int minindexa   = i;
            int minindexb   = -1;
            
            float a_t = 0, b_t = 0, c_t = 0, d_t = 0;
            
            for(int j=0; j< all_mylines.size();++j){
                
                // same line
                if(j == i)
                    continue;
                
                if(all_mylines[i]->x1 == all_mylines[j]->x1 && all_mylines[i]->y1 == all_mylines[j]->y1){
                    count1 = count1 +1;
                }else if(all_mylines[i]->x2 == all_mylines[j]->x1 && all_mylines[i]->y2 == all_mylines[j]->y1){
                    count2 = count2+1;
                }else if(all_mylines[i]->x1 == all_mylines[j]->x2 && all_mylines[i]->y1 == all_mylines[j]->y2){
                    count1 = count1 +1;
                }else if(all_mylines[i]->x2 == all_mylines[j]->x2 && all_mylines[i]->y2 == all_mylines[j]->y2){
                    count2 = count2+1;
                }else{
                    
                    float a_tp, b_tp, c_tp, d_tp, ap_tp, bp_tp, cp_tp, dp_tp;
                    
                    a_tp = glm::length(glm::vec2(all_mylines[j]->x1-all_mylines[i]->x1, all_mylines[j]->y1 - all_mylines[i]->y1));
                    b_tp = glm::length(glm::vec2(all_mylines[j]->x1-all_mylines[i]->x2, all_mylines[j]->y1 - all_mylines[i]->y2));
                    c_tp = glm::length(glm::vec2(all_mylines[j]->x2-all_mylines[i]->x1, all_mylines[j]->y2 - all_mylines[i]->y1));
                    d_tp = glm::length(glm::vec2(all_mylines[j]->x2-all_mylines[i]->x2, all_mylines[j]->y2 - all_mylines[i]->y2));
                    
                    if(all_mylines[j]->x2 == 91 && all_mylines[i]->x2 == 91 ){
                        printf("debugging test\n");
                    }
                    
                    float mina = min(a_tp, b_tp);
                    mina = min(mina, c_tp);
                    mina = min(mina, d_tp);
                    
                    if(mina < minvalue){
                        minvalue = mina;
                        minindexa = i;
                        minindexb = j;
                        
                        a_t = a_tp;
                        b_t = b_tp;
                        c_t = c_tp;
                        d_t = d_tp;
                    }
                }
            }
            
            // already merged line
            if(!(count1 > 0 && count2 > 0)){
                count1_g = count1;
                count2_g = count2;
                
                ++left_count;
                if(minvalue < minvalue_g){
                    minindexa_g = minindexa;
                    minindexb_g = minindexb;
                    minvalue_g  = minvalue;
                    
                    a = a_t;
                    b = b_t;
                    c = c_t;
                    d = d_t;
                }
            }
        }
        
        if(minindexa_g == -1){
            printf("All lines merged\n");
            return;
        }
        
        
        int thresh_m = 10;
        
        int i = minindexa_g;
        int j = minindexb_g;
        
        printf("i = %d, j = %d, minvalue = %f length_a = %f length_b %f leftcount = %d\n", minindexa_g, minindexb_g, minvalue_g, all_mylines[i]->get_line_length(), all_mylines[j]->get_line_length(), left_count);
        
        if(minvalue_g < thresh_m && minvalue_g != 0){
        //if(minvalue != 0){
            printf("Below threshold\n");
            
            flag       = true;
            int prev_x = -1;
            int prev_y = -1;
            int new_x  = -1;
            int new_y  = -1;
            
            // Decide which line's coordinates will be replaced
            // the line which has smaller length will be changed
            if(all_mylines[j]->get_line_length() < all_mylines[i]->get_line_length()){
                if(  min (a, b) < min(c, d)){
                    prev_x = all_mylines[j]->x1;
                    prev_y = all_mylines[j]->y1;
                    
                    if(a < b){
                        all_mylines[j]->x1 = all_mylines[i]->x1;
                        all_mylines[j]->y1 = all_mylines[i]->y1;
                        
                    }else{
                        all_mylines[j]->x1 = all_mylines[i]->x2;
                        all_mylines[j]->y1 = all_mylines[i]->y2;
                    }
                    
                    new_x = all_mylines[j]->x1;
                    new_y = all_mylines[j]->y1;
                    
                }else{
                    prev_x = all_mylines[j]->x2;
                    prev_y = all_mylines[j]->y2;
                    
                    if(c < d){
                        all_mylines[j]->x2 = all_mylines[i]->x1;
                        all_mylines[j]->y2 = all_mylines[i]->y1;
                    }else{
                        all_mylines[j]->x2 = all_mylines[i]->x2;
                        all_mylines[j]->y2 = all_mylines[i]->y2;
                    }
                    
                    new_x = all_mylines[j]->x2;
                    new_y = all_mylines[j]->y2;
                    
                }
            }else{
                if(min(a, c) < min(b, d)){
                    prev_x = all_mylines[i]->x1;
                    prev_y = all_mylines[i]->y1;
                    
                    if(a < c){
                        all_mylines[i]->x1 = all_mylines[j]->x1;
                        all_mylines[i]->y1 = all_mylines[j]->y1;
                    }else{
                        all_mylines[i]->x1 = all_mylines[j]->x2;
                        all_mylines[i]->y1 = all_mylines[j]->y2;
                    }
                    
                    new_x = all_mylines[i]->x1;
                    new_y = all_mylines[i]->y1;
                    
                }else{
                    prev_x = all_mylines[i]->x2;
                    prev_y = all_mylines[i]->y2;
                    
                    if(b < d){
                        all_mylines[i]->x2 = all_mylines[j]->x1;
                        all_mylines[i]->y2 = all_mylines[j]->y1;
                    }else{
                        all_mylines[i]->x2 = all_mylines[j]->x2;
                        all_mylines[i]->y2 = all_mylines[j]->y2;
                    }
                    
                    new_x = all_mylines[i]->x2;
                    new_y = all_mylines[i]->y2;
                    
                }
            }
            
            // change all the lines which are affected due to shift
            for (int pl = 0;pl < all_mylines.size();++pl){
                if(all_mylines[pl]->x1 == prev_x && all_mylines[pl]->y1 == prev_y){
                    all_mylines[pl]->x1 = new_x;
                    all_mylines[pl]->y1 = new_y;
                }else if(all_mylines[pl]->x2 == prev_x && all_mylines[pl]->y2 == prev_y){
                    all_mylines[pl]->x2 = new_x;
                    all_mylines[pl]->y2 = new_y;
                }
            }
            
            printf("After merge iteration\n");
        }
        
        
        printf("After merging the lines\n");
        
        
        // this means threshold has crossed and line is remaining which does not have two corners merged
        // this means we will first find all lines which are not merged
        // select the one whose corner is nearest to any line (perpendicular distance)
        // and then split obtained line into two across that corner
        if(!flag){
            
            // line i's corner 1 is not merged
            glm::vec2 pkl;
            if(count1_g < 1){
                pkl = glm::vec2(all_mylines[i]->x1, all_mylines[i]->y1);
            }
            else if(count2_g < 1){
                pkl = glm::vec2(all_mylines[i]->x2, all_mylines[i]->y2);
            }
            
            float mindist = 100000;
            int minindex = -1;
            
            // get the line whose perpendicular distance is the minimum from this point
            for(int pk=0;pk<all_mylines.size();++pk){
                if(i == pk)
                    continue;
                
                float dist = all_mylines[pk]->get_perpendicular_distance(glm::vec2(pkl[0], pkl[1]));
                if(dist < mindist){
                    mindist = dist;
                    minindex = pk;
                }
            }
            
            // remove the perpendicular line and insert two new ones
            myline *newline1 = new myline(all_mylines[minindex]->x1, (int)pkl[0], all_mylines[minindex]->y1, (int)pkl[1]);
            myline *newline2 = new myline(all_mylines[minindex]->x2, (int)pkl[0], all_mylines[minindex]->y2, (int)pkl[1]);
            all_mylines[minindex] = newline1;
            all_mylines.push_back(newline2);
        }
    
        
        get_num_corner_points();
        
//        char buffer [1000];
//        sprintf (buffer, "/Users/pranjal/Downloads/Graphics/checkit/corners_window_%d.jpg", iteration);
//        
////        for(int i=0;i< all_mylines.size();++i){
////            int c = rand()%7;
////            line(imgc, Point(all_mylines[i]->y1, all_mylines[i]->x1), Point(all_mylines[i]->y2, all_mylines[i]->x2), Scalar(colors[c][0],colors[c][1],colors[c][2]), 2, 8, 0);
////        }
//        
//        img.copyTo(dst_norm_scaled);
//        plot_corner_points_and_lines(dst_norm_scaled, all_mylines, corner_points);
//        imwrite( buffer, dst_norm_scaled );
    }
    
    printf("after connecting line segments\n");
    return ;
}



int   wireframe = 0;
int   segments = 8;
int   main_window;


void myGlutIdle( void )
{
    if ( glutGetWindow() != main_window )
        glutSetWindow(main_window);
    
    glutPostRedisplay();
}


int move_const = 5;

void move_left(){
    trans_x = trans_x-move_const;
    move_points();
    glFlush();
    glutPostRedisplay();
}
void move_right(){
    trans_x = trans_x+move_const;
    move_points();
    glFlush();
    glutPostRedisplay();
}
void move_up(){
    trans_y = trans_y+move_const;
    move_points();
    glFlush();
    glutPostRedisplay();
}
void move_down(){
    trans_y = trans_y-move_const;
    move_points();
    glFlush();
    glutPostRedisplay();
}
void reset_scale(){
    trans_x = 0;
    trans_y = 0;
    move_points();
    
    if(display_type == 3){
        render_scale = render_scale-5;
    }else{
        img_scale = 3;
    }
    glFlush();
    glutPostRedisplay();
}
void increase_scale(){
    if(display_type == 3){
        render_scale = render_scale+5;
    }else{
        img_scale = img_scale+1;
    }
    glFlush();
    glutPostRedisplay();
}
void decrease_scale(){
    if(display_type == 3){
        render_scale = render_scale-5;
    }else{
        img_scale = img_scale-1;
    }
    glFlush();
    glutPostRedisplay();
}

void delete_point_button(){
    if(point_selected < 0){
        return;
    }
    draw_axis = false;
    delete_point_from_corners_and_lines(point_selected);
    point_selected = -1;
    glFlush();
    glutPostRedisplay();
    return;
}
void draw_axis_button(){
    draw_axis = true;
}
void show_projected_polygon(){
    display_type = 2;
    glFlush();
    glutPostRedisplay();
}
void show_lines(){
    display_type = 4;
    glFlush();
    glutPostRedisplay();
}
void show_huffman_lines(){
    if(!huffman_done){
        get_huffman_label(valid_lines_directed, valid_lines_undirected, corner_points);
        huffman_done = true;
    }
    display_type = 0;
    glFlush();
    glutPostRedisplay();
}
void show_polygons(){
    display_type = 1;
    if(!polygon_done){
        all_polygons = get_all_polygons(valid_lines_directed);
        // remove the outer polygon which only contains occluding edges
        remove_outer_polygon();
        polygon_done= true;
    }
    
    glFlush();
    glutPostRedisplay();
}
void show_3d(){
    display_type = 3;
    
    if(!work_3d_done){
        
        if(!huffman_done){
            get_huffman_label(valid_lines_directed, valid_lines_undirected, corner_points);
            huffman_done = true;
        }
        if(!polygon_done){
            all_polygons = get_all_polygons(valid_lines_directed);
            // remove the outer polygon which only contains occluding edges
            remove_outer_polygon();
            polygon_done= true;
        }
        
        for(int i=0;i<all_mylines.size();++i){
            printf("LINE LABEL > %d %s\n", i, all_mylines[i]->label.c_str());
        }
        
        // insert the first polygon to place in the list
        polygons_to_place.push_back(all_polygons[poly_seq[current_polygon]]);
        
        // we start with xy = 0  then rotate this plane till we get the best plane to project
        plane_to_project = new plane(0, 0, 1, new mypoint(0, 0, 0));
        
        
        //placing all polygons
        while(1){
            bool flag = false;
            for(int i=0;i<polygons_to_place.size();++i){
                if(!polygons_to_place[i]->placed){
                    flag = true;
                    break;
                }
            }
            if(flag)
            place_polygon();
            else{
                break;
            }
        }
        
        work_3d_done = true;
    }
    
    glFlush();
    glutPostRedisplay();
}

void glui_setup(){
    
//    GLUI *glui_subwin = GLUI_Master.create_glui_subwindow(main_window,
//                                            GLUI_SUBWINDOW_LEFT);
//    

        GLUI *glui_subwin = GLUI_Master.create_glui("GUI Controls");

    
    //GLUI_Spinner *segment_spinner = glui_subwin->add_spinner( "Segments:", GLUI_SPINNER_INT, &segments );
    //segment_spinner->set_int_limits( 3, 60 );
    
    
    glui_subwin->set_main_gfx_window( main_window );
    
    glui_subwin->add_separator();
    //GLUI_Panel *obj_panel = glui_subwin->add_panel ("Test Panel");
    
    
    // ADD BUTTONS
    glui_subwin->add_button("INCREASE SCALE", 0, (GLUI_Update_CB)increase_scale);
    glui_subwin->add_button("DECREASE SCALE", 1, (GLUI_Update_CB)decrease_scale);
    glui_subwin->add_button("RESET", 1, (GLUI_Update_CB)reset_scale);
    glui_subwin->add_button("MOVE LEFT", 2, (GLUI_Update_CB)move_left);
    glui_subwin->add_button("MOVE RIGHT", 3, (GLUI_Update_CB)move_right);
    glui_subwin->add_button("MOVE UP", 4, (GLUI_Update_CB)move_up);
    glui_subwin->add_button("MOVE DOWN", 5, (GLUI_Update_CB)move_down);
    glui_subwin->add_button("SHOW LINES", 6, (GLUI_Update_CB)show_lines);               // simple lines
    glui_subwin->add_button("SHOW HUFFMAN LINES", 7, (GLUI_Update_CB)show_huffman_lines);       // huffman lines
    glui_subwin->add_button("SHOW POLYGONS", 8, (GLUI_Update_CB)show_polygons);         // show polygons
    glui_subwin->add_button("SHOW PROJECTED POLYGON", 9, (GLUI_Update_CB)show_projected_polygon);
    glui_subwin->add_button("DRAW AXIS", 10, (GLUI_Update_CB)draw_axis_button);
    glui_subwin->add_button("DELETE POINT", 11, (GLUI_Update_CB)delete_point_button);
    glui_subwin->add_button("SHOW 3D", 12, (GLUI_Update_CB)show_3d);                    // show 3d
    glui_subwin->add_button("Quit", 13, (GLUI_Update_CB)exit);
    
    
    
//    GLUI_RadioGroup *group1 = glui_subwin->add_radiogroup_to_panel(obj_panel);
//    glui_subwin->add_radiobutton_to_group(group1,"Option 1");
//    glui_subwin->add_radiobutton_to_group(group1,"Option 2");
    //GLUI_Rotation *arcball = glui_subwin->add_rotation("ball (doesn't do anything)");
    
    
    GLUI_Master.set_glutIdleFunc( myGlutIdle );
    GLUI_Master.set_glutReshapeFunc( reshape);
    
}



int old_x = 0;
int old_y = 0;
bool valid;

void mouse_func(int button, int state, int x, int y) {
    old_x=x;
    old_y=y;
    valid = (state == GLUT_DOWN);
}

void motion_func (int x, int y) {
    if (valid) {
        int dx = old_x - x;
        int dy = old_y - y;
    }
    old_x = x;
    old_y = y;
}


void get_all_lines_using_split_and_merge(){
    int iteration = 0;
    
    while(get_non_zero(img)){
        iteration = iteration+1;
        
        std::vector<glm::vec2> po    = get_polylines(img);
        std::vector<glm::vec2> lines = split_and_merge(po);
        
        for(int i=0;i<lines.size()-1;++i){
            all_mylines.push_back(new myline(lines[i][0], lines[i+1][0],lines[i][1], lines[i+1][1]));
        }
        
        zero_the_line(img, m2, lines);
    }
    
    merge_line_corners();   // get connected lines
    new_mergelines();       // merge lines with same slope
    return;
}

int main(int argc, char** argv){
    
    init_values();
    get_all_lines_using_split_and_merge();
    
    int colors[][3] = { { 0, 255, 0}, {255,0,0}, {0,0,255}, {0,255,255}, {255,255,0},  {155,155,0}, {0,155,155}};

    for(int i=0;i< all_mylines.size();++i){
        int c = 1;//rand()%7;
        line(imgc, Point(all_mylines[i]->y1, all_mylines[i]->x1), Point(all_mylines[i]->y2, all_mylines[i]->x2), Scalar(colors[c][0],colors[c][1],colors[c][2]), 2, 8, 0);
    }
    
    
    // get all the valid lines by checking the ratio of points lying on the line and its length
    //valid_lines_undirected = get_all_valid_lines();
    valid_lines_undirected = all_mylines;
    
    // showing the result of line detection
//    img.copyTo(dst_norm_scaled);
//    plot_corner_points_and_lines(dst_norm_scaled, valid_lines_undirected, corner_points);
//    namedWindow( "corners_window", WINDOW_NORMAL );
//    resizeWindow("corners_window", 800,800);
//    imshow( "corners_window", img );
//    //cv::resize(img, img, Size(), 0.5, 0.5);
//    imshow( "corners_window", dst_norm_scaled);
//    imwrite("/Users/Pranjal/Downloads/Graphics/test1.jpg", img);
//    //imshow( "corners_window", bw );
    //waitKey(0);

    typedef std::map<tuple<int, int>, int>::iterator it_type;
    for(it_type iterator = map_num_of_lines.begin(); iterator != map_num_of_lines.end(); iterator++) {
        printf(">>>>>>>>> (%d, %d) %d\n", std::get<0>(iterator->first), std::get<1>(iterator->first) , iterator->second);
    }
    
    
    for(int i=0;i<all_mylines.size();++i){
        printf("LINES ARE (%d, %d) -> (%d, %d)\n", all_mylines[i]->x1, all_mylines[i]->y1, all_mylines[i]->x2, all_mylines[i]->y2);
    }
    
    valid_lines_undirected = all_mylines;
    
    corner_points = get_correct_coord_point_and_line(corner_points, valid_lines_undirected);
    book_keeping();
    
    
    
    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);
    
    glutInitWindowSize(sa_width, sa_height);
    
    main_window = glutCreateWindow("3D Model");  // Create a window with the given title
    
    printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

    
    glutInitWindowPosition(300, 50);         // Position the window's initial top-left corner
    glutDisplayFunc(displayone);            // Register display callback handler for window re-paint
    glutKeyboardFunc(handleKeypressa);
    glutMouseFunc(mousemotion);
    glutTimerFunc(25, updatea, 0);
    initGL();
    glui_setup();
    glutMainLoop();
    return 0;
}


#endif
