#version 330

void main()
{
   if(gl_VertexID == 0)
   {
      gl_Position = vec4(-0.5, 0.5, 0, 1);
   }
   else if(gl_VertexID == 1)
   {
      gl_Position = vec4(0.5, 0.5, 0, 1);
   }
   else
   {
      gl_Position = vec4(0.5, -0.5, 0, 1);
   }
}