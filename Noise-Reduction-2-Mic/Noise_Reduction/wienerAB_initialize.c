/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: wienerAB_initialize.c
 *
 * MATLAB Coder version            : 3.2
 * C/C++ source code generated on  : 19-Jun-2017 17:45:48
 */

/* Include Files */
#include "rt_nonfinite.h"
#include "wienerAB.h"
#include "wienerAB_initialize.h"

/* Function Definitions */

/*
 * Arguments    : void
 * Return Type  : void
 */
void wienerAB_initialize(void)
{
  rt_InitInfAndNaN(8U);
  wienerAB_init();
}

/*
 * File trailer for wienerAB_initialize.c
 *
 * [EOF]
 */
