varying vec3 v_color;

uniform mat4 u_model;
uniform mat4 u_viewprojection;

uniform vec3 u_camera_position;
uniform vec3 u_ambient_intensity;

uniform vec3 u_light_position;
uniform vec3 u_light_intensity;

uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;

void main()
{
    vec3 world_position = (u_model * vec4(gl_Vertex.xyz, 1.0)).xyz;
    vec3 N = normalize((u_model * vec4(gl_Normal.xyz, 0.0)).xyz);

    vec3 L_vector = u_light_position - world_position;
    float dist2 = max(dot(L_vector, L_vector), 0.0001);
    vec3 L = normalize(L_vector);

    vec3 V = normalize(u_camera_position - world_position);
    vec3 R = reflect(-L, N);

    vec3 ambient = u_ka * u_ambient_intensity;

    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuse = u_kd * NdotL;

    vec3 specular = vec3(0.0);
    if (NdotL > 0.0)
    {
        float RdotV = clamp(dot(R, V), 0.0, 1.0);
        specular = u_ks * pow(RdotV, u_shininess);
    }

    vec3 Ip = ambient + (u_light_intensity / dist2) * (diffuse + specular);

    v_color = clamp(Ip, 0.0, 1.0);

    gl_Position = u_viewprojection * vec4(world_position, 1.0);
}
