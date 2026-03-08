varying vec2 v_uv;

uniform float u_mode;
uniform float u_aspect;
uniform float u_time;
uniform sampler2D u_texture;

void main()
{
    vec2 uv = v_uv;
    vec2 p = uv - vec2(0.5, 0.5);
    p.x *= u_aspect;

    vec3 color = vec3(0.0);

    // -----------------------------
    // 2.2 FORMULAS
    // -----------------------------

    if (u_mode < 0.5)
    {
        color = vec3(uv.x, 0.0, 1.0 - uv.x);
    }
    else if (u_mode < 1.5)
    {
        float d = distance(p, vec2(0.0, 0.0));
        float g = clamp(d * 1.5, 0.0, 1.0);
        color = vec3(g);
    }
    else if (u_mode < 2.5)
    {
        float vx = step(0.8, abs(sin(uv.x * 30.0)));
        float hy = step(0.8, abs(sin(uv.y * 30.0)));
        color = vec3(vx, 0.0, hy);
    }
    else if (u_mode < 3.5)
    {
        vec2 cell = floor(uv * 20.0) / 20.0;
        color = vec3(cell.x, cell.y, 0.0);
    }
    else if (u_mode < 4.5)
    {
        float cx = floor(uv.x * 16.0);
        float cy = floor(uv.y * 16.0);
        float checker = mod(cx + cy, 2.0);
        color = vec3(checker);
    }
    else if (u_mode < 5.5)
    {
        float wave = 0.5 + 0.25 * sin(uv.x * 6.28318);
        float mask = step(uv.y, wave);

        float glow = 1.0 - distance(p, vec2(0.0, 0.0)) * 1.2;
        glow = clamp(glow, 0.0, 1.0);

        vec3 bg = vec3(0.0, glow * 0.25, 0.0);
        vec3 fg = vec3(0.3, 0.7, 0.2);

        color = mix(bg, fg, mask);
    }

    // -----------------------------
    // 2.3 IMAGE FILTERS
    // -----------------------------

    else if (u_mode < 6.5)
    {
        vec3 tex = texture2D(u_texture, uv).rgb;
        float gray = dot(tex, vec3(0.299, 0.587, 0.114));
        color = vec3(gray);
    }
    else if (u_mode < 7.5)
    {
        vec3 tex = texture2D(u_texture, uv).rgb;
        color = vec3(1.0) - tex;
    }
    else if (u_mode < 8.5)
    {
        vec3 tex = texture2D(u_texture, uv).rgb;
        vec3 yellow = vec3(tex.r, tex.g, 0.0);
        color = mix(tex * 0.4, yellow, 0.8);
    }
    else if (u_mode < 9.5)
    {
        vec3 tex = texture2D(u_texture, uv).rgb;
        float gray = dot(tex, vec3(0.299, 0.587, 0.114));
        float bw = step(0.5, gray);
        color = vec3(bw);
    }
    else if (u_mode < 10.5)
    {
        vec3 tex = texture2D(u_texture, uv).rgb;
        float d = distance(p, vec2(0.0, 0.0));
        float vignette = 1.0 - clamp(d * 1.4, 0.0, 1.0);
        color = tex * vignette;
    }
    else if (u_mode < 11.5)
    {
        vec2 pixel = vec2(1.0 / 512.0, 1.0 / 512.0);

        vec3 c = vec3(0.0);
        c += texture2D(u_texture, uv + pixel * vec2(-1.0, -1.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 0.0, -1.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 1.0, -1.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2(-1.0,  0.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 0.0,  0.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 1.0,  0.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2(-1.0,  1.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 0.0,  1.0)).rgb;
        c += texture2D(u_texture, uv + pixel * vec2( 1.0,  1.0)).rgb;

        color = c / 9.0;
    }

    // -----------------------------
    // 2.4 IMAGE TRANSFORMATIONS
    // -----------------------------

    else if (u_mode < 12.5)
    {
        // Animated pixelization
        float cells = 20.0 + 18.0 * (0.5 + 0.5 * sin(u_time));
        vec2 pixel_uv = floor(uv * cells) / cells;
        color = texture2D(u_texture, pixel_uv).rgb;
    }
    else
    {
        // Animated rotation
        float angle = u_time;
        float c = cos(angle);
        float s = sin(angle);

        vec2 centered = uv - vec2(0.5, 0.5);

        mat2 rot = mat2(c, -s,
                        s,  c);

        vec2 rotated = rot * centered;
        vec2 final_uv = rotated + vec2(0.5, 0.5);

        color = texture2D(u_texture, final_uv).rgb;
    }

    gl_FragColor = vec4(color, 1.0);
}
