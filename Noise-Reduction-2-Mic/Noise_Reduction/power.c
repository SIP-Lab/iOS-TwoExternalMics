/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: power.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "wienerAB.h"
#include "power.h"
#include "wienerAB_emxutil.h"

/* Function Definitions */

/*
 * Arguments    : const emxArray_real32_T *a
 *                emxArray_real32_T *y
 * Return Type  : void
 */
void b_power(const emxArray_real32_T *a, emxArray_real32_T *y)
{
  short iv0[2];
  int n;
  int k;
  for (n = 0; n < 2; n++) {
    iv0[n] = (short)a->size[n];
  }

  n = y->size[0] * y->size[1];
  y->size[0] = iv0[0];
  y->size[1] = iv0[1];
  emxEnsureCapacity((emxArray__common *)y, n, (int)sizeof(float));
  n = a->size[0] * a->size[1];
  for (k = 0; k + 1 <= n; k++) {
    y->data[k] = a->data[k] * a->data[k];
  }
}

/*
 * Arguments    : const float a[512]
 *                float y[512]
 * Return Type  : void
 */
void power(const float a[512], float y[512])
{
  int k;
  for (k = 0; k < 512; k++) {
    y[k] = a[k] * a[k];
  }
}

/*
 * File trailer for power.c
 *
 * [EOF]
 */
