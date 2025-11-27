#version 330 core

// Input vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec4 aColor;

// Uniform matrices
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

// Output to fragment shader
out vec2 vTexCoord;
out vec4 vColor;

void main()
{
    // Transform vertex position by MVP matrices
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);

    // Pass texture coordinates and color to fragment shader
    vTexCoord = aTexCoord;
    vColor = aColor;
}
