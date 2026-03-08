varying vec2 v_uv;
varying vec3 v_world_position;
varying vec3 v_world_normal;

uniform mat4 u_model;

uniform vec3 u_camera_position;
uniform vec3 u_ambient_intensity;

uniform vec3 u_light_position;
uniform vec3 u_light_intensity;

uniform vec3 u_ka;
uniform vec3 u_kd;
uniform vec3 u_ks;
uniform float u_shininess;

uniform sampler2D u_texture;
uniform sampler2D u_normal_texture;

uniform int u_use_color_texture;
uniform int u_use_specular_texture;
uniform int u_use_normal_texture;

void main()
{
    vec3 Ka = u_ka;
    vec3 Kd = u_kd;
    vec3 Ks = u_ks;

    vec4 tex = texture2D(u_texture, v_uv);

    if (u_use_color_texture == 1)
    {
        Ka = tex.rgb;
        Kd = tex.rgb;
    }

    if (u_use_specular_texture == 1)
    {
        Ks = vec3(tex.a);
    }

    vec3 N = normalize(v_world_normal);

    if (u_use_normal_texture == 1)
    {
        vec3 normal_tex = texture2D(u_normal_texture, v_uv).xyz;
        normal_tex = normal_tex * 2.0 - vec3(1.0);
        normal_tex = normalize((u_model * vec4(normal_tex, 0.0)).xyz);

        // suavizamos un poco mezclando con la normal interpolada
        N = normalize(mix(N, normal_tex, 0.5));
    }

    vec3 L_vector = u_light_position - v_world_position;
    float dist2 = max(dot(L_vector, L_vector), 0.0001);
    vec3 L = normalize(L_vector);

    vec3 V = normalize(u_camera_position - v_world_position);
    vec3 R = reflect(-L, N);

    vec3 ambient = Ka * u_ambient_intensity;

    float NdotL = clamp(dot(N, L), 0.0, 1.0);
    vec3 diffuse = Kd * NdotL;

    vec3 specular = vec3(0.0);
    if (NdotL > 0.0)
    {
        float RdotV = clamp(dot(R, V), 0.0, 1.0);
        specular = Ks * pow(RdotV, u_shininess);
    }

    vec3 Ip = ambient + (u_light_intensity / dist2) * (diffuse + specular);

    gl_FragColor = vec4(clamp(Ip, 0.0, 1.0), 1.0);
}
