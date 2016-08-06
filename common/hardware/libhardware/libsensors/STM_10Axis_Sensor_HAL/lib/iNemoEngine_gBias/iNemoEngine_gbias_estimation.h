/*
 * Copyright (C) 2013 STMicroelectronics
 */

#ifndef INEMOENGINE_GBIAS_ESTIMATION_H
#define INEMOENGINE_GBIAS_ESTIMATION_H

void iNemoEngine_gbias_init(float accThr, float gyroThr, float scaleThr, float max_g_val, float max_acc_val_in, float k_in, float freq, int fast_start);
void iNemoEngine_gbias_setInitGB(float *gbias_in);
void iNemoEngine_gbias_calc(float *acc, float *gyro, float *gbias_out, int* bias_meas);
void iNemoEngine_gbias_getGB(float *gbias_in);
void iNemoEngine_gbias_setFrequency(float frequency);
int iNemoEngine_gbias_libVer(char *verstr);

#endif /* INEMOENGINE_GBIAS_ESTIMATION_H */
