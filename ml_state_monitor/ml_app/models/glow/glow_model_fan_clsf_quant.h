// Bundle API auto-generated header file. Do not edit!
// Glow Tools version: 2021-07-01 (057f691b9)

#ifndef _GLOW_BUNDLE_GLOW_MODEL_FAN_CLSF_QUANT_H
#define _GLOW_BUNDLE_GLOW_MODEL_FAN_CLSF_QUANT_H

#include <stdint.h>

// ---------------------------------------------------------------
//                       Common definitions
// ---------------------------------------------------------------
#ifndef _GLOW_BUNDLE_COMMON_DEFS
#define _GLOW_BUNDLE_COMMON_DEFS

// Glow bundle error code for correct execution.
#define GLOW_SUCCESS 0

// Memory alignment definition with given alignment size
// for static allocation of memory.
#define GLOW_MEM_ALIGN(size)  __attribute__((aligned(size)))

// Macro function to get the absolute address of a
// placeholder using the base address of the mutable
// weight buffer and placeholder offset definition.
#define GLOW_GET_ADDR(mutableBaseAddr, placeholderOff)  (((uint8_t*)(mutableBaseAddr)) + placeholderOff)

#endif

// ---------------------------------------------------------------
//                          Bundle API
// ---------------------------------------------------------------
// Model name: "glow_model_fan_clsf_quant"
// Total data size: 14656 (bytes)
// Placeholders:
//
//   Name: "acceleration"
//   Type: float<1 x 128 x 1 x 3>
//   Size: 384 (elements)
//   Size: 1536 (bytes)
//   Offset: 0 (bytes)
//
//   Name: "Identity"
//   Type: float<1 x 4>
//   Size: 4 (elements)
//   Size: 16 (bytes)
//   Offset: 1536 (bytes)
//
// NOTE: Placeholders are allocated within the "mutableWeight"
// buffer and are identified using an offset relative to base.
// ---------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

// Placeholder address offsets within mutable buffer (bytes).
#define GLOW_MODEL_FAN_CLSF_QUANT_acceleration  0
#define GLOW_MODEL_FAN_CLSF_QUANT_Identity      1536

// Memory sizes (bytes).
#define GLOW_MODEL_FAN_CLSF_QUANT_CONSTANT_MEM_SIZE     10880
#define GLOW_MODEL_FAN_CLSF_QUANT_MUTABLE_MEM_SIZE      1600
#define GLOW_MODEL_FAN_CLSF_QUANT_ACTIVATIONS_MEM_SIZE  2176

// Memory alignment (bytes).
#define GLOW_MODEL_FAN_CLSF_QUANT_MEM_ALIGN  64

// Bundle entry point (inference function). Returns 0
// for correct execution or some error code otherwise.
int glow_model_fan_clsf_quant(uint8_t *constantWeight, uint8_t *mutableWeight, uint8_t *activations);

#ifdef __cplusplus
}
#endif
#endif
