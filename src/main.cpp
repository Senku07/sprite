#include <stdio.h>
#include "glad.h"
#include "glad.c"
#include "glfw3.h"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Image.h"
#include "glm/common.hpp"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

typedef unsigned int UI;
UI Program;

struct mount
{
    UI VAO;
    UI VBO;
    UI Texture;
};

mount MountData(const char * texFile)
{
    mount result = {};
    float BackG[] =
        {
            -0.2f,
            -0.2f,
            0.0f,
            0.0f,
            0.0f,
            0.2f,
            -0.2f,
            0.0f,
            1.0f,
            0.0f,
            .2f,
            .2f,
            0.0f,
            1.0f,
            1.0f,

            0.2f,
            0.2f,
            0.0f,
            1.0f,
            1.0f,
            -0.2f,
            0.2f,
            0.0f,
            0.0f,
            1.0f,
            -0.2f,
            -.2f,
            0.0f,
            0.0f,
            0.0f,
        };

    glGenVertexArrays(1, &result.VAO);
    glBindVertexArray(result.VAO);

    glGenBuffers(1, &result.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, result.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(BackG), BackG, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(sizeof(float) * 3));

    glGenTextures(1, &result.Texture);
    glBindTexture(GL_TEXTURE_2D, result.Texture);
    int w, h, c;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texFile, &w, &h, &c, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        printf("failed to upload texture0");
    };
    stbi_image_free(data);

    return result;
}
void DrawMount(float x, float y,float w,float h, mount a)
{
    glBindVertexArray(a.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, a.VBO);
    glBindTexture(GL_TEXTURE_2D, a.Texture);

    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(x, y, 0));
    Model = glm::scale(Model,glm::vec3(w,h,0));
    glUniformMatrix4fv(glGetUniformLocation(Program, "Model"), 1, GL_FALSE, glm::value_ptr(Model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawSprite(float x, float y, float w, float h, mount a, uint32_t index) 
{
   float Xmin = (index*128.0f)/1024.0f;
   float Ymin = 0.0f;
   float Xmax = 128.0f/1024.0f;
   float Ymax = 1.0f;
    float Pos[] =
    {
        0.0f,0.0f,0.0f,  Xmin,Ymin,
        0.0f,1.0f,0.0f,  Xmin,Ymax,
        1.0f,1.0f,0.0f,  Xmax + Xmin,Ymax,
        1.0f,1.0f,0.0f,  Xmax + Xmin,Ymax,    
        1.0f,0.0f,0.0f,  Xmax+Xmin,Ymin,       
        0.0f,0.0f,0.0f,  Xmin,Ymin,
    };
    
    glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(Pos),Pos);
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(x, y, 0));
    Model = glm::scale(Model,glm::vec3(w,h,0));
    
    glBindVertexArray(a.VAO);
    glBindBuffer(GL_ARRAY_BUFFER,a.VBO);
    glBindTexture(GL_TEXTURE_2D,a.Texture);
    glUniformMatrix4fv(glGetUniformLocation(Program, "Model"), 1, GL_FALSE, glm::value_ptr(Model));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(1000, 600, "Sprit", 0, 0);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    UI vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
    glCompileShader(vertexShader);

    char infolog[1000];
    glGetShaderInfoLog(vertexShader, 1000, 0, infolog);

    printf("%s", infolog);

    UI fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);
    glCompileShader(fragmentShader);

    glGetShaderInfoLog(fragmentShader, 1000, 0, infolog);

    printf("%s", infolog);

    Program = glCreateProgram();
    glAttachShader(Program, vertexShader);
    glAttachShader(Program, fragmentShader);
    glLinkProgram(Program);
    glUseProgram(Program);

    glm::mat4 Proj(1.0f);
    Proj = glm::ortho(0.0f, 1000.0f, 0.0f, 600.0f);
    glUniformMatrix4fv(glGetUniformLocation(Program, "Proj"), 1, GL_FALSE, glm::value_ptr(Proj));

    glm::mat4 View;
    View = glm::translate(View, glm::vec3(0, 0,2));
    glUniformMatrix4fv(glGetUniformLocation(Program, "View"), 1, GL_FALSE, glm::value_ptr(View));

    mount a = MountData("../snowy_rocks1/snowy_rock1.png");
    mount b = MountData("../snowy_rocks1/snowy_rock2.png");
    mount c = MountData("../snowy_rocks1/snowy_rock3.png");
    mount d = MountData("../snowy_rocks1/snowy_rock2.png");
    mount e = MountData("../snowy_rocks1/snowy_rock1.png");
    
    mount f = MountData("../snowy_rocks1/snowy_rock5.png");
    mount g = MountData("../snowy_rocks1/snowy_rock5.png");
    mount h = MountData("../snowy_rocks1/snowy_rock5.png");

    mount r = MountData("../im/Knight_1/Walk.png");
    float posX = 0;
    float posY = 300; 
    float index = 0;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1,1,1,1);
        glEnable(GL_BLEND);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if(glfwGetKey(window,GLFW_KEY_A) == GLFW_PRESS)
        {
            index += 0.01;
            posX += 0.1;
        }
        else{
            index = 0;
        }

        DrawSprite(posX,posY,100,100,r,index);
        DrawSprite(500,500,100,100,r,3);
        DrawSprite(0,0,100,100,r,0);
        DrawSprite(100,100,100,100,r,1);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 07;
}