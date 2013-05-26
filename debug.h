/*
 * debug.h
 *
 *  Created on: 26 May 2013
 *      Author: santa
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG
#define debug(...) do{ fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#else
#define debug(...) do{ } while ( 0 )
#endif

#endif /* DEBUG_H_ */
