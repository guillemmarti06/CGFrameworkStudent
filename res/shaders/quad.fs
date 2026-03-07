varying vec2 v_uv;

uniform float u_mode;
uniform float u_aspect;

void main()
{
    vec2 uv = v_uv;
    vec2 p = uv - vec2(0.5, 0.5);
    p.x *= u_aspect;

    vec3 color = vec3(0.0);

    // a) red-blue gradient
    if (u_mode < 0.5)
    {
        color = vec3(uv.x, 0.0, 1.0 - uv.x);
    }
    // b) radial dark center
    else if (u_mode < 1.5)
    {
        float d = distance(p, vec2(0.0, 0.0));
        float g = clamp(d * 1.5, 0.0, 1.0);
        color = vec3(g);
    }
    // c) red/blue grid glow
    else if (u_mode < 2.5)
    {
        float vx = abs(sin(uv.x * 30.0));
        float hy = abs(sin(uv.y * 30.0));

        float red = step(0.8, vx);
        float blue = step(0.8, hy);

        color = vec3(red, 0.0, blue);
    }
    // d) blocky gradient
    else if (u_mode < 3.5)
    {
        vec2 cell = floor(uv * 20.0) / 20.0;
        color = vec3(cell.x, cell.y, 0.0);
    }
    // e) checkerboard
    else if (u_mode < 4.5)
    {
        float cx = floor(uv.x * 16.0);
        float cy = floor(uv.y * 16.0);
        float checker = mod(cx + cy, 2.0);
        color = vec3(checker);
    }
    // f) green wave
    else
    {
        float wave = 0.5 + 0.25 * sin(uv.x * 6.28318);
        float mask = step(uv.y, wave);
        float glow = 1.0 - distance(p, vec2(0.0, 0.0)) * 1.2;
        glow = clamp(glow, 0.0, 1.0);

        vec3 bg = vec3(0.0, glow * 0.25, 0.0);
        vec3 fg = vec3(0.0, 0.8, 0.0);

        color = mix(bg, fg, mask);
    }

    gl_FragColor = vec4(color, 1.0);
}
