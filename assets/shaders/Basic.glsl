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

void main() {
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
	v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
	gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec4 u_Color;
uniform sampler2D u_Texture;
uniform int u_UseTexture;
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

void main() {
	vec4 texColor = u_UseTexture == 1 ? texture(u_Texture, v_TexCoord) : vec4(1.0);
	vec3 normal = normalize(v_Normal);
	vec3 baseColor = (u_Color * texColor).rgb;

	vec3 lighting = u_GI;
	for (int i = 0; i < u_NumPointLights && i < 4; ++i) {
		vec3 lightDir = u_PointLights[i].Position - v_FragPos;
		float distance = length(lightDir);
		lightDir = normalize(lightDir);
		float diff = max(dot(normal, lightDir), 0.0);
		float attenuation = 1.0 / (1.0 + (distance / u_PointLights[i].Radius) * (distance / u_PointLights[i].Radius));
		lighting += u_PointLights[i].Color * diff * u_PointLights[i].Intensity * attenuation;
	}
	color = vec4(baseColor * lighting, (u_Color * texColor).a);
}
