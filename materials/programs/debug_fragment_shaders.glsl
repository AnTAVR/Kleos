#version 330 core
precision highp float;
in vec2 TexCoord;
in vec4 vColor;

uniform sampler2D tex;

out vec4 color;

void main()
{
    /* Using this as a work around to make it so that the variables doesn't get
     * optimized out. It was causing INVALID_OPERATIONS
     * not 100% sure if this is the issue.
     * complaining for "uniform mat4 view"
     * doing this fails glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(g_camera->view));
     */
    color = vColor;
    color.r += texture(tex, TexCoord).r * .4;
    color.g += texture(tex, TexCoord).g * .4;
    color.b += texture(tex, TexCoord).b * .4;
}
