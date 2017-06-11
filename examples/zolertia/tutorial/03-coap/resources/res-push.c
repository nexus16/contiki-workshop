/*
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *      Example resource
 * \author
 *      Matthias Kovatsch <kovatsch@inf.ethz.ch>
 */

#include <string.h>
#include "rest-engine.h"
#include "er-coap.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_periodic_handler(void);

PERIODIC_RESOURCE(res_push,
                  "title=\"Periodic demo\";obs",
                  res_get_handler,
                  NULL,
                  NULL,
                  NULL,
                  5 * CLOCK_SECOND,
                  res_periodic_handler);

/*
 * Use local resource state that is accessed by res_get_handler() and altered by res_periodic_handler() or PUT or POST.
 */
static int32_t event_counter = 0;
static const unsigned short TEMPERATURE[300] = { 2016, 1999, 1919, 1945, 1966,
        1959, 2056, 2073, 1984, 1946, 1949, 1957, 1900, 1994, 2059, 2028, 2056,
        1902, 1926, 1932, 2063, 2019, 1970, 1901, 2080, 1997, 2087, 1918, 1903,
        2096, 2071, 2084, 2023, 1947, 1985, 1977, 1980, 1933, 1976, 2004, 2033,
        2033, 1905, 1991, 2011, 2060, 2082, 1998, 1910, 2005, 1985, 1913, 1942,
        2000, 2058, 2031, 2025, 2088, 2033, 2028, 2030, 1988, 1902, 1915, 1932,
        1940, 1909, 1908, 2029, 2057, 2038, 2060, 2047, 2073, 2078, 1997, 2084,
        1959, 1913, 1953, 2063, 1996, 2003, 2040, 2080, 1973, 1958, 1986, 1940,
        2061, 1995, 2073, 1979, 1963, 1972, 2023, 1948, 2034, 2006, 1990, 2015,
        1953, 2097, 1926, 1909, 2054, 2069, 2095, 1977, 2087, 1900, 2068, 1975,
        1997, 2019, 2001, 1911, 1974, 2083, 2007, 2027, 2048, 2011, 2004, 1940,
        1925, 2000, 1910, 2073, 1949, 2013, 2065, 1904, 1991, 1919, 2097, 2082,
        1978, 2039, 1999, 1980, 1939, 1904, 2073, 2096, 2050, 2003, 1915, 2056,
        2054, 2016, 1957, 1922, 2081, 2015, 1988, 1924, 1991, 1941, 1910, 2072,
        1997, 1984, 2072, 1974, 1902, 2019, 1994, 1950, 2013, 2007, 1952, 1904,
        1995, 1958, 2074, 2086, 2047, 2072, 2007, 1980, 2093, 1998, 2002, 2022,
        2082, 1952, 2070, 1962, 1931, 1961, 2099, 2077, 2039, 1949, 2080, 1921,
        1976, 1913, 2038, 1984, 2099, 1925, 1971, 2006, 1931, 2083, 2033, 2048,
        2066, 1923, 2021, 2082, 2078, 1994, 1910, 1932, 2042, 2091, 2059, 2083,
        2023, 2033, 1988, 2032, 1908, 2083, 2039, 2062, 1914, 2056, 1990, 2065,
        1933, 1909, 1999, 1986, 2091, 1935, 1917, 1951, 2094, 1985, 2090, 2025,
        2040, 1928, 2076, 1900, 2063, 1988, 2096, 1996, 2021, 2089, 1909, 2073,
        1940, 1995, 2075, 1914, 2057, 2068, 1916, 1957, 2027, 1968, 2053, 2002,
        2098, 1936, 1953, 2042, 2086, 2098, 1963, 2029, 1915, 2064, 2024, 2071,
        2009, 1986, 1920, 1949, 1912, 1922, 1974, 2080, 1990, 2056, 1997, 1950,
        2066, 2066, 2005, 1931, 2003, 1972, 2049 };
static const unsigned short LIGHT[300] = { 4359, 4449, 4468, 4314, 4333, 4335,
        4470, 4331, 4425, 4300, 4370, 4439, 4381, 4336, 4309, 4332, 4446, 4327,
        4401, 4374, 4325, 4331, 4417, 4318, 4356, 4365, 4394, 4458, 4400, 4459,
        4399, 4378, 4419, 4358, 4479, 4335, 4375, 4418, 4435, 4409, 4414, 4347,
        4315, 4334, 4414, 4453, 4385, 4336, 4459, 4352, 4478, 4358, 4373, 4372,
        4376, 4321, 4331, 4478, 4354, 4357, 4386, 4459, 4411, 4356, 4368, 4313,
        4491, 4407, 4409, 4452, 4381, 4433, 4471, 4312, 4354, 4337, 4311, 4425,
        4309, 4472, 4369, 4376, 4353, 4460, 4487, 4415, 4450, 4432, 4411, 4419,
        4421, 4431, 4482, 4318, 4406, 4492, 4495, 4359, 4446, 4378, 4359, 4449,
        4468, 4314, 4333, 4335, 4470, 4331, 4425, 4300, 4370, 4439, 4381, 4336,
        4309, 4332, 4446, 4327, 4401, 4374, 4325, 4331, 4417, 4318, 4356, 4365,
        4394, 4458, 4400, 4459, 4399, 4378, 4419, 4358, 4479, 4335, 4375, 4418,
        4435, 4409, 4414, 4347, 4315, 4334, 4414, 4453, 4385, 4336, 4459, 4352,
        4478, 4358, 4373, 4372, 4376, 4321, 4331, 4478, 4354, 4357, 4386, 4459,
        4411, 4356, 4368, 4313, 4491, 4407, 4409, 4452, 4381, 4433, 4471, 4312,
        4354, 4337, 4311, 4425, 4309, 4472, 4369, 4376, 4353, 4460, 4487, 4415,
        4450, 4432, 4411, 4419, 4421, 4431, 4482, 4318, 4406, 4492, 4495, 4359,
        4446, 4378, 4359, 4449, 4468, 4314, 4333, 4335, 4470, 4331, 4425, 4301,
        4370, 4439, 4381, 4336, 4309, 4332, 4446, 4327, 4401, 4374, 4325, 4331,
        4417, 4318, 4356, 4365, 4394, 4458, 4400, 4459, 4399, 4378, 4419, 4358,
        4479, 4335, 4375, 4418, 4435, 4409, 4414, 4347, 4315, 4334, 4414, 4453,
        4385, 4336, 4459, 4352, 4478, 4358, 4373, 4372, 4376, 4321, 4331, 4478,
        4354, 4357, 4386, 4459, 4411, 4356, 4368, 4313, 4491, 4407, 4409, 4452,
        4381, 4433, 4471, 4312, 4354, 4337, 4311, 4425, 4309, 4472, 4369, 4376,
        4353, 4460, 4487, 4415, 4450, 4432, 4411, 4419, 4421, 4431, 4482, 4318,
        4406, 4492, 4495, 4359, 4446, 4378 };
static const unsigned short MOISUTRE[300] = { 3762, 3733, 3716, 3866, 3871,
        3727, 3845, 3765, 3774, 3798, 3700, 3756, 3881, 3830, 3883, 3787, 3813,
        3801, 3877, 3770, 3798, 3779, 3828, 3835, 3776, 3739, 3849, 3861, 3788,
        3723, 3837, 3764, 3740, 3818, 3862, 3832, 3807, 3897, 3865, 3806, 3863,
        3857, 3790, 3863, 3714, 3784, 3781, 3876, 3803, 3708, 3785, 3762, 3755,
        3853, 3757, 3714, 3820, 3733, 3819, 3727, 3843, 3792, 3765, 3762, 3756,
        3723, 3841, 3863, 3704, 3845, 3722, 3855, 3831, 3786, 3748, 3766, 3707,
        3771, 3768, 3783, 3883, 3826, 3895, 3792, 3821, 3723, 3734, 3826, 3795,
        3856, 3743, 3803, 3725, 3774, 3713, 3749, 3711, 3800, 3710, 3743, 3732,
        3703, 3762, 3701, 3702, 3795, 3829, 3738, 3870, 3787, 3702, 3827, 3798,
        3790, 3774, 3820, 3749, 3720, 3860, 3750, 3784, 3882, 3771, 3709, 3850,
        3725, 3799, 3833, 3727, 3819, 3750, 3764, 3821, 3871, 3878, 3893, 3859,
        3761, 3728, 3834, 3741, 3751, 3881, 3814, 3872, 3739, 3741, 3756, 3822,
        3852, 3721, 3741, 3765, 3722, 3793, 3816, 3763, 3850, 3809, 3712, 3896,
        3882, 3898, 3722, 3745, 3725, 3711, 3711, 3802, 3831, 3791, 3842, 3823,
        3822, 3894, 3862, 3838, 3718, 3807, 3801, 3773, 3770, 3793, 3743, 3786,
        3728, 3720, 3881, 3815, 3894, 3792, 3819, 3703, 3894, 3855, 3834, 3794,
        3885, 3746, 3856, 3740, 3722, 3831, 3751, 3732, 3729, 3797, 3848, 3889,
        3707, 3702, 3823, 3889, 3785, 3844, 3858, 3839, 3703, 3743, 3829, 3791,
        3863, 3706, 3860, 3814, 3863, 3767, 3777, 3719, 3710, 3890, 3726, 3707,
        3889, 3861, 3838, 3824, 3859, 3806, 3802, 3714, 3791, 3740, 3804, 3712,
        3849, 3802, 3878, 3856, 3779, 3899, 3746, 3831, 3798, 3891, 3739, 3850,
        3739, 3817, 3743, 3785, 3802, 3810, 3889, 3839, 3788, 3749, 3879, 3879,
        3782, 3899, 3741, 3781, 3819, 3715, 3838, 3820, 3731, 3835, 3777, 3749,
        3894, 3765, 3777, 3866, 3759, 3731, 3729, 3750, 3773, 3875, 3715, 3876,
        3873, 3855, 3825, 3726, 3835, 3885, 3812};
//static int data_send;
//static int temperatureValue;
static unsigned short temperature = 0;
static short temperature_tmp = 0;
//static int lightValue;
static unsigned short light = 0;
static short light_tmp = 0;
static unsigned short moisutre = 0;
static int moisutre_tmp = 0;
static short flag = 0;

//static int temperatureArray[5];
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  /*
   * For minimal complexity, request query and options should be ignored for GET on observable resources.
   * Otherwise the requests must be stored with the observer list and passed by REST.notify_subscribers().
   * This would be a TODO in the corresponding files in contiki/apps/erbium/!
   */



  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_header_max_age(response, res_push.periodic->period / CLOCK_SECOND);
  REST.set_response_payload(response, buffer, snprintf((char *)buffer, preferred_size, "%d|%d|%d|%d",flag,temperature_tmp,light_tmp,moisutre_tmp));

  /* The REST.subscription_handler() will be called for observable resources by the REST framework. */
}
/*
 * Additionally, a handler function named [resource name]_handler must be implemented for each PERIODIC_RESOURCE.
 * It will be called by the REST manager process with the defined period.
 */
static void
res_periodic_handler()
{
  /* Do a periodic task here, e.g., sampling a sensor. */
	const unsigned short light_overage = 4400;
	const unsigned short temperature_overage = 2000;
	const unsigned short moisutre_overage = 3800;
	 short light_flag;
	 short temperature_flag;
	 short moisutre_flag;

	temperature = TEMPERATURE[event_counter];
	light = LIGHT[event_counter];
	moisutre = MOISUTRE[event_counter];
	++event_counter;

	temperature_tmp = temperature - temperature_overage;
	light_tmp = light - light_overage;
	moisutre_tmp = moisutre - moisutre_overage;

	temperature_flag = (temperature_tmp > 0)?1:0;
	light_flag = (light_tmp > 0)?1:0;
	moisutre_flag = (moisutre_tmp > 0)?1:0;
	flag = temperature_flag*2*2 + light_flag*2 + moisutre_flag;
	temperature_tmp = abs(temperature - temperature_overage);
	light_tmp = abs(light - light_overage);
	moisutre_tmp = abs(moisutre - moisutre_overage);

		REST.notify_subscribers(&res_push);


  /* Usually a condition is defined under with subscribers are notified, e.g., large enough delta in sensor reading. */
//  if(event_counter<6) {
//    /* Notify the registered observers which will trigger the res_get_handler to create the response. */
//
//
//	  temperatureArray[event_counter-1] = temperatureValue;
//	  data_send = temperatureValue;
//    REST.notify_subscribers(&res_push);
//  }
//  else {
//	  temperaturePredict = 0.6 * temperatureArray[4] + 0.2 * temperatureArray[3] + 0.1 * temperatureArray[2] + 0.05 * temperatureArray[1] + 0.05 * temperatureArray[0];
//	  data_send = temperaturePredict - temperatureValue;
//	  for(i=0;i<4;i++){
//		  temperatureArray[i]=temperatureArray[i+1];
//	  }
//	  temperatureArray[4]= temperatureValue;
//
//  }
}
