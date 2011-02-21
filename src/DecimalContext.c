#include "DecimalContext.h"

void DecimalContext_init(DecimalContext *context, uint32_t precision,
    DecimalRoundingDirection rounding, int32_t maxExponent)
{
    DecimalContext_setPrecision(context, precision);
    DecimalContext_setRounding(context, rounding);
    context->flags = 0;
    DecimalContext_setMaxExponent(context, maxExponent);
}
