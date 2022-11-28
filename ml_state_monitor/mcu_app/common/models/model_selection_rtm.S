#include "model_selection.h"

	.section .rodata

	.global model_rtm_start
	.global model_rtm_end

	.align 8
model_rtm_start:
#if RTM_MODEL_QUANTIZED_EN
    .incbin "model_fan_clsf_quant.rtm"
#else
    .incbin "model_fan_clsf.rtm"
#endif
model_rtm_end:
