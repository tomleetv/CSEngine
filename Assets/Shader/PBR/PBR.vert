#version 300 es

//#version 100
precision highp float;
precision highp int;

#define MAX_LIGHTS 16
#define MAX_WEIGHTS 3
#define MAX_JOINTS 60

struct U_LightSource {
    vec4 u_diffuseLight;
    vec4 u_ambientLight;
    vec4 u_specularLight;

    int u_lightMode;
    int u_isDirectional;

    /**Attenuation Factor**/
    int u_isAttenuation;
    vec3 u_attenuationFactor;
    float u_lightRadius;

};


// Attributes
//[POSITION]//
in vec4 a_position;
//[NORMAL]//
in vec3 a_normal;
in vec3 a_diffuseMaterial;
//[JOINT_INDICES]//
in vec3 a_jointIndices;
//[WEIGHTS]//
in vec3 a_weights;
//[TEX_UV]//
in vec2 a_textureCoordIn;

// Uniforms
//[PROJECTION_MATRIX]//
uniform mat4 u_projectionMatrix;//Projection;
//[MODELVIEW_MATRIX]//
uniform mat4 u_modelViewMatrix;//Modelview;
//[MODELVIEW_NOCAMERA_MATRIX]//
uniform mat4 u_modelViewNoCameraMatrix;//Modelview - no camera matrix;
//[NORMAL_MATRIX]//
uniform mat3 u_normalMatrix;//NormalMatrix;
//[LIGHT_POSITION]//
uniform vec4 u_lightPosition[MAX_LIGHTS];//LightPosition;
//[JOINT_MATRIX]//
uniform mat4 u_jointMatrix[MAX_JOINTS];

//[SKINNING_MODE]//
uniform lowp int u_isSkinning;
uniform lowp int u_isDirectional[MAX_LIGHTS];
uniform U_LightSource u_lightSources[MAX_LIGHTS];
uniform lowp int u_lightsSize;


// Varying
out vec3 v_eyespaceNormal;//EyespaceNormal
out vec3 v_diffuse;//Diffuse;
out vec3 v_lightPosition[MAX_LIGHTS];
out vec2 v_textureCoordOut;
out float v_distance[MAX_LIGHTS];
//varying vec3 v_vertPosition;


//defined
const lowp float c_zero = 0.0;
const lowp float c_one = 1.0;

void main(void) {
	vec4 position_final = vec4(0.0);
	vec4 normal_final = vec4(0.0);

	//skinning
    if(u_isSkinning == 1) {
        vec4 totalNormal = vec4(0.0);

        for(int i=0; i<MAX_WEIGHTS; i++) {
            mat4 jointTransform = u_jointMatrix[int(a_jointIndices[i])];
            vec4 posePosition = jointTransform * a_position;
            position_final += posePosition * a_weights[i];

            vec4 worldNormal = jointTransform * vec4(a_normal, 0.0);
            normal_final += worldNormal * a_weights[i];
        }
        position_final = vec4(position_final.xyz, 1.0);
    } else {
        position_final = a_position;
        normal_final = vec4(a_normal, 0);
    }


	v_eyespaceNormal = u_normalMatrix * normal_final.xyz;
	v_diffuse = a_diffuseMaterial;
	v_textureCoordOut = a_textureCoordIn;


    for(int i = 0; i < u_lightsSize; i++) {
        	vec4 positionLight;
        	vec4 directionLight = u_lightPosition[i];

        	//direction & position light
        	if(u_lightSources[i].u_isDirectional == 1) {
        		positionLight = vec4(c_zero, c_zero, c_zero, c_one);
        	}else {
        		positionLight = u_modelViewNoCameraMatrix * position_final;
        	}

        	//for positional light
        	vec3 aux = vec3(directionLight - positionLight);

        	// Varying
        	v_lightPosition[i] = normalize(aux);
        	v_distance[i] = length(aux);
    }

	//vec4 vertPosition = u_modelViewMatrix * a_position;
	//v_vertPosition = vec3(vertPosition) / vertPosition.w;


    //vertex position
    gl_Position = u_projectionMatrix * u_modelViewMatrix * position_final;

}