levels = 64;
amplitude = 2^16 - 1;
delta = 2*pi/(levels-1);
x = 0:delta:2*pi;
y = round(amplitude * (sin(x) + 1) / 2);

for k = 1:length(y)
    fprintf('tone[%d] = %d;\n', k-1, y(k));
end