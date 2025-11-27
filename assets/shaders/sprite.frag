#version 330 core

// Input from vertex shader
in vec2 vTexCoord;
in vec4 vColor;

// Uniforms
uniform sampler2D uTexture;
uniform bool uUseTexture;

// Output color
out vec4 FragColor;

void main()
{
    if (uUseTexture)
    {
        // Sample texture and apply color tint
        vec4 texColor = texture(uTexture, vTexCoord);
        FragColor = texColor * vColor;
    }
    else
    {
        // Use color only (for untextured sprites/shapes)
        FragColor = vColor;
    }
}
