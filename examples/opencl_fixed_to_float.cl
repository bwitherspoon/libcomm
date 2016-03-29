__kernel void fixed_to_float(__global short* restrict input,
                             __global float* restrict output)
{
    int i = get_global_id(0);

    output[i] = convert_float(input[i]) / pow(2.0f, 15.0f);
}

__kernel void fixed4_to_float4(__global short4* restrict input,
                               __global float4* restrict output)
{
    int i = get_global_id(0);

    output[i] = convert_float4(input[i]) / pow(2.0f, 15.0f);
}
