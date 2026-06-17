clear; clc; clear all; clf;
pkg load signal

# Constants (mirroring Constants.hpp)
channel_start = 100;
channel_end = 10000;
channel_length = channel_end - channel_start;
order = 32;
mic_passband_end = 0.423;
sample_rate = (channel_end + channel_start) / mic_passband_end;
resolution = channel_length / order;

first_symbol_hz = channel_start + resolution / 2;
first_symbol_norm = first_symbol_hz / sample_rate;

printf("Sample rate: %.2f Hz\n", sample_rate);
printf("Resolution: %.4f Hz\n", resolution);
printf("First symbol: %.4f Hz (norm=%.6f)\n", first_symbol_hz, first_symbol_norm);

# FIR1 analytic filter
len = 32;
dislocation = -0.0;
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

# Mark first symbol on plot
bin = round((first_symbol_norm + 0.5) * 4096);
bin = max(1, min(4096, bin));
magnitude_db = 20*log10(abs(H(bin)) + 1e-12);
printf("Magnitude at first symbol: %.2f dB\n", magnitude_db);

hold on;
plot(f(bin), magnitude_db, 'ro', 'MarkerSize', 8, 'DisplayName', sprintf('First symbol (%.1f Hz)', first_symbol_hz));
legend('Filter response', sprintf('First symbol (%.1f Hz) = %.2f dB', first_symbol_hz, magnitude_db));
hold off;

# Export coefficients
export("analytic.hpp", b, []);
print('-dpng', 'analytic.png', '-r150');