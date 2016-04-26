#version 330 core
#define EXPLOSION 0
#define BLINKS 1
#define SNAKES 2
uniform sampler2D tex;
uniform int mode;
uniform vec2 spriteOffset;
uniform vec4 solidColor;
in vec2 v_texcoord;
in vec4 v_color;
out vec4 fragColor;

void main(void)
{
    vec4 surfaceColor;
    if(mode == EXPLOSION) {
        surfaceColor = texture(tex, (v_texcoord + spriteOffset) / 6.0) * v_color;
    } else if(mode == BLINKS) {
        surfaceColor = texture(tex, v_texcoord) * solidColor;
        surfaceColor.rgb += (0.5 - abs(v_texcoord.s - 0.5)) * 0.5;
        surfaceColor.rgb += (0.5 - abs(v_texcoord.t - 0.5)) * 0.5;
    } else if(mode == SNAKES) {
        surfaceColor = texture(tex, v_texcoord) * solidColor;
        surfaceColor.rgb += (0.5 - abs(v_texcoord.s - 0.5)) * 4.0 * solidColor.a * (1.0 - surfaceColor.rgb);
    }

    surfaceColor.rgb *= surfaceColor.a;
    fragColor = surfaceColor;
}
