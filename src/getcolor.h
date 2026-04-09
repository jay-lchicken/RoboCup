#ifndef GETCOLOR_H
#define GETCOLOR_H
#include "definitions.h"
#include <Arduino.h>
#include <Evo.h>
#include <cmath>
#include <algorithm>

enum Color { COLOR_WHITE = 0, COLOR_RED = 1, COLOR_GREEN = 2, COLOR_BLACK = 3, COLOR_SILVER = 4 };

// Color thresholds (HSV ranges) - calibrate these for your environment
// Format: {h_min, h_max, s_min, s_max, v_min, v_max}
struct ColorThresholds {
    float white_h_min = 62, white_h_max = 72;
    float white_s_min = 26, white_s_max = 33;
    float white_v_min = 8, white_v_max = 10;
    
    float red_h_min1 = 0, red_h_max1 = 15;
    float red_h_min2 = 345, red_h_max2 = 360;
    float red_s_min = 30, red_s_max = 100;
    float red_v_min = 20, red_v_max = 100;
    
    float green_h_min = 98, green_h_max = 110;
    float green_s_min = 35000, green_s_max = 35; //lower s min if darker ori 37. increase s min if it senses black on green
    float green_v_min = 8, green_v_max = 12;
    
    float black_h_min = 86, black_h_max = 96;
    float black_s_min = 20, black_s_max = 25;
    float black_v_min = 7, black_v_max = 13;
};

ColorThresholds colorThresholds;

Color classifyColor(float h, float s, float v, int c, int r, int g, int b, ColorThresholds &thresholds) {
    if (h >= thresholds.red_h_min1 && h <= thresholds.red_h_max1 || h >= thresholds.red_h_min2 && h <= thresholds.red_h_max2) {
       return COLOR_RED;
    }

    if (r > 135 || g > 125 || b > 100){
        return COLOR_SILVER;
    }
    
    if ((h*s*v) >= thresholds.green_s_min ){ //h >= thresholds.green_h_min && 
        return COLOR_GREEN;
    }
    
    if (c > 110){
        return COLOR_WHITE;
    }   
    else{
        return COLOR_BLACK;
    }
    
}

void getColor(int lr, int lg, int lb, int lc, int rr, int rg, int rb, int rc, int *lcolor, int *rcolor){

    int rred=0, rgreen=0, rblue=0;
    int lred=0, lgreen=0, lblue=0;

    if (lc == 0) {
       lr = lg = lb = 0;
       return;
    }

    lred = int((float)lr / lc * 255.0);
    lgreen = int((float)lg / lc * 255.0);
    lblue = int((float)lb / lc * 255.0);

    float lrNorm = (float)lred / 1024.0f;
    float lgNorm = (float)lgreen / 1024.0f;
    float lbNorm = (float)lblue / 1024.0f;

    float lminVal = min(lrNorm, min(lgNorm, lbNorm));
    float lmaxVal = max(lrNorm, max(lgNorm, lbNorm));
    float ldelta = lmaxVal - lminVal;

    lv = lmaxVal * 100;

    if (lmaxVal != 0)
        ls = (ldelta / lmaxVal) * 100;
    else
    {
        ls = 0;
        lh = -1;
        return;
    }

    if (ldelta == 0)
        lh = 0;
    else if (lmaxVal == lrNorm)
        lh = 60 * (fmod(((lgNorm - lbNorm) / ldelta), 6));
    else if (lmaxVal == lgNorm)
        lh = 60 * (((lbNorm - lrNorm) / ldelta) + 2);
    else if (lmaxVal == lbNorm)
        lh = 60 * (((lrNorm - lgNorm) / ldelta) + 4);

    if (lh < 0)
        lh += 360;

    if (rc == 0) {
       rr = rg = rb = 0;
       return;
    }

    rred = int((float)rr / rc * 255.0);
    rgreen = int((float)rg / rc * 255.0);
    rblue = int((float)rb / rc * 255.0);

    float rrNorm = rred / 1024.0f;
    float rgNorm = rgreen / 1024.0f;
    float rbNorm = rblue / 1024.0f;

    float rminVal = min(rrNorm, min(rgNorm, rbNorm));
    float rmaxVal = max(rrNorm, max(rgNorm, rbNorm));
    float rdelta = rmaxVal - rminVal;

    rv = rmaxVal * 100;

    if (rmaxVal != 0)
        rs = (rdelta / rmaxVal) * 100;
    else
    {
        rs = 0;
        rh = -1;
        return;
    }

    if (rdelta == 0)
        rh = 0;
    else if (rmaxVal == rrNorm)
        rh = 60 * (fmod(((rgNorm - rbNorm) / rdelta), 6));
    else if (rmaxVal == rgNorm)
        rh = 60 * (((rbNorm - rrNorm) / rdelta) + 2);
    else if (rmaxVal == rbNorm)
        rh = 60 * (((rrNorm - rgNorm) / rdelta) + 4);

    if (rh < 0)
        rh += 360;

    Color leftColor = classifyColor(lh, ls, lv, map(lc, linputmin, linputmax, outputMin, outputMax), lr, lg, lb, colorThresholds);
    Color rightColor = classifyColor(rh, rs, rv, map(rc, rinputmin, rinputmax, outputMin, outputMax), rr, rg, rb, colorThresholds);

    *lcolor = static_cast<int>(leftColor);
    *rcolor = static_cast<int>(rightColor);
    // evo.clearDisplay();
    // evo.writeToDisplay(int(lr), 0, 0);
    // evo.writeToDisplay(int(lg), 32, 0);
    // evo.writeToDisplay(int(lb), 64, 0);
    // evo.writeToDisplay(int(rr), 0, 8);
    // evo.writeToDisplay(int(rg), 32, 8);
    // evo.writeToDisplay(int(rb), 64, 8);
    // evo.writeToDisplay(int(lh), 0, 16);
    // evo.writeToDisplay(int(ls), 32, 16);
    // evo.writeToDisplay(int(lv), 64, 16);
    // evo.writeToDisplay(int(rh), 0, 24);
    // evo.writeToDisplay(int(rs), 32, 24);
    // evo.writeToDisplay(int(rv), 64, 24);
    // evo.writeToDisplay(leftColor, 0, 32);
    // evo.writeToDisplay(rightColor, 32, 32);
    // evo.writeToDisplay(evo.getBattery(), 96, 0);
    // evo.writeToDisplay(int(lh*ls*lv), 0, 40);
    // evo.writeToDisplay(int(rh*rs*rv), 0, 48);
    // evo.drawDisplay();
}

#endif