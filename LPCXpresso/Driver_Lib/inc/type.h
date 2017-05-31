/****************************************************************************
 *   $Id: type.h 48757 2011-11-14 10:01:56Z anderslu $
 *   Project: NXP LPC11xx software example
 *
 *   Description:
 *     This file contains different type definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
****************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#define __inline  inline
#endif  /* __TYPE_H__ */

#undef _BIT
/* Set bit macro */
#define _BIT(n) (1 << (n))
