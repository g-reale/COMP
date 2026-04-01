clear; clc;

amplitude = (2^16)-1;
space = 2^13-1;
order = 4;
N = 2^16;

# Full domain (one full sine period: 2 domes)
x_full = linspace(0, 2*space, N);

# Split domains (local coordinates)
x1 = linspace(0, space, N/2);
x2 = linspace(0, space, N/2);

# Generate domes
y1 = amplitude/2 * sin((pi/space) * x1) + amplitude/2;
y2 = amplitude/2 * sin((pi/space) * (x2 + space)) + amplitude/2;

# Fit polynomials
p1 = polyfit(x1, y1, order);
p2 = polyfit(x2, y2, order);

# ---- Function to print polynomial in rational form ----
function r = print_poly(p, order)
    r = [];
    [n, d] = rat(p, 1e-20);

    for i = 1:length(p)
        k = order - (i-1);
        fits = abs(n(i)) < 2^37 && d(i) < 2^37;

        if fits
            str = sprintf("%d/%d", n(i), d(i));
            r = [r n(i)/d(i)];
        else
            str = "0";
            r = [r 0];
        endif

        if k > 1
            printf("%s*x^%d", str, k);
        elseif k == 1
            printf("%s*x", str);
        else
            printf("%s", str);
        endif

        if i < length(p)
            printf(" + ");
        endif
    endfor

    printf("\n");
endfunction

# ---- Print both polynomials ----
printf("Polynomial for dome 1:\n");
r1 = print_poly(p1, order);

printf("\nPolynomial for dome 2:\n");
r2 = print_poly(p2, order);

# ---- Piecewise reconstruction ----
y_approx = zeros(size(x_full));

mask1 = x_full <= space;
mask2 = x_full > space;

x1_eval = x_full(mask1);
x2_eval = x_full(mask2) - space;

y_approx(mask1) = polyval(r1, x1_eval);
y_approx(mask2) = polyval(r2, x2_eval);

# ---- Ground truth ----
y_true = amplitude/2 * sin((pi/space) * x_full) + amplitude/2;

# ---- Error ----
eps = min(y_approx);  
err_rel = abs(y_approx - y_true) ./ max(y_true, eps);

# ---- Plots ----
figure(1); clf;

subplot(2,1,1);
hold on;
plot(x_full, y_true, 'b');
plot(x_full, y_approx, 'r--');
legend("true", "approx");
title("Piecewise Polynomial Approximation");
grid on;

subplot(2,1,2);
plot(x_full, err_rel);
title("Relative Error");
grid on;

# ---- Print max error ----
printf("\nMax relative error: %f\n", max(err_rel));