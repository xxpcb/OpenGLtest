//片元着色器代码
#version 330 core

out vec4 FragColor;
out vec4 FragColor2;

in vec2 TexCoord;
in vec3 ourColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
//uniform float factor;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    FragColor2 = vec4(ourColor, 1.0);
}