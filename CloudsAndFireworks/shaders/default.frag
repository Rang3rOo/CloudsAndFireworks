#version 330 core
#define COLOR_SOLID 0
#define COLOR_VERTEX 1
#define COLOR_TEXTURE 2
#define COLOR_TINTED 3
uniform sampler2D tex;
uniform int colorMode;
uniform vec4 solidColor;
in vec2 v_texcoord;
in vec4 v_color;
out vec4 fragColor;

void main(void)
{
    vec4 surfaceColor;
    if (colorMode == COLOR_SOLID) {
        surfaceColor = solidColor;
    } else if (colorMode == COLOR_VERTEX) {
        surfaceColor = v_color;
    } else if (colorMode == COLOR_TEXTURE) {
        surfaceColor = texture(tex, v_texcoord) * v_color;
    } else if (colorMode == COLOR_TINTED) {
        surfaceColor = texture(tex, v_texcoord) * solidColor;
    }

    surfaceColor.rgb *= surfaceColor.a;
    fragColor = surfaceColor;
}
