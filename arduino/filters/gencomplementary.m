clear; clc; clear all; clf;
pkg load signal;

len = 16;
lpf = fir1(len,0.65);
hpf = lpf .* cos(pi*(0:len));

[H_lpf, w] = freqz(lpf, 1, 4096);
[H_hpf, ~] = freqz(hpf, 1, 4096);

plot(w/(2*pi), 20*log10(abs(H_lpf)+1e-12), 'b'); hold on;
plot(w/(2*pi), 20*log10(abs(H_hpf)+1e-12), 'r');
hold off;
grid on;
legend('LPF','HPF');
xlabel('Normalized Frequency');
ylabel('dB');

export("lpf.hpp",lpf);
export("hpf.hpp",hpf);
print('-dpng', 'complementary.png', '-r150');

#{
clear; clc; clear all; clf;
pkg load signal;

[b_lpf, a_lpf] = cheby2(16, 40, 0.5);
[b_hpf, a_hpf] = cheby2(16, 40, 0.5, 'high');

[H_lpf, w] = freqz(b_lpf, a_lpf, 4096);
[H_hpf, ~] = freqz(b_hpf, a_hpf, 4096);

plot(w/(2*pi), 20*log10(abs(H_lpf)+1e-12), 'b'); hold on;
plot(w/(2*pi), 20*log10(abs(H_hpf)+1e-12), 'r');
hold off;
grid on;
legend('LPF','HPF');
xlabel('Normalized Frequency');
ylabel('dB');

export("lpf.hpp", b_lpf, a_lpf(2:end));
export("hpf.hpp", b_hpf, a_hpf(2:end));
print('-dpng', 'complementary.png', '-r150');
#}