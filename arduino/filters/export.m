function export(path, fir, iir = [])
    fid = fopen(path, 'w');
    is_complex = any(imag(fir) != 0) || any(imag(iir) != 0);

    % extract filename without extension as prefix
    [~, name, ~] = fileparts(path);
    prefix = toupper(name);

    fprintf(fid, "#pragma once\n\n");

    if is_complex
        fprintf(fid, "#include \"complex.hpp\"\n\n");
        dtype = "complex_t";
    else
        dtype = "float";
    end

    fprintf(fid, "constexpr size_t %s_FIR_SIZE = %d;\n", prefix, length(fir));
    fprintf(fid, "constexpr size_t %s_IIR_SIZE = %d;\n\n", prefix, length(iir));

    fprintf(fid, "constexpr %s %s_FIR_COEFFS[] = {", dtype, prefix);
    for k = 1:length(fir)
        if is_complex
            fprintf(fid, "complex_t(%.9ff, %.9ff)", real(fir(k)), imag(fir(k)));
        else
            fprintf(fid, "%.9ff", real(fir(k)));
        end
        if k < length(fir)
            fprintf(fid, ", ");
        end
    endfor
    fprintf(fid, "};\n\n");

    fprintf(fid, "constexpr %s %s_IIR_COEFFS[] = {", dtype, prefix);
    for k = 1:length(iir)
        if is_complex
            fprintf(fid, "complex_t(%.9ff, %.9ff)", real(iir(k)), imag(iir(k)));
        else
            fprintf(fid, "%.9ff", real(iir(k)));
        end
        if k < length(iir)
            fprintf(fid, ", ");
        end
    endfor
    fprintf(fid, "};\n");

    fclose(fid);
endfunction