/* version.h
**
** Copyright 2011, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#ifndef TINYALSA_VERSION_H
#define TINYALSA_VERSION_H

/* Macros for expanding the version numbers into string literals */
#define TINYALSA_VERSION_STR_EX(number) #number
#define TINYALSA_VERSION_STR(number) TINYALSA_VERSION_STR_EX (number)

#define TINYALSA_VERSION_MAJOR 2

#define TINYALSA_VERSION_MINOR 0

#define TINYALSA_VERSION_PATCH 0

/* The final version number is constructed based on minor, major and patch */
#define TINYALSA_VERSION \
    ((unsigned long) \
    ((TINYALSA_VERSION_MAJOR << 16)   | \
     (TINYALSA_VERSION_MINOR << 8 )   | \
     (TINYALSA_VERSION_PATCH      )))

/* The version string is constructed by concatenating individual ver. strings */
#define TINYALSA_VERSION_STRING \
    TINYALSA_VERSION_STR (TINYALSA_VERSION_MAJOR) \
    "." \
    TINYALSA_VERSION_STR (TINYALSA_VERSION_MINOR) \
    "." \
    TINYALSA_VERSION_STR (TINYALSA_VERSION_PATCH)

#endif /* TINYALSA_VERSION_H */

