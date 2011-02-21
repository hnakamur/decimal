#ifndef _DECIMAL_CONTEXT_H_
#define _DECIMAL_CONTEXT_H_

#include <stdint.h>
#include "bool.h"

typedef enum {
    Decimal_RoundTiesToEven,
    Decimal_RoundTiesToAway,
    Decimal_RoundTowardPositive,
    Decimal_RoundTowardNegative,
    Decimal_RoundTowardZero
} DecimalRoundingDirection;

#define Decimal_Inexact          0x00000001
#define Decimal_Overflow         0x00000002
#define Decimal_Underflow        0x00000004
#define Decimal_DivisionByZero   0x00000008
#define Decimal_InvalidOperation 0x00000010
#define Decimal_AllFlags         0x0000001F

typedef struct {
    uint32_t precision;
    DecimalRoundingDirection rounding;
    uint32_t flags;
} DecimalContext;

#define DecimalContext_getPrecision(c) ((c)->precision)
#define DecimalContext_setPrecision(c, p) ((c)->precision = (p))
#define DecimalContext_getRounding(c) ((c)->rounding)
#define DecimalContext_setRounding(c, r) ((c)->rounding = (r))

#define DecimalContext_lowerFlags(c, exceptionGroup) \
    ((c)->flags &= ~(exceptionGroup))
#define DecimalContext_raiseFlags(c, exceptionGroup) \
    ((c)->flags |= (exceptionGroup))
#define DecimalContext_testFlags(c, exceptionGroup) \
    ((c)->flags & (exceptionGroup) ? true : false)
#define DecimalContext_testSavedFlags(flags, exceptionGroup) \
    ((flags) & (exceptionGroup) ? true : false)
#define DecimalContext_restoreFlags(c, f, exceptionGroup) \
    DecimalContext_raiseFlags(c, (f) & (exceptionGroup))
#define DecimalContext_saveAllFlags(c) ((c)->flags)

void DecimalContext_init(DecimalContext *context, uint32_t precision,
    DecimalRoundingDirection rounding);

#endif /* _DECIMAL_CONTEXT_H_ */
