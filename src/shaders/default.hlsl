// vertex shader
struct vs_input {
    float3 position: POSITION;
};

struct vs_output {
    float4 position: SV_Position;
};

vs_output vs_main(vs_input input) {
    vs_output output = (vs_output) 0;
    output.position = float4(input.position, 1.0);

    return output;
}

// pixel shader
struct ps_input {
    float4 position: SV_Position;
};

struct ps_output {
	float4 color: SV_Target0;
};

ps_output ps_main(ps_input input) {
	ps_output output = (ps_output) 0;
	output.color = float4(1.0, 0.0, 0.0, 1.0);
	
	return output;
}