const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 Position;
    layout (location = 1) in vec2 TexCoord;

    out vec2 Tex;

    uniform mat4 Model;
    uniform mat4 View;
    uniform mat4 Proj;

    void main()
    {
        gl_Position =  Proj * Model * vec4(Position,1.0f);
        Tex = TexCoord;
    }
)";
const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    in vec2 Tex;

    uniform sampler2D Texture;
    
    void main()
    {
        FragColor = texture(Texture,Tex);
    }
)";