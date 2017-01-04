/*
 * Copyright (C) 2014 STMicroelectronics
 * Author:	Denis Ciocca - <denis.ciocca@st.com>
 */

#include <errno.h>
#include <math.h>
#include <string.h>

#include "iNemoEngine_gbias_API.h"
#include "iNemoEngine_gbias_estimation.h"

#define GRAVITY_EARTH			(9.80665f)
#define NS_TO_HZ(x)			(1000000000.0f / x)
#define RAD2DEG(rad)			(rad * 180.0f / M_PI)
#define DEG2RAD(deg)			(deg * M_PI / 180.0f)

static struct iNemoEngineGBiasData {
	iNemoGbiasInitData init_data;
	float current_gbias[3];
	unsigned int frequency;
	float accel_data[3];
	bool status;
} iNemoEngineGBiasData = {
	.init_data = {
		.Gbias_threshold_accel = 0.3f,
		.Gbias_threshold_gyro = 0.4f,
		.accel_max_range = 19.6f,
		.gyro_max_range = 40.0f,
		.gbias_file = "/data/iNemoEngine_gbias.dat",
	},
	.current_gbias = { 0, 0, 0 },
	.frequency = 100,
	.accel_data = { 0, 0, 0 },
};

static struct data_output {
	int bias_valid;
	float gbias[3];
} data_output = {
	.bias_valid = 0,
};

static void load_gbias()
{
	int err;
	FILE *f;

	if (iNemoEngineGBiasData.init_data.gbias_file == NULL)
		return;

	f = fopen(iNemoEngineGBiasData.init_data.gbias_file, "r");
	if (f == NULL) {
		iNemoEngineGBiasData.current_gbias[0] = 0.0f;
		iNemoEngineGBiasData.current_gbias[1] = 0.0f;
		iNemoEngineGBiasData.current_gbias[2] = 0.0f;
		return;
	}

	err = fread(iNemoEngineGBiasData.current_gbias, 3 * sizeof(float), 1, f);
	if (err <= 0) {
		iNemoEngineGBiasData.current_gbias[0] = 0.0f;
		iNemoEngineGBiasData.current_gbias[1] = 0.0f;
		iNemoEngineGBiasData.current_gbias[2] = 0.0f;
	}

	fclose(f);

	return;
}

static void save_gbias()
{
	FILE *f;

	if (iNemoEngineGBiasData.init_data.gbias_file == NULL)
		return;

	f = fopen(iNemoEngineGBiasData.init_data.gbias_file, "w+");
	if (f == NULL)
		return;

	iNemoEngine_gbias_getGB(iNemoEngineGBiasData.current_gbias);
	fwrite(iNemoEngineGBiasData.current_gbias, 3 * sizeof(float), 1, f);

	fclose(f);
}

void iNemoEngine_API_gbias_Initialization(iNemoGbiasInitData *init_data)
{
	data_output.bias_valid = 0;

	if (init_data != NULL) {
		if (init_data->accel_max_range <= 0)
			init_data->accel_max_range =
				iNemoEngineGBiasData.init_data.accel_max_range;

		if (init_data->gyro_max_range <= 0)
			init_data->gyro_max_range =
				iNemoEngineGBiasData.init_data.gyro_max_range;

		if (init_data->gbias_file == NULL)
			init_data->gbias_file = "/data/iNemoEngine_gbias.dat";

		memcpy(&iNemoEngineGBiasData.init_data, init_data,
					sizeof(iNemoEngineGBiasData.init_data));
	}

	iNemoEngine_gbias_init(iNemoEngineGBiasData.init_data.Gbias_threshold_accel,
				iNemoEngineGBiasData.init_data.Gbias_threshold_gyro,
				1.0f,
				RAD2DEG(iNemoEngineGBiasData.init_data.gyro_max_range),
				iNemoEngineGBiasData.init_data.accel_max_range / GRAVITY_EARTH,
				0.001,
				100,
				0);

	load_gbias();

	iNemoEngineGBiasData.status = false;
	iNemoEngine_gbias_setInitGB(iNemoEngineGBiasData.current_gbias);

	return;
}

void iNemoEngine_API_gbias_enable(bool enable)
{
	if (!enable) {
		if (iNemoEngineGBiasData.status)
			save_gbias();

		iNemoEngineGBiasData.status = false;
		data_output.bias_valid = 0;
	} else
		iNemoEngineGBiasData.status = true;
}

void iNemoEngine_API_gbias_set_frequency(unsigned int frequency)
{
	if (frequency > 0) {
		iNemoEngine_gbias_setFrequency(frequency);
		iNemoEngineGBiasData.frequency = frequency;
	}
}

void iNemoEngine_API_set_accel_data(float *accel_data)
{
	memcpy(iNemoEngineGBiasData.accel_data, accel_data, 3 * sizeof(float));
}

void iNemoEngine_API_gbias_Run(float *gyro_data)
{
	float gyro_deg[3];

	if (!iNemoEngineGBiasData.status)
		return;

	iNemoEngineGBiasData.accel_data[0] /= GRAVITY_EARTH;
	iNemoEngineGBiasData.accel_data[1] /= GRAVITY_EARTH;
	iNemoEngineGBiasData.accel_data[2] /= GRAVITY_EARTH;

	gyro_deg[0] = RAD2DEG(gyro_data[0]);
	gyro_deg[1] = RAD2DEG(gyro_data[1]);
	gyro_deg[2] = RAD2DEG(gyro_data[2]);

	iNemoEngine_gbias_calc(iNemoEngineGBiasData.accel_data, gyro_deg,
				data_output.gbias, &data_output.bias_valid);
}

int iNemoEngine_API_Get_gbias(float *gbias_out)
{
	if (!iNemoEngineGBiasData.status)
		return -EINVAL;

	iNemoEngine_gbias_getGB(gbias_out);

	gbias_out[0] = DEG2RAD(gbias_out[0]);
	gbias_out[1] = DEG2RAD(gbias_out[1]);
	gbias_out[2] = DEG2RAD(gbias_out[2]);

	return 0;
}
