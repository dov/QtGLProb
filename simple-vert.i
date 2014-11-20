"uniform vec3 lightDir;\n"
"varying float intensity;\n"
"\n"
"void main()\n"
"{\n"
"  vec3 normal, lightDir;\n"
"  vec4 diffuse, ambient, globalAmbient;\n"
"  float NdotLplusAmbiant;\n"
"\n"
"  lightDir = normalize(vec3(gl_LightSource[0].position));\n"
"\n"
"  // Note that the gl_NormalMatrix is the inverse transpose of the\n"
"  // gl_ModelViewMatrix. See: http://www.arcsynthesis.org/gltut/Illumination/Tut09%20Normal%20Transformation.html\n"
"  normal = normalize(gl_NormalMatrix * gl_Normal);\n"
"  NdotLplusAmbiant = 0.8*max(dot(normal, lightDir), 0.0)+0.2;\n"
"\n"
"  gl_FrontColor =  vec4(gl_Color[0] * NdotLplusAmbiant,\n"
"                        gl_Color[1] * NdotLplusAmbiant,\n"
"                        gl_Color[2] * NdotLplusAmbiant,\n"
"                        1.0);\n"
"\n"
"  gl_Position = ftransform();\n"
"}\n"