#version 330 core
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;
uniform sampler2D tex5;
uniform float offset;
in vec2 v_texcoord;
in vec4 v_color;
in vec3 v_normal;
out vec4 fragColor;

void main(void)
{
    vec2 texCoord = v_texcoord;

    texCoord.x += offset;

    float texColor = (texture(tex1, texCoord).r + texture(tex2, texCoord).r
                    + texture(tex3, texCoord).r + texture(tex4, texCoord).r + texture(tex5, texCoord).r) / 2.3;

    if(texCoord.y < 0.5)
        texColor *= texCoord.y * 2;

    vec4 surfaceColor = vec4(texColor, texColor, texColor, texColor) * v_color;
    surfaceColor.rgb *= surfaceColor.a;
    fragColor = surfaceColor;
}
