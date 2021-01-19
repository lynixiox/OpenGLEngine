#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
 #define TOTALLIGHTS 128

uniform vec3 viewPos;
uniform Material material;
uniform Light light;
uniform Light lights[TOTALLIGHTS];
uniform int amountOfLights;

void main()
{
    // ambient
	float distance = length(light.position - FragPos);
	float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.016* (distance * distance));    

    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  
     
	  vec3 result = ambient + diffuse + specular;
	for(int i = 0; i < amountOfLights;i++)
	{
	    distance = length(lights[i].position - FragPos);
		attenuation = 1.0 / (1.0 + 0.09 * distance + 0.016* (distance * distance));    


			 // ambient
		ambient = lights[i].ambient * texture(material.diffuse, TexCoords).rgb;
  	
		// diffuse 
		lightDir = normalize(lights[i].position - FragPos);
		diff = max(dot(norm, lightDir), 0.0);
		diffuse = lights[i].diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
		// specular
		reflectDir = reflect(-lightDir, norm);  
		spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		specular = lights[i].specular * spec * texture(material.specular, TexCoords).rgb;  
     
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		 vec3 result2 = ambient + diffuse + specular;

		 result += result2;
	}
    FragColor = vec4(result, 1.0);
} 