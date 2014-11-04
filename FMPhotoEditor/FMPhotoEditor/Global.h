#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#define DIRPATH_FILENAME_LEN		1024

#define BLUE_MAX_LIMITE					10
#define BLUE_MIN_LIMITE					5

#define BLUE_LIMITE_MAX_VALUE			5
#define BLUE_LIMITE_MIN_VALUE			3

#define GREEN_MAX_LIMITE				100
#define GREEN_MIN_LIMITE				30

#define GREEN_LIMITE_MAX_VALUE			10
#define GREEN_LIMITE_MIN_VALUE			3

#define MAX_RGB_COLOR					255
#define RGB_COLOR_RANGE_MIN				100

#define THREE_RATIO						3
#define TWO_RATIO						2

#define MIDDLE_RGB_COLOR				126

#define RoundOff(x, dig)		(floor((x) * pow(10,dig) + 0.5) / pow(10,dig))
#define Lab_A							128
#define Lab_B							128


#define  EPSILON				0.008856
#define  KAPPA					903.3

#define  X_POSITION_RANGE		51

#define	 X_REFERENCE_POINT		50

#define  X_ARRAY_VALUE			(255/X_POSITION_RANGE) + 1

#define	 LABEL_1				0
#define	 LABEL_2				1
#define  LABEL_3				2
#define  LABEL_4				3
#define  LABEL_5			    4
#define  LABEL_6				5


#if 1		/* Najak 20141024 16:00 값 */
#define  ALPHA_LABEL_1			1.23
#define  ALPHA_LABEL_2			1.18
#define  ALPHA_LABEL_3			1.12		// 1.07
#define  ALPHA_LABEL_4			1.04		// 1.02
#define  ALPHA_LABEL_5			1.01
#define  ALPHA_LABEL_ZERO		0.0


#define  BETA_LABEL_1			4
#define  BETA_LABEL_2			5
#define  BETA_LABEL_3			4
#define  BETA_LABEL_4			4
#define  BETA_LABEL_5			4

#else

#if 1		/* Najak 20141023 18시 값*/
#define  ALPHA_LABEL_1			1.23
#define  ALPHA_LABEL_2			1.16
#define  ALPHA_LABEL_3			1.03
#define  ALPHA_LABEL_ZERO		0.0

#define  BETA_LABEL_1			6
#define  BETA_LABEL_2			8
#define  BETA_LABEL_3			8

#else			/* Najak 20141022 24시 값*/
#define  ALPHA_LABEL_1			1.25
#define  ALPHA_LABEL_2			1.18
#define  ALPHA_LABEL_3			1.05
#define  ALPHA_LABEL_ZERO		0.0

#define  BETA_LABEL_1			4
#define  BETA_LABEL_2			6
#define  BETA_LABEL_3			8
#endif
#endif
#endif