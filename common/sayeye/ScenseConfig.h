/*
 * Copyright (C) 2014 ALLWINNERTECH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SCENSE_CONFIG_H__
#define __SCENSE_CONFIG_H__

/* app type define */
#define APP_TYPE_CPU        (0x00000001)
#define APP_TYPE_GPU        (0x00000002)
#define APP_TYPE_IO         (0x00000003)
#define APP_TYPE_CPU_GPU    (0x00000004)

/* Frameworks Tag define */
#define TAG_TP_SUSPEND      (0x00000001)

#define CPU_GOVERNOR        "interactive"

#ifdef SUN9IW1P1
#include "SUN9IW1P1.h"
#elif defined SUN8IW5P1 /* sun9iw1p1 define end */
#include "SUN8IW5P1.h"
#elif defined SUN8IW6P1 /* sun8iw5p1 define end */
#include "SUN8IW6P1.h"
#elif defined SUN50IW1P1 /* sun8iw6p1 define end */
#include "SUN50IW1P1.h"
#endif /* sun50iw1p1 define end */

#endif /* file define end */
