#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Normal;
out vec3 v_FragPos;
out vec4 v_FragPosLightSpace[4];

// Directional light uniforms (needed in vertex shader for light space transform)
struct DirectionalLight {
	vec3 Direction;
	vec3 Color;
	float Intensity;
	mat4 LightSpaceMatrix;
};
uniform int u_NumDirectionalLights;
uniform DirectionalLight u_DirectionalLights[4];

void main() {
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
	
	// Calculate position in light space for each directional light
	for (int i = 0; i < u_NumDirectionalLights && i < 4; ++i) {
		v_FragPosLightSpace[i] = u_DirectionalLights[i].LightSpaceMatrix * vec4(v_FragPos, 1.0);
	}
	
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;
in vec4 v_FragPosLightSpace[4];

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform int u_UseTexture;
uniform vec2 u_TextureTiling;
uniform vec3 u_GI;

// Point light uniforms
struct PointLight {
	vec3 Position;
	vec3 Color;
	float Intensity;
	float Radius;
};
uniform int u_NumPointLights;
uniform PointLight u_PointLights[4];

// Directional light uniforms
struct DirectionalLight {
	vec3 Direction;
	vec3 Color;
	float Intensity;
	mat4 LightSpaceMatrix;
};
uniform int u_NumDirectionalLights;
uniform DirectionalLight u_DirectionalLights[4];
uniform sampler2D u_ShadowMaps[4];

// PCF shadow calculation
float CalculateShadow(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 lightDir, vec3 normal)
{
	// Perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	
	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	
	// Get closest depth value from light's perspective
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	
	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	
	// Check if outside shadow map bounds
	if (projCoords.z > 1.0)
		return 0.0;
	
	// Calculate bias to reduce shadow acne
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
	
	// PCF (Percentage Closer Filtering) for soft shadows
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;
	
	return shadow;
}

void main() {
	vec2 tiledTexCoord = v_TexCoord * u_TextureTiling;
	vec4 texColor = u_UseTexture == 1 ? texture(u_Texture, tiledTexCoord) : vec4(1.0);
	vec3 normal = normalize(v_Normal);
	vec3 baseColor = (u_Color * texColor).rgb;

	vec3 lighting = u_GI;
	
	// Point lights
	for (int i = 0; i < u_NumPointLights && i < 4; ++i) {
		vec3 lightDir = u_PointLights[i].Position - v_FragPos;
		float distance = length(lightDir);
		lightDir = normalize(lightDir);
		float diff = max(dot(normal, lightDir), 0.0);
		float attenuation = 1.0 / (1.0 + (distance / u_PointLights[i].Radius) * (distance / u_PointLights[i].Radius));
		lighting += u_PointLights[i].Color * diff * u_PointLights[i].Intensity * attenuation;
	}
	
	// Directional lights with shadows
	for (int i = 0; i < u_NumDirectionalLights && i < 4; ++i) {
		vec3 lightDir = normalize(-u_DirectionalLights[i].Direction);
		float diff = max(dot(normal, lightDir), 0.0);
		
		// Calculate shadow
		float shadow = CalculateShadow(v_FragPosLightSpace[i], u_ShadowMaps[i], lightDir, normal);
		
		// Apply lighting with shadow
		lighting += u_DirectionalLights[i].Color * diff * u_DirectionalLights[i].Intensity * (1.0 - shadow);
	}
	
	color = vec4(baseColor * lighting, (u_Color * texColor).a);
}

