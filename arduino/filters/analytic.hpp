#pragma once

#include "complex.hpp"

constexpr size_t ANALYTIC_FIR_SIZE = 33;
constexpr size_t ANALYTIC_IIR_SIZE = 0;

constexpr complex_t ANALYTIC_FIR_COEFFS[] = {complex_t(-0.000078241f, 0.000000000f), complex_t(-0.000000000f, -0.001886691f), complex_t(-0.000112487f, 0.000000000f), complex_t(-0.000000000f, -0.003860728f), complex_t(-0.000210010f, 0.000000000f), complex_t(-0.000000000f, -0.008239973f), complex_t(-0.000355964f, 0.000000000f), complex_t(-0.000000000f, -0.015944290f), complex_t(-0.000528129f, 0.000000000f), complex_t(-0.000000000f, -0.028674361f), complex_t(-0.000700293f, 0.000000000f), complex_t(-0.000000000f, -0.050718494f), complex_t(-0.000846247f, 0.000000000f), complex_t(0.000000000f, -0.098020697f), complex_t(-0.000943771f, 0.000000000f), complex_t(-0.000000000f, -0.315965134f), complex_t(0.499766377f, -0.000000000f), complex_t(-0.000000000f, 0.315965134f), complex_t(-0.000943771f, 0.000000000f), complex_t(0.000000000f, 0.098020697f), complex_t(-0.000846247f, 0.000000000f), complex_t(-0.000000000f, 0.050718494f), complex_t(-0.000700293f, 0.000000000f), complex_t(0.000000000f, 0.028674361f), complex_t(-0.000528129f, 0.000000000f), complex_t(-0.000000000f, 0.015944290f), complex_t(-0.000355964f, -0.000000000f), complex_t(0.000000000f, 0.008239973f), complex_t(-0.000210010f, 0.000000000f), complex_t(-0.000000000f, 0.003860728f), complex_t(-0.000112487f, 0.000000000f), complex_t(0.000000000f, 0.001886691f), complex_t(-0.000078241f, 0.000000000f)};

constexpr complex_t ANALYTIC_IIR_COEFFS[] = {};
