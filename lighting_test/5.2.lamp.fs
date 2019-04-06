#version 330 core
out vec4 FragColor;

in vec2 TexCoord;//纹理坐标

uniform sampler2D texture1;

void main()
{
    //FragColor = vec4(1.0); // 将向量的四个分量全部设置为1.0
    FragColor = texture(texture1, TexCoord);

}