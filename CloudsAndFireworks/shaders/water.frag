#version 330 core
uniform sampler2D tex;
uniform float angle;
in vec2 v_texcoord;
in vec4 v_color;
out vec4 fragColor;

void main(void)
{
    float wave = 0.002;
    float wave_x = v_texcoord.x + wave * sin(radians(angle + v_texcoord.x * 360.0) + gl_FragCoord.x / 10.0);
    float wave_y = v_texcoord.y + wave * sin(radians(angle + v_texcoord.y * 360.0));

    vec4 texColor = texture(tex, vec2(wave_x, wave_y)) * v_color;
    texColor.rgb *= texColor.a;

    fragColor = texColor;

}
