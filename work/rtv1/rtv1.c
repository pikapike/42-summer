#include "./minilibx_macos/mlx.h"
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void *canvas; // coords: x, y as in screen, z = depth coming in/out of screen
int scrx = 875, scry = 625, mid_x = 437, mid_y = 312; // Origin (0, 0) at px = 437, py = 312
void *core;
double **shapes;
int num_shapes;
double *light;
int cam_dist = 1500; // Cam pos: (0, 0, -cam_dist)

// Sphere (0): (a, b, c, r): (x-a)^2+(y-b)^2+(z-c)^2=r^2
// Cone (1): (Figure this out later) (v, a, b, c, d, e, f) Line: y=ax+b, z=cx+d Radius: r=e(w->x=0, y=1, z=2)+f (cylinder: r=f)
// Plane (2): (a, b, c, d) ax+by+cz=d
// Color at the end
// I should probably make structs

/*
 ^
 |
 |
 |  negative y
 |
 |
 *---------> positive x
 |
 |
 |
 |  positive y
 |
*/

void init_shapes() {
    num_shapes = 4;
    shapes = (double **) malloc(sizeof(double *) * num_shapes);
    light = (double *) malloc(sizeof(double) * 3);
    light[0] = 200, light[1] = -200, light[2] = 300;
    double *sphere = malloc(sizeof(double) * 6);
    sphere[0] = 0; sphere[1] = 0; sphere[2] = 0;
    sphere[3] = 500; sphere[4] = 200; sphere[5] = 0x00FF0000;
    shapes[0] = sphere;
    double *plane = malloc(sizeof(double) * 6);
    plane[0] = 2; plane[1] = 1; plane[2] = 0;
    plane[3] = 0; plane[4] = -450; plane[5] = 0x00000FF;
    shapes[1] = plane;
    double *plane2 = malloc(sizeof(double) * 6);
    plane2[0] = 2; plane2[1] = 0; plane2[2] = 1;
    plane2[3] = 0; plane2[4] = 170; plane2[5] = 0x00FFFF00;
    shapes[2] = plane2;
    double *plane3 = malloc(sizeof(double) * 6);
    plane3[0] = 2; plane3[1] = 1; plane3[2] = 0;
    plane3[3] = 0; plane3[4] = 1000; plane3[5] = 0x00FF00FF;
    shapes[3] = plane3;
    return;
}

double light_level(double x, double y, double z, int i);

double *intersection(int x, int y, int i) { // include light level as well as coords
    double *ans = (double *)malloc(sizeof(double)*4);
    double depth;
    if (shapes[i][0] == 2) {
        if (shapes[i][1]*x+shapes[i][2]*y+shapes[i][3]*cam_dist == 0) return NULL;
        depth = cam_dist*(-shapes[i][1]*x-shapes[i][2]*y+shapes[i][4])/(shapes[i][1]*x+shapes[i][2]*y+shapes[i][3]*cam_dist);
    }
    if (shapes[i][0] == 0) {
        double a = shapes[i][1]-x;
        double b = shapes[i][2]-y;
        double c = shapes[i][3];
        double d = x;
        double e = y;
        double f = cam_dist;
        double r = shapes[i][4];
        double denom = d*d+e*e+f*f;
        if (denom == 0) {
            if (a*d+b*e-c*f == 0) return NULL;
            depth = f*(a*a+b*b+c*c-r*r)/(2*(a*d+b*e+c*f));
        } else {
            double helpful = a*d+b*e+c*f;
            double det = f*f*(pow(helpful, 2) - denom*(a*a+b*b+c*c-r*r));
            if (det < 0) return NULL;
            depth = (-sqrt(det)+helpful*f)/denom;
        }
    }
    ans[0] = (depth+cam_dist)*x/cam_dist; ans[1] = (depth+cam_dist)*y/cam_dist, ans[2] = depth;
    ans[3] = light_level(ans[0], ans[1], ans[2], i);
    return ans;
    return NULL;
}

double light_level(double x, double y, double z, int i) {
    int variablebeingmadebecauseofbaddatastructure;
    if (shapes[i][0] == 2) variablebeingmadebecauseofbaddatastructure = 6;
    if (shapes[i][0] == 0) variablebeingmadebecauseofbaddatastructure = 6;
    int rgb = shapes[i][variablebeingmadebecauseofbaddatastructure-1];
    double lev_light_unscaled;
    if (shapes[i][0] == 2) {
        double dist_from_plane = fabs(shapes[i][1]*light[0]+shapes[i][2]*light[1]+shapes[i][3]*light[2]-shapes[i][4])/sqrt(pow(shapes[i][1], 2)+pow(shapes[i][2], 2)+pow(shapes[i][3], 2));
        double dist_from_inter = sqrt(pow(x - light[0], 2) + pow(y - light[1], 2) + pow(z - light[2], 2));
        lev_light_unscaled = dist_from_plane/dist_from_inter < 1 ? dist_from_plane/dist_from_inter : 1;
    } else if (shapes[i][0] == 0) {
        double dist_from_center = sqrt(pow(shapes[i][1]-light[0], 2) + pow(shapes[i][2]-light[1], 2) + pow(shapes[i][3]-light[2], 2));
        //double rat = (dist_from_center-shapes[i][4])/dist_from_center;
        //double dist_from_opt = sqrt(pow(x-(shapes[i][1]-light[0])*rat+light[0], 2)+pow(y-(shapes[i][2]-light[1])*rat+light[1], 2)+pow(z-(shapes[i][3]-light[2])*rat+light[2], 2));
        double dist_from_inter = sqrt(pow(x - light[0], 2) + pow(y - light[1], 2) + pow(z - light[2], 2));
        double cos_180_a = (dist_from_inter*dist_from_inter+shapes[i][4]*shapes[i][4]-dist_from_center*dist_from_center)/(2*shapes[i][4]*dist_from_inter);
        //double cos_a = dist_from_opt/2/shapes[i][4];
        lev_light_unscaled = -cos_180_a;
        //printf("%f %f %f %f %f %f %f %f \n", x, y, z, dist_from_center, rat, dist_from_opt, cos_a, lev_light_unscaled);
    }
    double lev_light = asin(lev_light_unscaled)/3.14159266*2;
    // if (x == 0) printf("%f %f %f %i %i %i\n", x - light[0], y - light[1], z - light[2], light[0], light[1], light[2]);
    int b = rgb % 256, g = rgb / 256 % 256, r = rgb / 256 / 256 % 256;
    int new_rgb = round(r * lev_light) * 256 * 256 + round(g * lev_light) * 256 + round(b * lev_light);
    return new_rgb;
}

// x, y, z+cam_dist : p_x, p_y, cam_dist
// We're solving for z; x = (z+cam_dist)*p_x/cam_dist, y = (z+cam_dist)*p_y/cam_dist
// Sphere equation: ((z+f)*d/f-a)^2+((z+f)*e/f-b)^2+(z-c)^2=r^2
// (d/f*z+(d-a))^2+(e/f*z+(e-b))^2+(z-c)^2=r^2
// z_c z ?                  0              light_z
//  *--*-*------------------*------------------*
//  |----|-------------------------------------|
//    a                     b
// rat = b/(a+b) = (?-light_z)/(z_c-light_z) -> ?-light_z = rat*(z_c-light_z) -> ? = rat*(z_c-light_z)+light_z
// a+b = z_c+cam_dist

int eval_pixel(int x, int y) { // Note: assumed that all light reflected goes into POV
    double *closest_coords = NULL;
    for (int i = 0; i < num_shapes; i++) {
        int good = 0;
        double *inter_coords = intersection(x-mid_x, y-mid_y, i);
        if (inter_coords != NULL) {
            if (inter_coords[2] > 0) {
                if (closest_coords != NULL) {
                    if (inter_coords[2] < closest_coords[2]) {
                        good = 1;
                    }
                } else {
                    good = 1;
                }
                if (good) {
                    free(closest_coords);
                    closest_coords = inter_coords;
                } else {
                    free(inter_coords);
                }
            }
        }
    }
    if (closest_coords == NULL) {
        return 0x00000000;
    }
    return closest_coords[3];
}

void draw_screen() {
    for (int i = 0; i < scrx; i++) {
        for (int j = 0; j < scry; j++) {
            int color = eval_pixel(i, j);
            mlx_pixel_put(core, canvas, i, j, color);
        }
    }
    return;
}

int main() {
    core = mlx_init();
    canvas = mlx_new_window(core, scrx, scry, "rtv1");
    init_shapes();
    draw_screen();
    mlx_loop(core);
    return 0;
}
