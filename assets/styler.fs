#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

out vec4 finalColor;

void main()
{
    vec4 color = texture(texture0, fragTexCoord);

    vec2 uv = fragTexCoord;
    // center at 0
    vec2 centered = uv - 0.5;

    float vinDist = length(centered);
    float vin = smoothstep(0.8, 0.4, vinDist);

    float strength = 0.25;

    float dist = dot(centered, centered);

    // barrel distortion
    uv = centered * (1.0 + strength * dist) + 0.5;

    if(uv.x < 0 || uv.x > 1){
        finalColor = vec4(0,0,0,1);
        return;
    }

    vec4 retroColor = texture(texture0, uv);

    float scan = sin(fragTexCoord.y * 800.0) * 0.05;
    retroColor.rgb *= vin;
    retroColor.rgb -= scan;
    retroColor.rgb *= 0.95;

    finalColor = retroColor;
}
