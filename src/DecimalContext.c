#include "DecimalContext.h"

void DecimalContext_init(DecimalContext *context, uint32_t precision,
    DecimalRoundingDirection rounding)
{
    context->precision = precision;
    context->rounding = rounding;
    context->flags = 0;
}
