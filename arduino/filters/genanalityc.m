clear; clc; clear all; clf;
pkg load signal

# FIR1 analytic filter
len = 16;
dislocation = -0.1;
b = fir1(len, 0.5 - dislocation);
a = 1;

# Shift to bandpass via frequency-shift theorem
shift = (1 + dislocation) * (2*pi/4);
b = b .* exp(j * shift * (0:length(b)-1));

# Show filter
[H, w] = freqz(b, a, 4096, 'whole');
H = fftshift(H);
f = linspace(-0.5, 0.5, 4096);

plot(f, 20*log10(abs(H) + 1e-12));
grid on;
xlabel('Normalized Frequency');
ylabel('dB');
title('Analytic Filter Response');

# Export coefficients
export("analytic.hpp", b, []);
print('-dpng', 'analytic.png', '-r150');