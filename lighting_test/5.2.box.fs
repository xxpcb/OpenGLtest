#version 330 core

struct Light {
    vec3 position; 
    vec3  direction;
    float cutOff;
    float outerCutOff;//====

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;//漫反射贴图  
    sampler2D specular;   //vec3 specular;//镜面光照
    float shininess;//反光度
}; 

in vec3 Normal;//法线
in vec3 FragPos;//片段位置
in vec2 TexCoords;//纹理坐标

out vec4 FragColor;

uniform vec3 viewPos;//观察者位置
uniform Material material;//材质
uniform Light light;//光照

void main()
{
    //1.环境光 ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //2.环境光+漫反射 diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  

    //3.环境光+漫反射+镜面光照 specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

    //衰减 attenuation
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation; 

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}