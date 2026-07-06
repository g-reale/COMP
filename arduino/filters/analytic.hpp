#pragma once

#include "complex.hpp"

constexpr size_t ANALYTIC_FIR_SIZE = 17;
constexpr size_t ANALYTIC_IIR_SIZE = 0;

constexpr complex_t ANALYTIC_FIR_COEFFS[] = {complex_t(0.001927545f, 0.000000000f), complex_t(0.000465124f, 0.002936677f), complex_t(0.010331753f, -0.003356990f), complex_t(-0.000160865f, -0.000315715f), complex_t(0.032827953f, -0.023850904f), complex_t(-0.031877456f, -0.031877456f), complex_t(0.047031469f, -0.064733263f), complex_t(-0.259753667f, -0.132351104f), complex_t(0.184544079f, -0.567968274f), complex_t(0.287939158f, 0.045605082f), complex_t(-0.000000000f, -0.080014714f), complex_t(0.044526503f, -0.007052305f), complex_t(-0.012539162f, -0.038591573f), complex_t(0.000315715f, -0.000160865f), complex_t(-0.006385375f, -0.008788714f), complex_t(-0.002102429f, 0.002102429f), complex_t(-0.001559417f, -0.001132982f)};

constexpr complex_t ANALYTIC_IIR_COEFFS[] = {};
