$input a_position

#include <bgfx_shader.sh>

uniform highp vec4 u_viewport;

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(vec3(0.0), 1.0)) + vec4(vec3(a_position.xy / u_viewport.xy * vec2(2.0), 0), 0.0);
}
