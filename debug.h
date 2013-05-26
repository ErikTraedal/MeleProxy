/*
 * debug.h
 *
 *  Created on: 26 May 2013
 *      Author: santa
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define debug(...)
#endif

#endif /* DEBUG_H_ */
