varying vec2 v_uv;

uniform mat4 u_model;
uniform mat4 u_viewprojection;

void main()
{
    v_uv = gl_MultiTexCoord0.xy;
    gl_Position = u_viewprojection * u_model * gl_Vertex;
}
