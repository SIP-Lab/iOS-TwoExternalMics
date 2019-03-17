/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: bluestein_setup.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "wienerAB.h"
#include "bluestein_setup.h"

/* Function Definitions */

/*
 * Arguments    : creal32_T wwc[799]
 * Return Type  : void
 */
void bluestein_setup(creal32_T wwc[799])
{
  int idx;
  int rt;
  int k;
  int y;
  float nt_im;
  float nt_re;
  idx = 398;
  rt = 0;
  wwc[399].re = 1.0F;
  wwc[399].im = 0.0F;
  for (k = 0; k < 399; k++) {
    y = ((k + 1) << 1) - 1;
    if (800 - rt <= y) {
      rt = (y + rt) - 800;
    } else {
      rt += y;
    }

    nt_im = -3.14159274F * (float)rt / 400.0F;
    if (nt_im == 0.0F) {
      nt_re = 1.0F;
      nt_im = 0.0F;
    } else {
      nt_re = (float)cos(nt_im);
      nt_im = (float)sin(nt_im);
    }

    wwc[idx].re = nt_re;
    wwc[idx].im = -nt_im;
    idx--;
  }

  idx = 0;
  for (k = 398; k >= 0; k += -1) {
    wwc[k + 400] = wwc[idx];
    idx++;
  }
}

/*
 * File trailer for bluestein_setup.c
 *
 * [EOF]
 */
